#include "PageLayer.h"
#include "SimpleAudioEngine.h"
#include "PageManager.h"
#include "Configurations.h"
#include "MainMenuLayer.h"
#include "SharedGlobalData.h"
#include "ChipmunkLayer.h"
#include "VideoPlayer.h"

#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

// the distance(in points) to determine swipe left or right
#define TOLERATE_VERTICAL_DISTANCE      150
#define TOLERATE_HORIZONTAL_DISTANCE    30
// page layer tag
#define PARAGRAPH_LAYER_TAG       10
#define PAGELAYER_HANDLER_PRIORITY 12

#define STORY_TOUCH_DETECT_TAG          11
#define PARAGRAPH_HOTSPOT_DETECT_TAG    12

#define XSCALE          (GlobalData::sharedGlobalData()->xScale)
#define YSCALE          (GlobalData::sharedGlobalData()->yScale)
#define WORD_SPACING    (1.5 * XSCALE)

PageLayer::PageLayer()
: currentIndexOfParagraph(0)
, beginPoint(0, 0)
, page(NULL)
, paragraphLayer(NULL)
, mydialog(NULL)
, delayOfAnimation(0)
, storyTouchDetector(NULL)
, touchSoundId(0)
, isVideoPlaying(false)
, isHighLighting(false)
, isPlayingAnimation(false)
{}

PageLayer* PageLayer::pageLayerWithPage(Page* page)
{
	PageLayer *layer = new PageLayer();
	layer->autorelease();
    
    layer->init(page);
	
	return layer;
}

PageLayer::~PageLayer()
{
    // resources is release in onExit()
    CC_SAFE_RELEASE_NULL(mydialog);
    CC_SAFE_RELEASE_NULL(storyTouchDetector);
}

void PageLayer::init(Page *page)
{
    if( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)) ) //RGBA
    {
        // below line not working
        // return false;
    }
        
    this->page = page;
    
    // preload backgound music
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(page->settings.audioFilePath.c_str());
    // preload effect used to speak paragraph
    for (int i = 0; i < page->paragraphs.size(); ++i)
    {
        SimpleAudioEngine::sharedEngine()->preloadEffect(page->paragraphs[i]->voiceAudioFile.c_str());
    }
    
    this->delayOfAnimation = this->calculateDelayTimeOnEnter();
    if (this->delayOfAnimation > 0)
    {
        isPlayingAnimation = true;
        CCAction *seq = CCSequence::actions(CCDelayTime::actionWithDuration(delayOfAnimation),
                                               CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::animationDelayCallback)),
                                               NULL);
        this->runAction(seq);
        
        // should skip this action to invoke animationDelayCallback() to set "isPlayingAnimation" to false
        addActionsToBeSkipped(seq);
    }
    
    createSprites();
    createPhysicsLayer();
	createParagraph(0);
    
    // add menu item to go to main menu
    createMainMenuItem();
}

void PageLayer::animationDelayCallback(cocos2d::CCObject *sender)
{
    isPlayingAnimation = false;
}

void PageLayer::onEnter()
{
    addStoryTouchNode();
    
    setIsTouchEnabled(true);
    
    CCLayer::onEnter();
}

void PageLayer::addStoryTouchNode()
{
    // add touchable nodes
	storyTouchDetector = TouchDetection::node();
    storyTouchDetector->retain();
    
	vector<StoryTouchableNode*>::iterator iter;
	for (iter = page->storyTouchableNodes.begin(); iter != page->storyTouchableNodes.end(); ++iter)
	{
		StoryTouchableNode* storyTouchNode = *iter;
		storyTouchDetector->addZoneWithPositionRadiusTargetSel(storyTouchNode->hotspotInfo.position, storyTouchNode->hotspotInfo.radius, this, schedule_selector(PageLayer::storyTouchCallback), storyTouchNode->hotspotInfo.touchFlag);
        
        // add particle system
        CCParticleSystem *particle = NULL;
        if (storyTouchNode->hotspotInfo.glitterIndicator)
        {
            particle = CCParticleSystemQuad::particleWithFile("Flower.plist");
            particle->setPosition(storyTouchNode->hotspotInfo.position);
            particle->initWithTotalParticles(6);
            addChild(particle);
        }
        
        // preload effect if exists
        SimpleAudioEngine::sharedEngine()->preloadEffect(storyTouchNode->hotspotInfo.soundToPlay.c_str());
        
        // delay for animation
        if (storyTouchNode->delayForAnimation)
        {
            storyTouchDetector->enableTouchByFlag(storyTouchNode->hotspotInfo.touchFlag, false);
            if (particle)
            {
                particle->setIsVisible(false);
                particle->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(delayOfAnimation),
                                                        CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::enableDelayForAnimationTouchNode)),
                                                        CCShow::action(),
                                                        NULL));
            }
        }
        
        // delay for text
        if (storyTouchNode->delayForText
            && (MainMenuLayer::storyMode == kStoryModeReadToMe
                || (MainMenuLayer::storyMode == kSotryModeAutoPlay)))
        {
            storyTouchDetector->enableTouchByFlag(storyTouchNode->hotspotInfo.touchFlag, false);
            if (particle)
            {
                particle->setIsVisible(false);
            }
        }
	}
    
    storyTouchDetector->setTag(STORY_TOUCH_DETECT_TAG);
    addChild(storyTouchDetector);
    storyTouchDetector->setIsTouchEnabled(true);
}

void PageLayer::enableDelayForAnimationTouchNode(CCObject *sender)
{
    std::vector<StoryTouchableNode*> &storyTouchableNodes = page->storyTouchableNodes;
    vector<StoryTouchableNode*>::iterator iter;
    for (iter = storyTouchableNodes.begin(); iter != storyTouchableNodes.end(); ++iter)
    {
        if ((*iter)->delayForAnimation)
        {
            storyTouchDetector->enableTouchByFlag((*iter)->hotspotInfo.touchFlag, true);
        }
    }
}

void PageLayer::enableDelayForTextTouchNode()
{   
    std::vector<StoryTouchableNode*> &storyTouchableNodes = page->storyTouchableNodes;
    vector<StoryTouchableNode*>::iterator iter;
    for (iter = storyTouchableNodes.begin(); iter != storyTouchableNodes.end(); ++iter)
    {
        if ((*iter)->delayForText)
        {
            storyTouchDetector->enableTouchByFlag((*iter)->hotspotInfo.touchFlag, true);
            if ((*iter)->hotspotInfo.particle)
            {
                (*iter)->hotspotInfo.particle->setIsVisible(true);
            }
        }
    }
}

void PageLayer::onExit()
{
    CC_SAFE_RELEASE_NULL(paragraphLayer);
    
    // unload effect
    for (int i = 0; i < page->paragraphs.size(); ++i)
    {
        SimpleAudioEngine::sharedEngine()->unloadEffect(page->paragraphs[i]->voiceAudioFile.c_str());
    }
    
    CCLayer::onExit();
}

void PageLayer::storyTouchCallback(float flag)
{
    StoryTouchableNode *storyTouchableNode = page->getSotryTouchableNode(flag);
    doHotspotTouched(&storyTouchableNode->hotspotInfo, false);
}

void PageLayer::doHotspotTouched(HotspotInfo *hotspot,bool isParagraphHotspot)
{
    // stop all sounds and stop highlight
    if (hotspot->stopSoundAndHighlightingWhenTouched)
    {
        SimpleAudioEngine::sharedEngine()->stopAllBackgroundMusic();
        SimpleAudioEngine::sharedEngine()->stopAllEffects();
        stopHighlightParagraph();
    }
    
    // play video
    bool showControl = true;
    string &videoName = hotspot->videoToPlay;
    if (videoName.size() != 0)
    {
        if (MainMenuLayer::storyMode == kSotryModeAutoPlay)
        {
            VideoPlayer::sharedVideoPlayer()->playVideoByFilename(videoName.c_str(), showControl, isParagraphHotspot, this);
        }
        else
        {
            VideoPlayer::sharedVideoPlayer()->playVideoByFilename(videoName.c_str(), showControl, isParagraphHotspot);
        }
        
        isVideoPlaying = true;
    }
    
    // play audeo
    string &audeoName = hotspot->soundToPlay;
    if (audeoName.size() != 0)
    {
        // stop first
        SimpleAudioEngine::sharedEngine()->stopEffect(touchSoundId);
        // play
        touchSoundId = SimpleAudioEngine::sharedEngine()->playEffect(audeoName.c_str());
    }
}

// this call back function will be called by VideoPlayer only with auto play mode
void PageLayer::moviePlayBackDidFinish(const char *videoName, bool isParagraphHotspot)
{
    isVideoPlaying = false;
    
    if (isHighLighting)
    {
        return;
    }
    else
    {
        HotspotInfo* hotspotInfo = getHotspotInfo(videoName, isParagraphHotspot);
        assert(hotspotInfo != NULL);
        
        if (hotspotInfo->delayAfterVideoDuringAutoplay > 0)
        {
            // do some delay to swipe left and should disable touch node and hide particle to play video again
            storyTouchDetector->enableTouchByFlag(hotspotInfo->touchFlag, false);
            if (hotspotInfo->particle)
            {
                hotspotInfo->particle->setIsVisible(false);
            }
            
            // swipe left after a delay
            this->runAction(CCSequence::actions(
                                                CCDelayTime::actionWithDuration(hotspotInfo->delayAfterVideoDuringAutoplay),
                                                CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::delaySwipeAfterPlayingVideo)),
                                                NULL)
                            );
        }
        else
        {
            swipeLeft();
        } 
    }
}

HotspotInfo* PageLayer::getHotspotInfo(const string &videoName, bool isParagraphHotspot)
{
    if (isParagraphHotspot)
    {
        // paragraph hotspot is touched
        return page->getParagraphHotspotInfo(currentIndexOfParagraph, videoName);
    }
    else
    {
        StoryTouchableNode *storyTouchableNode = page->getStoryTouchableNode(videoName);
        if (storyTouchableNode)
        {
            return &storyTouchableNode->hotspotInfo;
        }
        else
        {
            return NULL;
        }
    }
}

void PageLayer::delaySwipeAfterPlayingVideo(cocos2d::CCObject *sender)
{
    swipeLeft();
}

/// touch event
void PageLayer::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, PAGELAYER_HANDLER_PRIORITY, false);
}

void PageLayer::onEnterTransitionDidFinish()
{   
    playBackgroundMusic();  
    //float delay = calculateDelayTimeOnEnter();
    showParagraph(delayOfAnimation);
    
    CCLayer::onEnterTransitionDidFinish();
}

// some sprites may run action when onEnter
// so should calcuate delay time for showing paragraph and touch zones
float PageLayer::calculateDelayTimeOnEnter()
{
    float delay = 0.0f;
    
    // return the longest delay
    vector<SpriteInfo*> &sprites = page->sprites;
    for (int i = 0; i < sprites.size(); ++i)
    {
        vector<int> &actions = sprites[i]->actions;
        for (int j = 0; j < actions.size(); ++j)
        {
            CCAction *action = page->getActionByTag(actions[j]);
            if (dynamic_cast<CCMoveTo*>(action) != NULL || dynamic_cast<CCScaleTo*>(action) != NULL ||  dynamic_cast<CCDelayTime*>(action) != NULL || dynamic_cast<CCSequence*>(action) != NULL ||  dynamic_cast<CCSpawn*>(action) != NULL)
            {
                float temDelay = ((CCFiniteTimeAction*)action)->getDuration();
                if (temDelay > delay)
                {
                    delay = temDelay;
                }
            }
        }
    }
    
    return delay;
}

bool PageLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    // record touch began position
    beginPoint = pTouch->locationInView(pTouch->view());
    beginPoint = CCDirector::sharedDirector()->convertToGL(beginPoint);
    
    return true;
}

void PageLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    // do not allow user to turn pages during autoplay
    if (MainMenuLayer::storyMode == kSotryModeAutoPlay)
    {
        return;
    }
    
    // skip animations if user "skipAnimationOnSwipe" is true
    if (isPlayingAnimation == true)
    {
        if (Configurations::skipAnimationOnSwipe)
        {
            skipAnimation();
        }
 
        return;
    }
    
    CCPoint endPoint = pTouch->locationInView(pTouch->view());
    endPoint = CCDirector::sharedDirector()->convertToGL(endPoint);
    
    if (isSwipeLeft(beginPoint, endPoint)) 
    {
        swipeLeft();
    }
    
    if (isSwipeRight(beginPoint, endPoint)) 
    {
        swipeRight();
    }
}

float PageLayer::swipeEndedOperationAndCalculateTotalDelay(bool isSwipeLeft)
{
    StorySwipeEnded &swipeEnded = page->storySwipeEnded;
    /*
     should add children and remove children every time swiping ended
     
     // add child
     vector<int> &spritesToAdd = swipeEnded.spritesToAdd;
     vector<int>::iterator iter;
     for (iter = spritesToAdd.begin(); iter != spritesToAdd.end(); ++iter)
     {
     CCSprite *sprite = page->getSpriteInfoByTag(*iter);
     addChild(sprite);
     }
     
     // remove child
     vector<int> &spritesToRemove = swipeEnded.spritesToMove;
     for (iter = spritesToRemove.begin(); iter != spritesToRemove.end(); ++iter)
     {
     CCSprite *sprite = page->getSpriteInfoByTag(*iter);
     removeChildByTag(sprite->getTag(), true);
     }
     */
    
    // run actions
    float delay = 0.0f;
    
    vector<StorySwipeEndedActionsToRun*> *swipeEndedActionsToRun = page->getStorySwipeEndedActionToRun(currentIndexOfParagraph);
    
    if (swipeEndedActionsToRun)
    {
        for (int i = 0; i < swipeEndedActionsToRun->size(); ++i) 
        {
            StorySwipeEndedActionsToRun* actionToRun = (*swipeEndedActionsToRun)[i];
            
            // get sprite
            CCSprite *sprite = (CCSprite*)getChildByTag(actionToRun->spriteTag);
            assert(sprite != NULL);
            
            // get action
            CCFiniteTimeAction *action;
            if (actionToRun->actionTags.size() > 1)
            {
                // spawn action
                
                CCArray *array = CCArray::array();
                for (int j = 0; j < actionToRun->actionTags.size(); ++j)
                {
                    // the action should be CCScaleBy, CCScaleTo, CCMoveBy or CCMoveTo
                    CCAction *element = page->getActionByTag(actionToRun->actionTags[j]);
                    assert(dynamic_cast<CCScaleBy*>(element) != NULL ||
                           dynamic_cast<CCScaleTo*>(element) != NULL ||
                           dynamic_cast<CCMoveBy*>(element) != NULL ||
                           dynamic_cast<CCMoveTo*>(element) != NULL);
                    
                    array->addObject(element);
                    
                    
                }
                action = CCSpawn::actionsWithArray(array);                
            }
            else 
            {
                action = (CCFiniteTimeAction*)page->getActionByTag(actionToRun->actionTags[0]);
                assert(dynamic_cast<CCScaleBy*>(action) != NULL ||
                       dynamic_cast<CCScaleTo*>(action) != NULL ||
                       dynamic_cast<CCMoveBy*>(action) != NULL ||
                       dynamic_cast<CCMoveTo*>(action) != NULL);
            }
            
            // caculate delay time
            delay = ((CCFiniteTimeAction*)action)->getDuration();
            
            if (isSwipeLeft)
            {
                // swipe left
                CCFiniteTimeAction* seq = CCSequence::actions(action, 
                                                              CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::swipEndCallBack)),
                                                              NULL);
                sprite->runAction(seq);
                
                // should skip this action to invoke swipEndCallBack() to set "isPlayingAnimation" to false
                addActionsToBeSkipped(seq);
            }
            else 
            {
                // swipe right
                // run reverse action
                CCFiniteTimeAction* seq = CCSequence::actions(action->reverse(), 
                                                              CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::swipEndCallBack)),
                                                              NULL);
                sprite->runAction(seq);
                
                // should skip this action to invoke swipEndCallBack() to set "isPlayingAnimation" to false
                addActionsToBeSkipped(seq);
            }
            
            // record actoin to be skipped
            addActionsToBeSkipped(action);
            
            // start swipping, then don't do swipe operation until done
            isPlayingAnimation= true;
        }
        
        delete swipeEndedActionsToRun;
    }
    
    return delay;
}

void PageLayer::swipEndCallBack()
{
    // swipping ended
    isPlayingAnimation = false;
}

void PageLayer::swipeLeft()
{
    stopHighlighVoiceAndOtherEffect();
    
    if (currentIndexOfParagraph == (page->paragraphs.size() - 1))
    {
        PageManager::turnToPage(page->settings.number + 1, false);
    }
    else 
    {
        // increase current index of paragraph
        ++currentIndexOfParagraph;
        
        float delay = swipeEndedOperationAndCalculateTotalDelay(true);
                
        createParagraph(currentIndexOfParagraph);
        
        showParagraph(delay);
    }
}

void PageLayer::swipeRight()
{
    if (page->settings.number == 1 &&
        currentIndexOfParagraph == 0)
    {
        return;
    }
    
    stopHighlighVoiceAndOtherEffect();
    
    if (currentIndexOfParagraph == 0)
    {
        PageManager::turnToPage(page->settings.number - 1, true);
    }
    else 
    {
        float delay = swipeEndedOperationAndCalculateTotalDelay(false);
        
        // decrease current index of paragraph
        // should decrease index after swipeEndedOperationAndCalculateTotalDelay, because it will
        // run reverse action of current index
        --currentIndexOfParagraph;

        createParagraph(currentIndexOfParagraph);
        
        showParagraph(delay);
    }
}

bool PageLayer::isSwipeLeft(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos)
{
    // if there is physical object, should check horizontal distance,
    // because we should determine if it is dragging physical object or swipping
    if (page->settings.fallingObjectSetting.plistfilename.size() > 0)
    {
        if ((beginPos.x - endPos.x) > TOLERATE_HORIZONTAL_DISTANCE
            && abs(int(beginPos.y - endPos.y)) < TOLERATE_VERTICAL_DISTANCE)
        {
            return true;
        }
    }
    else
    {
        if ((beginPos.x - endPos.x) > TOLERATE_HORIZONTAL_DISTANCE)
        {
            return true;
        }
    }
    
    return false;
}

bool PageLayer::isSwipeRight(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos)
{
    // if there is physical object, should check horizontal distance,
    // because we should determine if it is dragging physical object or swipping
    if (page->settings.fallingObjectSetting.plistfilename.size() > 0)
    {
        if ((endPos.x - beginPos.x) > TOLERATE_HORIZONTAL_DISTANCE
            && abs(int(beginPos.y - endPos.y)) < TOLERATE_VERTICAL_DISTANCE)
        {
            return true;
        }
    }
    else
    {
        if ((endPos.x - beginPos.x) > TOLERATE_HORIZONTAL_DISTANCE)
        {
            return true;
        }
    }
    
    return false;
}

void PageLayer::createSprites()
{
	// create sprites and add to layer
	vector<SpriteInfo*>::iterator iter;
	for (iter = page->sprites.begin(); iter != page->sprites.end(); ++iter)
	{
		SpriteInfo* spriteInfo = *iter;

		CCSprite *sprite = CCSprite::spriteWithFile(spriteInfo->image.c_str());
		sprite->setTag(spriteInfo->spriteTag);
		sprite->setPosition(spriteInfo->position);

		// runAction
		for (unsigned int i = 0; i < spriteInfo->actions.size(); ++i)
		{
			CCAction *action = page->actions[spriteInfo->actions[i]];
			assert(action != NULL);
			sprite->runAction(action);
            
            // record all actions, then these actions may be skipped when swipping
            addActionsToBeSkipped(action);
		}

		addChild(sprite, spriteInfo->zOrder);
	}
}

void PageLayer::createMainMenuItem()
{
    CCMenuItem *mainMenuItem = CCMenuItemImage::itemFromNormalImage(Configurations::homeButtonNormalStateImage.c_str(), 
                                                                    Configurations::homeButtonTappedStateImage.c_str(),
                                                                    this,
                                                                    menu_selector(PageLayer::mainMenuItemCallback));
    mainMenuItem->setPosition(Configurations::homeButtonPosition);
    mainMenuItem->setScale(Configurations::homeButtonScale);
    
    CCMenu *mainMenu = CCMenu::menuWithItems(mainMenuItem, NULL);
    mainMenu->setPosition(CCPointZero);
    addChild(mainMenu, GlobalData::ZORDER_MAIN_MENU);
}

void PageLayer::createPhysicsLayer()
{
    if (page->settings.fallingObjectSetting.plistfilename.size() > 0)
    {
        ChipmunkLayer *chipmunkLayer = ChipmunkLayer::layerWithPage(&page->settings.fallingObjectSetting,
                                                                    &page->settings.staticObjectSetting);
        addChild(chipmunkLayer);
    }    
}

void PageLayer::mainMenuItemCallback(CCObject *sender)
{
    // popup a dialog to select
    // should release previous dialog if it is exist
    CC_SAFE_RELEASE_NULL(mydialog);
    
    // pop a dialog to select 
    vector<string> items;
    items.push_back("Yes");
    items.push_back("No");
    
    mydialog = new MyDialog();
    mydialog->initWithItems("Go to main menu?", items, this);
    mydialog->popUp();  
}

void PageLayer::buttonClicked(int index)
{
    if (index == 0)
    {
        PageManager::gotoMainMenu();
    }
}

bool PageLayer::addTextSpace(float yFirstLineText)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    if (yFirstLineText > winSize.height/2)
    {
        return false;
    }
    else 
    {
        return true;
    }
}

void PageLayer::createParagraph(int index)
{
    assert(index < page->paragraphs.size());
    
    // add paragraph to a seperate layer 
    // then it is easy to remove a paragraph
    if (page->paragraphs.size() > 0 && index < page->paragraphs.size())
    {
        currentIndexOfParagraph = index;

        // remove previous label layer if it exists
        removeChildByTag(PARAGRAPH_LAYER_TAG, true);
        CC_SAFE_RELEASE_NULL(paragraphLayer);
        
        paragraphLayer = CCLayer::node();
        paragraphLayer->retain();
        paragraphLayer->setTag(PARAGRAPH_LAYER_TAG);
        
        addParagraphText(index);
        addParagraphHotspot(index);
    }
}

void PageLayer::addParagraphText(int index)
{
    // clear paragrsphs
    clearLabelIndex();
    
    // font settings
    const char* fontName = NULL;
    ccColor3B *fontColor = NULL;
    int fontSize = 0;
    
    // add texts
    
    Paragraph *paragraph = page->paragraphs[index];
    
    vector<LineText*> &linesOfText = paragraph->linesOfTest;
    vector<LineText*>::iterator lineTextIter;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    int downIndex = linesOfText.size();
    int upIndex = 0;
    int lineNum = 0;
    for (lineTextIter = linesOfText.begin(); lineTextIter != linesOfText.end(); ++lineTextIter, --downIndex, ++upIndex, ++lineNum)
    {
        LineText* lineText = *lineTextIter;
        xOffset = lineText->xOffset;
        
        fontName = lineText->fontType.c_str();
        fontColor = &lineText->fontColor;
        fontSize = lineText->fontSize;
        
        yOffset = lineText->yOffset;
        
        for (int i = 0; i < lineText->words.size(); ++i)
        {
            string &word = lineText->words[i];
            CCLabelTTF *label = CCLabelTTF::labelWithString(word.c_str(), fontName, fontSize);
            
            label->setColor(*fontColor);
            label->setAnchorPoint(ccp(0.0f, 0.25f));
            label->setPosition(ccp(xOffset, yOffset));
            
            // record label in a vector, don't have to retain it
            // because it is retained by labelLayer
            LabelIndex *labelIndex = new LabelIndex();
            labelIndex->label = label;
            labelIndex->lineIndex = lineNum;
            labelIndex->paragraphIndex = currentIndexOfParagraph;
            wordsOfParagraph.push_back(labelIndex);
            
            paragraphLayer->addChild(label);
            
            //
            // New word spacing/offset codes - Chris Whitman
            //
            
            // If we are on the last word of a line, do not perform increment on xOffset
            if (i < lineText->words.size()-1) {
                // Calculate placement of CCLabelTTF
                xOffset += label->getContentSize().width + WORD_SPACING;
            }
        }
    }
}

void PageLayer::addParagraphHotspot(int index)
{
    removeChildByTag(PARAGRAPH_HOTSPOT_DETECT_TAG, true);
    
    TouchDetection *paragraphTouchDectector = TouchDetection::node();
    paragraphTouchDectector->setIsTouchEnabled(true);
    paragraphTouchDectector->setTag(PARAGRAPH_HOTSPOT_DETECT_TAG);
    addChild(paragraphTouchDectector, GlobalData::ZORDER_HOTSPOT);
    
    // add hotspots
    Paragraph *paragraph = page->paragraphs[index];
    std::vector<HotspotInfo*> &hotspots = paragraph->hotspots;
    std::vector<HotspotInfo*>::iterator hotspotIter;
    for (hotspotIter = hotspots.begin(); hotspotIter != hotspots.end(); ++hotspotIter)
    {
        paragraphTouchDectector->addZoneWithPositionRadiusTargetSel((*hotspotIter)->position,
                                                                    (*hotspotIter)->radius,
                                                                    this,
                                                                    schedule_selector(PageLayer::hotspotCallback),
                                                                    (*hotspotIter)->touchFlag);
        if ((*hotspotIter)->glitterIndicator)
        {
            // add particle system
            CCParticleSystem *particle = CCParticleSystemQuad::particleWithFile("Flower.plist");
            particle->setPosition((*hotspotIter)->position);
            particle->initWithTotalParticles(6);
            
            paragraphLayer->addChild(particle);
        }
    }
}

void PageLayer::hotspotCallback(float flag)
{
    HotspotInfo *hotspot = page->getParagraphHotspotInfo(currentIndexOfParagraph, flag);
    assert(hotspot != NULL);
    
    doHotspotTouched(hotspot, true);
}

void PageLayer::showParagraph(float delay)
{
    CCFadeIn *fadeIn = CCFadeIn::actionWithDuration(Configurations::paragraphTextFadeDuration);
    CCCallFunc* callBack = CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::highlightParagraph));
    
    // delay some time then turn left
    // use "autoplayDuration" instead of highlight time
    if (MainMenuLayer::storyMode == kSotryModeAutoPlay)
    {
        this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(page->paragraphs[currentIndexOfParagraph]->autoplayDuration),
                                            CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::doSwipeLeftAfterDelay)),
                                            NULL));
    }
    
    if (delay != 0)
    {
        paragraphLayer->setIsVisible(false);
        CCAction *seq = CCSequence::actions(CCDelayTime::actionWithDuration(delay),
                                             CCShow::action(),
                                             fadeIn,
                                             callBack,
                                              NULL);
        paragraphLayer->runAction(seq);
        
        // record actions to be skipped
        // when animations are skipped, then should show paragraph at once
        addActionsToBeSkipped(seq);
    }
    else 
    {
        paragraphLayer->runAction(CCSequence::actions(fadeIn,
                                                      callBack,
                                                      NULL));
    }
    
    addChild(paragraphLayer, GlobalData::ZORDER_PARAGRAPH);
}

// stop highlight and should change color back
void PageLayer::stopHighlightParagraph()
{
    isHighLighting = false;
    
    if (MainMenuLayer::storyMode != kStoryModeReadItMyself)
    {
        std::vector<LabelIndex*>::iterator iter;
        for (iter = wordsOfParagraph.begin(); iter != wordsOfParagraph.end(); ++iter)
        {
            (*iter)->label->stopAllActions();
            (*iter)->label->setColor(getLineTextByLabel((*iter)->label)->fontColor);
        }
    }
}

void PageLayer::highlightParagraph()
{
    // don't hilight if the story mode is read to myself
    if (MainMenuLayer::storyMode != kStoryModeReadItMyself)
    {
        // play corresponding effect
        int wordCount = 0;
        vector<float> &audioInterval = page->paragraphs[currentIndexOfParagraph]->highlightingTimes;
        std::vector<LabelIndex*>::iterator iter;
        for (iter = wordsOfParagraph.begin(); iter != wordsOfParagraph.end(); ++iter)
        {
            if (wordCount < audioInterval.size())
            {
                (*iter)->label->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(audioInterval[wordCount]),
                                                       CCCallFuncN::actionWithTarget(this, callfuncN_selector(PageLayer::changeColor)),
                                                       NULL));
            }
            
            if (wordCount < audioInterval.size() - 1)
            {
                (*iter)->label->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(audioInterval[wordCount+1]),
                                                       CCCallFuncN::actionWithTarget(this, callfuncN_selector(PageLayer::changeColorBack)),
                                                       NULL));
            }
            
            if (wordCount == audioInterval.size() - 1)
            {
                (*iter)->label->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(audioInterval[wordCount] + 0.8f),
                                                       CCCallFuncN::actionWithTarget(this, callfuncN_selector(PageLayer::changeColorBack)),
                                                       NULL));
            }
            
            ++wordCount;
        }
        SimpleAudioEngine::sharedEngine()->playBackgroundMusic(page->paragraphs[currentIndexOfParagraph]->voiceAudioFile.c_str(),
                                                               false);
        
        if (wordCount > 0)
        {
            isHighLighting = true;
        }
    }
}

void PageLayer::changeColor(CCObject *sender)
{
    ((CCLabelTTF*)sender)->setColor(getLineTextByLabel((CCLabelTTF*)sender)->fontHighlightColor);
}

void PageLayer::changeColorBack(CCObject *sender)
{
    CCLabelTTF *word = (CCLabelTTF*)sender;
    word->setColor(getLineTextByLabel(word)->fontColor);
    
    if (word == wordsOfParagraph[wordsOfParagraph.size()-1]->label)
    {
        enableDelayForTextTouchNode();
    }
}

void PageLayer::doSwipeLeftAfterDelay(CCObject *sender)
{
    isHighLighting = false;
    
    // this fucntion is invoked to swipe left automatically when selecting "auto play" mode
    // if the video is playing, it should not swipe left
    if (!isVideoPlaying)
    {
        swipeLeft();
    } 
}

void PageLayer::stopHighlighVoiceAndOtherEffect()
{
    const char* voiceAudioOfHighlight = page->paragraphs[currentIndexOfParagraph]->voiceAudioFile.c_str();
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(voiceAudioOfHighlight);
    
    SimpleAudioEngine::sharedEngine()->stopEffect(touchSoundId);
}

void PageLayer::playBackgroundMusic()
{
	// file path
    if (page->settings.audioFilePath.size() > 0)
    {
        const char *musicFilePath = page->settings.audioFilePath.c_str();
        bool loop = page->settings.loop;
        
        if (loop)
        {
            SimpleAudioEngine::sharedEngine()->playBackgroundMusic(musicFilePath, true);
        }
        else
        {
            SimpleAudioEngine::sharedEngine()->playBackgroundMusic(musicFilePath, false);
        }
    }	
}

void PageLayer::clearLabelIndex()
{
    while(! wordsOfParagraph.empty()) {
        delete wordsOfParagraph.back();
        wordsOfParagraph.pop_back();
    }
}

LineText* PageLayer::getLineTextByLabel(cocos2d::CCLabelTTF* label)
{
    vector<LabelIndex*>::iterator iter;
    for (iter = wordsOfParagraph.begin(); iter != wordsOfParagraph.end(); ++iter)
    {
        if (label == (*iter)->label)
        {
            int paragraphIndex = (*iter)->paragraphIndex;
            int textIndex = (*iter)->lineIndex;
            return page->paragraphs[paragraphIndex]->linesOfTest[textIndex];
        }
    }
    
    return NULL;
}

void PageLayer::addActionsToBeSkipped(cocos2d::CCAction *action)
{
    actionsToBeSkipped.push_back(action);
    action->retain();
}

void PageLayer::skipAnimation()
{
    // skip animations and show paragraph at once
    vector<cocos2d::CCAction*>::iterator iter;
    for (iter = actionsToBeSkipped.begin(); iter != actionsToBeSkipped.end(); ++iter)
    {
        (*iter)->finish();
        (*iter)->release();
    }
    actionsToBeSkipped.clear();
}

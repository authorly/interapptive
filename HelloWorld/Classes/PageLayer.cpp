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
#define PAGELAYER_HANDLER_PRIORITY 10

#define TOUCH_DETECT_TAG    11

#define XSCALE          (GlobalData::sharedGlobalData()->xScale)
#define YSCALE          (GlobalData::sharedGlobalData()->yScale)
#define WORD_SPACING    (3 * XSCALE)

PageLayer::PageLayer()
: currentIndexOfParagraph(0)
, beginPoint(0, 0)
, page(NULL)
, paragraphLayer(NULL)
, mydialog(NULL)
, isSwiping(false)
, delayOfAnimation(0)
, touchDetector(NULL)
, touchSoundId(0)
, isVideoPlaying(false)
, isHighLighting(false)
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
    CC_SAFE_RELEASE_NULL(touchDetector);
}

void PageLayer::init(Page *page)
{
    this->page = page;
    
    // preload backgound music
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(page->settings.audioFilePath.c_str());
    // preload effect used to speak paragraph
    for (int i = 0; i < page->paragraphs.size(); ++i)
    {
        SimpleAudioEngine::sharedEngine()->preloadEffect(page->paragraphs[i]->voiceAudioFile.c_str());
    }
    
    this->delayOfAnimation = this->calculateDelayTimeOnEnter();
}

void PageLayer::onEnter()
{
    createSprites();
    createPhysicsLayer();
	createParagraph(0);
    
    // add menu item to go to main menu
    createMainMenuItem();
    
    addTouchNode();
    
    setIsTouchEnabled(true);
    
    CCLayer::onEnter();
}

void PageLayer::addTouchNode()
{
    // add touchable nodes
	touchDetector = TouchDetection::node();
    touchDetector->retain();
    
	vector<StoryTouchableNode*>::iterator iter;
	for (iter = page->storyTouchableNodes.begin(); iter != page->storyTouchableNodes.end(); ++iter)
	{
		StoryTouchableNode* touchNode = *iter;
		touchDetector->addZoneWithPositionRadiusTargetSel(touchNode->position, touchNode->radius, this, schedule_selector(PageLayer::touchCallback), touchNode->touchFlag);
        
        // add particle system
        CCParticleSystem *particle = CCParticleSystemQuad::particleWithFile("Flower.plist");
        particle->setPosition(touchNode->position);
        particle->initWithTotalParticles(6);
        
        // preload effect if exists
        SimpleAudioEngine::sharedEngine()->preloadEffect(touchNode->soundToPlay.c_str());
        
        // delay for animation
        if (touchNode->delayForAnimation)
        {
            touchableNodeDelayForAnimationArray.push_back(touchNode);
            touchDetector->enableTouchByFlag(touchNode->touchFlag, false);
            particle->setIsVisible(false);
            particle->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(delayOfAnimation),
                                                    CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::enableDelayForAnimationTouchNode)),
                                                    CCShow::action(),
                                                    NULL));
        }
        
        // delay for text
        if (touchNode->delayForText
            && (MainMenuLayer::storyMode == kStoryModeReadToMe
                || (MainMenuLayer::storyMode == kSotryModeAutoPlay)))
        {
            DelayForTextTouchNodeInfo info;
            info.touchNode = touchNode;
            info.partileSystem = particle;
            touchableNodeDelayForTextArray.push_back(info);
            
            touchDetector->enableTouchByFlag(touchNode->touchFlag, false);
            particle->setIsVisible(false);
        }
        
        addChild(particle);
	}
    
    touchDetector->setTag(TOUCH_DETECT_TAG);
    addChild(touchDetector);
    touchDetector->setIsTouchEnabled(true);
}

void PageLayer::enableDelayForAnimationTouchNode(CCObject *sender)
{
    vector<StoryTouchableNode*>::iterator iter;
    for (iter = touchableNodeDelayForAnimationArray.begin(); iter != touchableNodeDelayForAnimationArray.end(); ++iter)
    {
        touchDetector->enableTouchByFlag((*iter)->touchFlag, true);
    }
}

void PageLayer::enableDelayForTextTouchNode()
{
    vector<DelayForTextTouchNodeInfo>::iterator iter;
    for (iter = touchableNodeDelayForTextArray.begin(); iter != touchableNodeDelayForTextArray.end(); ++iter)
    {
        touchDetector->enableTouchByFlag((*iter).touchNode->touchFlag, true);
        (*iter).partileSystem->setIsVisible(true);
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

void PageLayer::touchCallback(float flag)
{
    StoryTouchableNode *storyTouchableNode = page->getSotryTouchableNodeByFlag(flag);
    
    // stop all sounds and stop highlight
    if (storyTouchableNode->stopEffectIndicator)
    {
        stopHighlightEffect();
        stopHighlightParagraph();
    }

    // play video
    bool showControl = true;
    string &videoName = storyTouchableNode->videoToPlay;
    if (videoName.size() != 0)
    {
        if (MainMenuLayer::storyMode == kSotryModeAutoPlay)
        {
            VideoPlayer::sharedVideoPlayer()->playVideoByFilename(videoName.c_str(), showControl, this);
        }
        else
        {
            VideoPlayer::sharedVideoPlayer()->playVideoByFilename(videoName.c_str(), showControl);
        }
        
        isVideoPlaying = true;
    }
    
    // play audeo
    string &audeoName = storyTouchableNode->soundToPlay;
    if (audeoName.size() != 0)
    {
        // stop first
        SimpleAudioEngine::sharedEngine()->stopEffect(touchSoundId);
        // play
        touchSoundId = SimpleAudioEngine::sharedEngine()->playEffect(audeoName.c_str());
    }
}

void PageLayer::moviePlayBackDidFinish()
{
    isVideoPlaying = false;
    
    if (isHighLighting)
    {
        return;
    }
    else
    {
        swipeLeft();
    }
}

/// touch event
void PageLayer::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, PAGELAYER_HANDLER_PRIORITY, true);
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
    
    CCPoint endPoint = pTouch->locationInView(pTouch->view());
    endPoint = CCDirector::sharedDirector()->convertToGL(endPoint);
    
    if (isSwiping)
    {
        return;
    }
    
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
                    // the action should be CCScaleBy or CCMoveBy
                    CCAction *element = page->getActionByTag(actionToRun->actionTags[j]);
                    assert(dynamic_cast<CCScaleBy*>(element) != NULL || dynamic_cast<CCMoveBy*>(element) != NULL);
                    
                    array->addObject(element);
                }
                action = CCSpawn::actionsWithArray(array);                
            }
            else 
            {
                action = (CCFiniteTimeAction*)page->getActionByTag(actionToRun->actionTags[0]);
                assert(dynamic_cast<CCScaleBy*>(action) != NULL || dynamic_cast<CCMoveBy*>(action) != NULL);
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
            }
            else 
            {
                // swipe right
                // run reverse action
                CCFiniteTimeAction* seq = CCSequence::actions(action->reverse(), 
                                                              CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::swipEndCallBack)),
                                                              NULL);
                sprite->runAction(seq); 
            }  
            
            // start swipping, then don't do swipe operation until done
            isSwiping = true;
        }
        
        delete swipeEndedActionsToRun;
    }
    
    return delay;
}

void PageLayer::swipEndCallBack()
{
    // swipping ended
    isSwiping = false;
}

void PageLayer::swipeLeft()
{
    stopHighlightEffect();
    
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
    if (page->settings.number == 1)
    {
        return;
    }
    
    stopHighlightEffect();
    
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
        
        // add child
        // remove child
        
        // run actions
        
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
		}

		addChild(sprite);
	}
}

void PageLayer::createMainMenuItem()
{
    CCMenuItem *mainMenuItem = CCMenuItemImage::itemFromNormalImage(Configurations::homeButtonNormalStateImage.c_str(), 
                                                                    Configurations::homeButtonTappedStateImage.c_str(),
                                                                    this,
                                                                    menu_selector(PageLayer::mainMenuItemCallback));
    mainMenuItem->setPosition(Configurations::homeButtonPosition);
    
    CCMenu *mainMenu = CCMenu::menuWithItems(mainMenuItem, NULL);
    mainMenu->setPosition(CCPointZero);
    addChild(mainMenu);
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
        // remove previous label layer if it exists
        removeChildByTag(PARAGRAPH_LAYER_TAG, true);
        CC_SAFE_RELEASE_NULL(paragraphLayer);
        
        // clear paragrsphs
        wordsOfParagraph.clear();
        
        currentIndexOfParagraph = index;
        
        // font settings
        const char* fontName = page->settings.fontType.c_str();
        ccColor3B &fontColor = page->settings.fontColor;
        int fontSize = page->settings.fontSize;

        paragraphLayer = CCLayer::node();
        paragraphLayer->retain();
        paragraphLayer->setTag(PARAGRAPH_LAYER_TAG);
        
        Paragraph *paragraph = page->paragraphs[index];
        vector<LineText*> &linesOfText = paragraph->linesOfTest;
        
		vector<LineText*>::iterator lineTextIter;
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        // should add space when it is zoomed in, because we add text size
        float ySpaceAdded = XSCALE < 1.0f ? 4 : 0;
        int downIndex = linesOfText.size();
        int upIndex = 0;
        bool addSpace = addTextSpace(downIndex > 0 ? linesOfText[0]->yOffset : 0);
		for (lineTextIter = linesOfText.begin(); lineTextIter != linesOfText.end(); ++lineTextIter, --downIndex, ++ upIndex)
		{
			LineText* lineText = *lineTextIter;
            xOffset = lineText->xOffset;
            
            if (addSpace)
            {
                yOffset = (lineText->yOffset + downIndex*ySpaceAdded) - 30;
            }
            else 
            {
                yOffset = lineText->yOffset - upIndex*ySpaceAdded;
            }
            
            for (int i = 0; i < lineText->words.size(); ++i)
            {
                string &word = lineText->words[i];
                CCLabelTTF *label = CCLabelTTF::labelWithString(word.c_str(), fontName, fontSize);                
                
                label->setColor(fontColor);                           
                label->setPosition(ccp(xOffset, yOffset));
                        
                // record label in a vector, don't have to retain it
                // because it is retained by labelLayer
                wordsOfParagraph.push_back(label);
                
                paragraphLayer->addChild(label);

                //                              
                // New word spacing/offset codes - Chris Whitman
                //
                
                // If we are on the last word of a line, do not perform increment on xOffset 
                if (i < lineText->words.size()-1) {
                    
                    // Local variable for the next word (string) in line (lineText array)
                    string &word = lineText->words[i+1];
                    
                    // Create a CCLabelTTF for the next word so we may measure it
                    CCLabelTTF *nextLabel = CCLabelTTF::labelWithString(word.c_str(), fontName, fontSize); 
                   
                    // Calculate placement of CCLabelTTF
                    xOffset += label->getContentSize().width/2 + nextLabel->getContentSize().width/2 + WORD_SPACING;
                }
            }
		}
    }
}

void PageLayer::showParagraph(float delay)
{
    CCFadeIn *fadeIn = CCFadeIn::actionWithDuration(Configurations::paragraphTextFadeDuration);
    CCCallFunc* callBack = CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::highlightParagraph));
    
    if (delay != 0)
    {
        paragraphLayer->setIsVisible(false);
        paragraphLayer->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(delay),
                                                      CCShow::action(),
                                                      fadeIn,
                                                      callBack,
                                                      NULL));
    }
    else 
    {
        paragraphLayer->runAction(CCSequence::actions(fadeIn,
                                                      callBack,
                                                      NULL));
    }
    
    addChild(paragraphLayer);
}

// stop highlight and should change color back
void PageLayer::stopHighlightParagraph()
{
    if (MainMenuLayer::storyMode != kStoryModeReadItMyself)
    {
        vector<CCLabelTTF*>::iterator iter;
        for (iter = wordsOfParagraph.begin(); iter != wordsOfParagraph.end(); ++iter)
        {
            (*iter)->stopAllActions();
            (*iter)->setColor(page->settings.fontColor);
        }
    }
}

void PageLayer::highlightParagraph()
{
    // don't hilight if the story mode is read to myself
    if (MainMenuLayer::storyMode != kStoryModeReadItMyself)
    {
        isHighLighting = true;
        
        // play corresponding effect
        int wordCount = 0;
        vector<float> &audioInterval = page->paragraphs[currentIndexOfParagraph]->highlightingTimes;
        vector<CCLabelTTF*>::iterator iter;
        for (iter = wordsOfParagraph.begin(); iter != wordsOfParagraph.end(); ++iter)
        {
            if (wordCount < audioInterval.size())
            {
                (*iter)->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(audioInterval[wordCount]),
                                                       CCCallFuncN::actionWithTarget(this, callfuncN_selector(PageLayer::changeColor)),
                                                       NULL));
            }
            
            if (wordCount < audioInterval.size() - 1)
            {
                (*iter)->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(audioInterval[wordCount+1]),
                                                       CCCallFuncN::actionWithTarget(this, callfuncN_selector(PageLayer::changeColorBack)),
                                                       NULL));
            }
            
            if (wordCount == audioInterval.size() - 1)
            {
                (*iter)->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(audioInterval[wordCount] + 0.8f),
                                                       CCCallFuncN::actionWithTarget(this, callfuncN_selector(PageLayer::changeColorBack)),
                                                       NULL));
            }
            
            ++wordCount;
        }
        SimpleAudioEngine::sharedEngine()->playEffect(page->paragraphs[currentIndexOfParagraph]->voiceAudioFile.c_str());
    }
}

void PageLayer::changeColor(CCObject *sender)
{
    ((CCLabelTTF*)sender)->setColor(page->settings.fontHighlightColor);
}

void PageLayer::changeColorBack(CCObject *sender)
{
    CCLabelTTF *word = (CCLabelTTF*)sender;
    word->setColor(page->settings.fontColor);
    
    if (MainMenuLayer::storyMode == kSotryModeAutoPlay
        && word == wordsOfParagraph[wordsOfParagraph.size()-1])
    {
        this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(page->settings.autoplayDelayBeforePageTurn),
                                            CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::doSwipeLeftAfterDelay)),
                                            NULL));
    }
    
    if (word == wordsOfParagraph[wordsOfParagraph.size()-1])
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

void PageLayer::stopHighlightEffect()
{
    // stop all effect, I think now it will only have one effect that speaking word.
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
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
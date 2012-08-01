#include "PageLayer.h"
#include "Page.h"
#include "TouchDetection.h"
#include "SimpleAudioEngine.h"
#include "PageManager.h"
#include "Configurations.h"
#include "MainMenuLayer.h"
#include "SharedGlobalData.h"
#include "ChipmunkLayer.h"

#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

// the distance(in points) to determine swipe left or right
#define TOLERATE_DISTANCE   50
// page layer tag
#define PARAGRAPH_LAYER_TAG       10
#define PAGELAYER_HANDLER_PRIORITY 10

#define TOUCH_DETECT_TAG    11

#define XSCALE          (GlobalData::sharedGlobalData()->xScale)
#define YSCALE          (GlobalData::sharedGlobalData()->yScale)
#define WORD_SPACING    (6 * XSCALE)

PageLayer::PageLayer()
: currentIndexOfParagraph(0)
, beginPoint(0, 0)
, page(NULL)
, paragraphLayer(NULL)
, mydialog(NULL)
, isVideoPlaying(false)
, isSwiping(false)
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
    
	createSprites();
    createPhysicsLayer();
	createParagraph(0);
    
    // add menu item to go to main menu
    createMainMenuItem();
}

void PageLayer::onEnter()
{
    // add touchable nodes
	TouchDetection *touchDetector = TouchDetection::node();
	addChild(touchDetector);
    touchDetector->setTag(TOUCH_DETECT_TAG);
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
        
        addChild(particle);
	}
	touchDetector->setIsTouchEnabled(true);
    
    setIsTouchEnabled(true);
    
    CCLayer::onEnter();
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
    
    // stop all sounds
    if (storyTouchableNode->stopEffectIndicator)
    {
        stopHighlightEffect();
    }

    // play video
    bool showControl = false;
    string &videoName = storyTouchableNode->videoToPlay;
    if (videoName.size() != 0)
    {
       VideoPlayer::sharedVideoPlayer()->playVideoByFilename(this, videoName.c_str(), showControl); 
    }
    
    // play audeo
    string &audeoName = storyTouchableNode->soundToPlay;
    if (audeoName.size() != 0)
    {
        SimpleAudioEngine::sharedEngine()->playEffect(audeoName.c_str());
    }
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (showControl)
    {
        // disable touch event
        setIsTouchEnabled(false);
        ((TouchDetection*)getChildByTag(TOUCH_DETECT_TAG))->setIsTouchEnabled(false);
    }
    isVideoPlaying = true;
#endif
}

void PageLayer::moviePlayBackDidFinish()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (getIsTouchEnabled() == false)
    {
        setIsTouchEnabled(true);
        ((TouchDetection*)getChildByTag(TOUCH_DETECT_TAG))->setIsTouchEnabled(true);
    }
    
    isVideoPlaying = false;
#endif
}

/// touch event
void PageLayer::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, PAGELAYER_HANDLER_PRIORITY, true);
}

void PageLayer::onEnterTransitionDidFinish()
{   
    playBackgroundMusic();  
    float delay = calculateDelayTimeOnEnter();
    showParagraph(delay);
    
    CCLayer::onEnterTransitionDidFinish();
}

// some sprites may run action when onEnter
// so should calcuate delay time for showing paragraph
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
            if (dynamic_cast<CCMoveTo*>(action) != NULL || dynamic_cast<CCScaleTo*>(action) != NULL)
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
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (isVideoPlaying)
    {
        VideoPlayer::sharedVideoPlayer()->stopPlay();
        isVideoPlaying = false;
        
        return;
    }
#endif
    
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

float PageLayer::swipeEndedOperationAndCalculateTotalDelay(bool swipeLeft)
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
    
    /*
     typedef struct storySwipeEndedActionsToRun
     {
     int runAfterSwipeNumber;
     int spriteTag;
     int actionTag;
     } StorySwipeEndedActionsToRun;
     */
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
            
            
            
            if (swipeLeft)
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
    float beginX = beginPos.x;
    float endX = endPos.x;
    
    if ((beginX - endX) > TOLERATE_DISTANCE)
    {
        return true;
    }
    
    return false;
}

bool PageLayer::isSwipeRight(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos)
{
    float beginX = beginPos.x;
    float endX = endPos.x;
    
    if ((endX - beginX) > TOLERATE_DISTANCE)
    {
        return true;
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

		//std::string               image;
	    //int                       spriteTag;
	    //cocos2d::CCPoint          position;
	    //std::vector<int>          actions; // actionTags

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
        ChipmunkLayer *chipmunkLayer = ChipmunkLayer::layerWithPage(page);
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
		for (lineTextIter = linesOfText.begin(); lineTextIter != linesOfText.end(); ++lineTextIter)
		{
			LineText* lineText = *lineTextIter;
            xOffset = lineText->xOffset;
            yOffset = lineText->yOffset;
            
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
    CCCallFunc* showParagraphAction = CCCallFunc::actionWithTarget(this, callfunc_selector(PageLayer::setParagraphVisible));
    
    if (delay != 0)
    {
        paragraphLayer->setIsVisible(false);
        paragraphLayer->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(delay),
                                                      showParagraphAction,
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

void PageLayer::setParagraphVisible()
{
    paragraphLayer->setIsVisible(true);
}

void PageLayer::highlightParagraph()
{
    // don't hilight if the story mode is read to myself
    if (MainMenuLayer::storyMode != kStoryModeReadItMyself)
    {
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
    
    if (MainMenuLayer::storyMode == kSotryModeAutoPlay && word == wordsOfParagraph[wordsOfParagraph.size()-1])
    {
        swipeLeft();
    }
}

void PageLayer::stopHighlightEffect()
{
    // stop all effect, I think now it will only have one effect that speaking word.
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
    // stop background music
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
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
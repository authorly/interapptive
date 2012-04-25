#include "PageLayer.h"
#include "Page.h"
#include "TouchDetection.h"
#include "SimpleAudioEngine.h"
#include "PageManager.h"
#include "Configurations.h"

#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

// the distance(in points) to determine swipe left or right
#define TOLERATE_DISTANCE   50
// page layer tag
#define PARAGRAPH_LAYER_TAG       10
#define PAGELAYER_HANDLER_PRIORITY 10
// space between words of text
#define WORD_SPACING 8

PageLayer::PageLayer()
: currentIndexOfParagraph(0)
, beginPoint(0, 0)
, page(NULL)
, paragraphLayer(NULL)
, highlightCallbackTimes(0)
{}

PageLayer* PageLayer::pageLayerWithPage(Page* page)
{
	PageLayer *layer = new PageLayer();
	layer->autorelease();
    
    layer->init(page);
	
	return layer;
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
	createParagraph(0);
}

void PageLayer::onEnter()
{
    // add touchable nodes
	TouchDetection *touchDetector = TouchDetection::node();
	addChild(touchDetector);
	vector<StoryTouchableNode*>::iterator iter;
	for (iter = page->storyTouchableNodes.begin(); iter != page->storyTouchableNodes.end(); ++iter)
	{
		StoryTouchableNode* touchNode = *iter;
        
		touchDetector->addZoneWithPositionRadiusTargetSel(touchNode->position, touchNode->radius, this, schedule_selector(PageLayer::touchCallBack), touchNode->touchFlag);
	}
	touchDetector->isDebugDrawing = true;
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
}

void PageLayer::touchCallBack(float flag)
{
	CCLog("touch zone call back %f", flag);
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
    CCPoint endPoint = pTouch->locationInView(pTouch->view());
    endPoint = CCDirector::sharedDirector()->convertToGL(endPoint);
    
    if (isSwipeLeft(beginPoint, endPoint)) 
    {
        stopHightAndPlayEffect();
        
        if (currentIndexOfParagraph == (page->paragraphs.size() - 1))
        {
            PageManager::turnToPage(page->settings.number + 1);
        }
        else 
        {
            swipeLeft();
        }
    }
    
    if (isSwipeRight(beginPoint, endPoint)) 
    {
        stopHightAndPlayEffect();
        
        if (currentIndexOfParagraph == 0)
        {
            PageManager::turnToPage(page->settings.number - 1);
        }
        else 
        {
            swipeRight();
        }
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
    StorySwipeEndedActionsToRun *swipeEndedActionsToRun = page->getStorySwipeEndedActionToRun(currentIndexOfParagraph);
    
    if (swipeEndedActionsToRun)
    {
        // get sprite
        CCSprite *sprite = (CCSprite*)getChildByTag(swipeEndedActionsToRun->spriteTag);
        assert(sprite != NULL);
        
        // get action
        CCAction *action = page->getActionByTag(swipeEndedActionsToRun->actionTag);
        assert(action != NULL);
        // the action should be CCScaleTo or CCMoveTo
        assert(dynamic_cast<CCScaleBy*>(action) != NULL || dynamic_cast<CCMoveBy*>(action) != NULL);
        
        // caculate delay time
        delay = ((CCFiniteTimeAction*)action)->getDuration();
        
        if (swipeLeft)
        {
            // swipe left
            sprite->runAction(action);
            if (dynamic_cast<CCScaleBy*>(action) != NULL)
            {
                CCLog("CCScaleBy");
            }
        }
        else 
        {
            // swipe right
            // run reverse action
            sprite->runAction(((CCActionInterval*)action)->reverse()); 
        }        
    }
    
    return delay;
}

void PageLayer::swipeLeft()
{
    // increase current index of paragraph
    ++currentIndexOfParagraph;
    
    
    float delay = swipeEndedOperationAndCalculateTotalDelay(true);
 
    
    createParagraph(currentIndexOfParagraph);
    
    showParagraph(delay);
}

void PageLayer::swipeRight()
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
        
        // don't use a 
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
                    
                   
                    // START: xOffset                                                                    
                    //       
                    //    Result:
                    //
                    //        HOORAY
                    //        HOORAY
                    //           
                    // 
                    // 1: xOffset += label->getContentSize().width/2                                     
                    //                                                                                   
                    // Increment x coordinates position for next word.                                   
                    // Since we place text by its center, we divide current label's width by 2. 
                    //     - This represents the current labels width, which must be added to the xOffset
                    //     - After adding this width, (label->getContentSize().width/2),
                    //       the center of next label would be placed at the end of the current label
                    //
                    //    Result:
                    // 
                    //     HOORAY
                    //        HOORAY
                    //
                    //
                    // 2: xOffset += label->getContentSize().width/2 + nextLabel->getContentSize().width/2
                    //
                    // Then, we add the width of the next label, divided by 2 (nextLabel->getContentSize().width/2).
                    //     - This will give enough spacing for beginning of the next label to be placed
                    //       at the end of the current label
                    //
                    //   Result:
                    //
                    //     HOORAY
                    //           HOORAY
                    //
                    //
                    // 3. xOffset += label->getContentSize().width/2 + nextLabel->getContentSize().width/2 + WORD_SPACING
                    //
                    // Then, we increment by the WORD_SPACING constant, this is the spacing between the words.
                    //     - This may need to be scaled according to font size?
                    //       Because a large font size must require more spacing then a small font size. 
                    //       So, we must take the different font sizes into account when considering WORD_SPACING
                    //       We may need a minimum font size and maximum font size for this. 
                    //
                    //    Result:
                    //
                    //     HOORAY
                    //             HOORAY
                    //
                    //
                    // END: xOffset += label->getContentSize().width/2 + nextLabel->getContentSize().width/2 + WORD_SPACING
                    
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
    CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(PageLayer::highlightSchedule), 
                                                     this, 
                                                     page->paragraphs[currentIndexOfParagraph]->highlightingTimes[0], 
                                                     false);
    
    // change first word's color
    wordsOfParagraph[0]->setColor(page->settings.fontHighlightColor);
    
    // play corresponding effect
    SimpleAudioEngine::sharedEngine()->playEffect(page->paragraphs[currentIndexOfParagraph]->voiceAudioFile.c_str());
}

void PageLayer::highlightSchedule(ccTime dt)
{
    ccColor3B fontColor = page->settings.fontColor;
    ccColor3B highlightColor = page->settings.fontHighlightColor;
    
    ++highlightCallbackTimes;
    
    if (highlightCallbackTimes == wordsOfParagraph.size())
    {
        // don't highlight last word
        highlightCallbackTimes = 0;
        wordsOfParagraph[wordsOfParagraph.size()-1]->setColor(fontColor);
        CCScheduler::sharedScheduler()->unscheduleSelector(schedule_selector(PageLayer::highlightSchedule), this);
    }
    else {
        // unhighlight previous word
        wordsOfParagraph[highlightCallbackTimes-1]->setColor(fontColor);
        // highlight current
        wordsOfParagraph[highlightCallbackTimes]->setColor(highlightColor);
        
        // set next schedule time
        float nextScheduleTime = page->paragraphs[currentIndexOfParagraph]->highlightingTimes[highlightCallbackTimes] -
        page->paragraphs[currentIndexOfParagraph]->highlightingTimes[highlightCallbackTimes-1];
        
        CCScheduler::sharedScheduler()->unscheduleSelector(schedule_selector(PageLayer::highlightSchedule), this);
        CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(PageLayer::highlightSchedule), 
                                                         this, 
                                                         nextScheduleTime, 
                                                         false);
    }
}

void PageLayer::stopHightAndPlayEffect()
{
    // unschedule
    CCScheduler::sharedScheduler()->unscheduleSelector(schedule_selector(PageLayer::highlightSchedule), this);
    // stop all effect, I think now it will only have one effect that speaking word.
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
    
    // reset helper data
    highlightCallbackTimes = 0;
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
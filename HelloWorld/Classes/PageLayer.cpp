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

PageLayer::PageLayer()
: currentIndexOfParagraph(0)
, beginPoint(0, 0)
, page(NULL)
, paragraphLayer(NULL)
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
        // the action should CCScaleTo or CCMoveTo
        assert(dynamic_cast<CCScaleTo*>(action) != NULL || dynamic_cast<CCMoveTo*>(action) != NULL);
        
        // caculate delay time
        delay = ((CCFiniteTimeAction*)action)->getDuration();
        
        if (swipeLeft)
        {
            // swipe left
            sprite->runAction(action);
        }
        else 
        {
            // swipe right
            // run reverse action
            CCFiniteTimeAction* action = ((CCFiniteTimeAction*)action)->reverse();
            sprite->runAction(action);
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
        paragraphs.clear();
        
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
                
                // change xoffet
                xOffset += label->getContentSize().width;
                
                // record label in a vector, don't have to retain it
                // because it is retained by labelLayer
                paragraphs.push_back(label);
                
                paragraphLayer->addChild(label);
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
        //paragraphLayer->setIsVisible(false);
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
    ccColor3B fontColor = page->settings.fontColor;
    ccColor3B highlightColor = page->settings.fontHighlightColor;
    
    
    
    // play corresponding effect
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
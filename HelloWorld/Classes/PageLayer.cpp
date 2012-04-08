#include "PageLayer.h"
#include "Page.h"
#include "TouchDetection.h"
#include "SimpleAudioEngine.h"
#include "PageManager.h"

#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

// the distance(in points) to determine swipe left or right
#define TOLERATE_DISTANCE   50
// z
#define PARAGRAPH_LAYER_TAG       10

PageLayer::PageLayer()
: currentIndexOfParagraph(0)
, beginPoint(0, 0)
, page(NULL)
{}

PageLayer* PageLayer::pageLayerWithPage(Page* page)
{
	PageLayer *layer = new PageLayer();
	layer->autorelease();
    
    layer->page = page;

	layer->createSprites();
	layer->createParagraph(0);
	layer->playBackgroundMusic();

	// add touchable nodes
	TouchDetection *touchDetector = TouchDetection::node();
	layer->addChild(touchDetector);
	vector<StoryTouchableNode*>::iterator iter;
	for (iter = page->storyTouchableNodes.begin(); iter != page->storyTouchableNodes.end(); ++iter)
	{
		StoryTouchableNode* touchNode = *iter;

		touchDetector->addZoneWithPositionRadiusTargetSel(touchNode->position, touchNode->radius, layer, schedule_selector(PageLayer::touchCallBack), touchNode->touchFlag);
	}
	touchDetector->isDebugDrawing = true;
	touchDetector->setIsTouchEnabled(true);
    
    layer->setIsTouchEnabled(true);
	
	return layer;
}

void PageLayer::touchCallBack(float flag)
{
	CCLog("touch zone call back %f", flag);
}

/// touch event
void PageLayer::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 0, true);
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
            CCLOG("turn next page");
            PageManager::turnToPage(page->settings.number + 1);
        }
        else 
        {
            createParagraph(++currentIndexOfParagraph);
        }
    }
    
    if (isSwipeRight(beginPoint, endPoint)) 
    {
        if (currentIndexOfParagraph == 0)
        {
            CCLOG("turn previois page");
            PageManager::turnToPage(page->settings.number - 1);
        }
        else 
        {
            createParagraph(--currentIndexOfParagraph);
        }
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

// only show first paragraph
void PageLayer::createParagraph(int index)
{
    assert(index < page->paragraphs.size());
    
    // add paragraph to a seperate layer 
    // then it is easy to hide it
    if (page->paragraphs.size() > 0 && index < page->paragraphs.size())
    {
        // remove previous label layer if it exists
        removeChildByTag(PARAGRAPH_LAYER_TAG, true);
        
        currentIndexOfParagraph = index;
        
        // font settings
        const char* fontName = page->settings.fontType.c_str();
        ccColor3B &fontColor = page->settings.fontColor;
        int fontSize = page->settings.fontSize;
        
        CCLayer *labelLayer = CCLayer::node();
        labelLayer->setTag(PARAGRAPH_LAYER_TAG);
        addChild(labelLayer);
        
        Paragraph *paragraph = page->paragraphs[index];
        vector<LineText*> &linesOfText = paragraph->linesOfTest;
        
		vector<LineText*>::iterator lineTextIter;
		for (lineTextIter = linesOfText.begin(); lineTextIter != linesOfText.end(); ++lineTextIter)
		{
			LineText* lineText = *lineTextIter;
			CCLabelTTF *label = CCLabelTTF::labelWithString(lineText->text.c_str(), fontName, fontSize);
			label->setColor(fontColor);
			label->setPosition(ccp(lineText->xOffset, lineText->yOffset));
            
			labelLayer->addChild(label);
		}
    }	
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
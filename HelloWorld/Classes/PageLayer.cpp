#include "PageLayer.h"
#include "Page.h"
#include "TouchDetection.h"
#include "SimpleAudioEngine.h"

#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

PageLayer* PageLayer::pageLayerWithPage(Page* page)
{
	PageLayer *layer = new PageLayer();
	layer->autorelease();

	layer->createSprites(page);
	layer->createParagraphs(page);
	layer->playBackgroundMusic(page);

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
	
	return layer;
}

void PageLayer::touchCallBack(float flag)
{
	CCLog("touch zone call back %f", flag);
}

void PageLayer::createSprites(Page *page)
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

void PageLayer::createParagraphs(Page *page)
{
	// font settings
	const char* fontName = page->settings.fontType.c_str();
	ccColor3B &fontColor = page->settings.fontColor;
	int fontSize = page->settings.fontSize;

	// add paragraphs
	vector<Paragraph*>::iterator iter;
	for (iter = page->paragraphs.begin(); iter != page->paragraphs.end(); ++iter)
	{
		vector<LineText*> &linesOfText = (*iter)->linesOfTest;

		vector<LineText*>::iterator lineTextIter;
		for (lineTextIter = linesOfText.begin(); lineTextIter != linesOfText.end(); ++lineTextIter)
		{
			LineText* lineText = *lineTextIter;
			CCLabelTTF *label = CCLabelTTF::labelWithString(lineText->text.c_str(), fontName, fontSize);
			label->setColor(fontColor);
			label->setPosition(ccp(lineText->xOffset, lineText->yOffset));

			addChild(label);
		}
	}
}

void PageLayer::playBackgroundMusic(Page *page)
{
	// file path
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
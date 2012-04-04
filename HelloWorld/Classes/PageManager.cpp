#include "PageManager.h"
#include "JsonParser.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

map<int, Page*> PageManager::pages = map<int, Page*>();

void PageManager::parseJsonAndRun()
{
	if (! JsonParser::parseJson())
	{
		return;
	}

	// create scene for page number 1 and run
	CCDirector::sharedDirector()->runWithScene(createSceneByPageNumber(1));
}


void PageManager::insertPageWithPageNumber(int pageNumber, Page *page)
{
}

Page* PageManager::getPageByPageNumber(int pageNumber)
{
	std::map<int, Page*>::iterator iter = pages.find(pageNumber);
	if (iter != pages.end())
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}

CCScene* PageManager::createSceneByPageNumber(int pageNumber)
{
	CCScene *scene = NULL;
	Page *page = getPageByPageNumber(pageNumber);

	if (page)
	{
		// create a scene
		scene = CCScene::node();

		// create a layer because we want to receive touch event
		CCLayer *layer = CCLayer::node();
		scene->addChild(layer);

		createSprites(page, layer);
        createParagraphs(page, layer);
		playBackgroundMusic(page);
	}

	return scene;
}

void PageManager::createSprites(Page *page, cocos2d::CCLayer *layer)
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
		for (int i = 0; i < spriteInfo->actions.size(); ++i)
		{
			CCAction *action = page->actions[spriteInfo->actions[i]];
			assert(action != NULL);
			sprite->runAction(action);
		}

		layer->addChild(sprite);
	}
}

void PageManager::createParagraphs(Page *page, cocos2d::CCLayer *layer)
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

			layer->addChild(label);
		}
	}
}

void PageManager::playBackgroundMusic(Page *page)
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


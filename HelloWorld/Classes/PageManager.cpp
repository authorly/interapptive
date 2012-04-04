#include "PageManager.h"
#include "JsonParser.h"
#include "cocos2d.h"
#include "PageLayer.h"

using namespace cocos2d;

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
		CCLayer *layer = PageLayer::pageLayerWithPage(page);;
		scene->addChild(layer);
	}

	return scene;
}


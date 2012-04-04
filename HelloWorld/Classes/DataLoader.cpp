#include "DataLoader.h"

using namespace std;
using namespace cocos2d;

DataLoader* DataLoader::instance()
{
	static DataLoader* instance = NULL;
	if (! instance)
	{
		instance = new DataLoader();
	}

	return instance;
}

void DataLoader::loadAssetsAsync(std::vector<string> *filePathArray, CCObject *target, SEL_CallFunc callBack)
{
	loadPointer = 0;
	totalToLoad = filePathArray->size();
	this->target = target;
	this->callBack = callBack;
	this->filePathArray = filePathArray;

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	CCDirector::sharedDirector()->purgeCachedData();
	CCLog("start async download");

	CCTextureCache::sharedTextureCache()->addImageAsync((*filePathArray)[loadPointer++].c_str(), this, callfuncO_selector(DataLoader::didLoadImage));
}

void DataLoader::didLoadImage(CCObject *sender)
{
	CCLog("async next step, load %i", loadPointer);
	if (loadPointer < totalToLoad)
	{
		CCTextureCache::sharedTextureCache()->addImageAsync((*filePathArray)[loadPointer++].c_str(), this, callfuncO_selector(DataLoader::didLoadImage));
	}
	else
	{
		(target->*callBack)();
	}
}
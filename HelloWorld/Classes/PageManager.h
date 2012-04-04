#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include "Page.h"

#include <map>

/**
 * This class is to manage different pages, like insert a page and get a page
 */
class PageManager
{
public:
	static void parseJsonAndRun();
	static void insertPageWithPageNumber(int pageNumber, Page *page);
	
	static Page* getPageByPageNumber(int pageNumber);
	static cocos2d::CCScene* createSceneByPageNumber(int pageNumber);
private:
	// helper functions to create scene
	static void createSprites(Page *page, cocos2d::CCLayer *layer);
	static void createParagraphs(Page *page, cocos2d::CCLayer *layer);
	static void playBackgroundMusic(Page *page);
private:
	// key: page number 
	static std::map<int, Page*> pages;

	// make JsonParser can access pages
	friend class JsonParser;
};

#endif // __PAGE_MANAGER_H__

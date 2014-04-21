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
	static void parseJsonAndRun(const char* pathOfJasonFile);
	
	static Page* getPageByPageNumber(int pageNumber);
	static cocos2d::CCScene* createSceneByPageNumber(int pageNumber);
    static void turnToPage(int pageNumber, bool backWards);
    static void gotoMainMenu(void);
    static void gotoBookList(void);
    static int numberOfPages(void);
    static void clear();
    
private:
	
private:
	// key: page number 
	static std::map<int, Page*> pages;

	// make JsonParser can access pages
	friend class JsonParser;
};

#endif // __PAGE_MANAGER_H__

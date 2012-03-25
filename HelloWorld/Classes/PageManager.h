#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include "Page.h"

#include <map>
#include <json/value.h>

/**
 * This class is to manage different pages, like insert a page and get a page
 */
class PageManager
{
public:
	static void insertPageWithPageNumber(int pageNumber, Page *page);
	static bool parseJson();
	static Page* getPage(int pageNumber);
private:
	// jsonPage is "Page" in json data
	static void parseWithPage(Page* page, Json::Value &jsonPage);
	static void parseWithSettings(Page* page, Json::Value &jsonSettings);
	static void parseWithText(Page* page, Json::Value &jsonText);
	// jsonAPI is "API" in json data
	static void parseWithAPI(Page* page, Json::Value &jsonAPI);
public:
	// key: page number 
	static std::map<int, Page*> pages;
	const static char* pathJsonFile;
};

#endif // __PAGE_MANAGER_H__

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
	static void parseWithBezierByOrBezierTo(Page *page, Json::Value &value, bool isBezierBy);
	static void parseWithEaseBounceInOrEaseBounceOut(Page *page, Json::Value &value, bool isEaseBounceIn);
	static void parseWithEaseInOrEaseOut(Page *page, Json::Value &value, bool isEaseIn);
	static void parseWithFadeInOrFadeOut(Page *page, Json::Value &value, bool isFadeIn);
	static void parseWithFadeTo(Page *page, Json::Value &value);
	static void parseWithFlipxOrFlipy(Page *page, Json::Value &value, bool isFlipx);
	static void parseWithFlipx3dOrFlipy3d(Page *page, Json::Value &value, bool isFlipx3d);
	static void parseWithJumpToOrJumpBy(Page *page, Json::Value &value, bool isJumpTo);
	static void parseWithRotateToOrRotateBy(Page *page, Json::Value &value, bool isRotateTo);
	static void parseWithSkewToOrSkewBy(Page *page, Json::Value &value, bool isSkewTo);
	static void parseWithMoveToOrMoveBy(Page *page, Json::Value &value, bool isMoveTo);
	static void parseWithScaleToOrScaleBy(Page *page, Json::Value &value, bool isScaleTo);
	static void parseWithSequence(Page *page, Json::Value &value);
	static void parseWithSprites(Page *page, Json::Value &value);
	static void parseWithStoryTouchableNode(Page *page, Json::Value &value);
	static void parseWithStorySwipeEnded(Page *page, Json::Value &value);
private:
	// key: page number 
	static std::map<int, Page*> pages;
	const static char* pathJsonFile;
};

#endif // __PAGE_MANAGER_H__

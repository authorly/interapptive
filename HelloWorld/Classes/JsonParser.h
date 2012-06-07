#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "Page.h"

#include <json/value.h>

class JsonParser
{
public:
	static bool parseJson(const char* pathOfJasonFile);
private:
    // parse configurations
    static void parseConfigurations(Json::Value &root);
    
    // parse main menu
    static void parseMainMenu(Json::Value &root);
    static void parseMainMenuAodio(Json::Value &value);
    static void parseMainMenuSprites(Json::Value &value);
    static void parseMenuItems(Json::Value &value);
    static void parseMainMenuAPI(Json::Value &value);
    static void parseMainMenuRunActionsOnEnter(Json::Value &value);
    
    // parse pages
    
    static void parsePages(Json::Value &root);
    
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
	static void parseWithDelayTime(Page *page, Json::Value &value);
    static void parseWithSpawn(Page *page, Json::Value &value);
};

#endif // __JSON_PARSER_H__

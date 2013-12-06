#include "SharedGlobalData.h"

#include "cocos2d.h"

#define KEY_OF_PAGE_NUMBER "currentPageNumber"

static GlobalData *g_sharedGlobalData = 0;

// Zorder of sprite is [1000, 2000], so the zorder of main menu, paragraph and hotspot should be greater than 2000
int GlobalData::ZORDER_MAIN_MENU = 2001;
int GlobalData::ZORDER_PARAGRAPH = 2002;
int GlobalData::ZORDER_HOTSPOT = 2003;

using namespace cocos2d;

GlobalData::GlobalData()
: xScale(0.0f)
, yScale(0.0f)
, minScale(0.0f)
, maxScale(0.0f)
, currentPageNumber(0)
{}

GlobalData::~GlobalData()
{
    // save currentPageNumber
    CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_OF_PAGE_NUMBER, currentPageNumber);
}

GlobalData* GlobalData::sharedGlobalData()
{
    if (g_sharedGlobalData == 0)
    {
        g_sharedGlobalData = new GlobalData();
        g_sharedGlobalData->init();
    }
    
    return g_sharedGlobalData;
}

void GlobalData::save()
{
    CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_OF_PAGE_NUMBER, currentPageNumber);
    CCUserDefault::sharedUserDefault()->flush();
}

void GlobalData::init()
{
    currentPageNumber = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_OF_PAGE_NUMBER, -1);
    
    // if it is the first time to run the app, set last left off page number to 0
    if (currentPageNumber == -1)
    {  
        currentPageNumber = 0;
    }
    
    // calculate all the scale ratio
    xScale = CCDirector::sharedDirector()->getXScale();
    yScale = CCDirector::sharedDirector()->getYScale();
    minScale = xScale > yScale ? yScale : xScale;
    maxScale = xScale > yScale ? xScale : yScale;
}

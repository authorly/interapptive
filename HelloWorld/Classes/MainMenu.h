#ifndef interapptive_MainMenu_h
#define interapptive_MainMenu_h

#include "cocos2d.h"
#include "SharedGlobalData.h"

#include <string>
#include <vector>

enum MainMenuItemType
{
    kNormalMainMenuItemType,    // has storyMode to go to a certain page
    kPlayVideoMainMenuItemType, // play a video
    kUrlMainMenuItemType,       // open a url
};

typedef struct _audio
{
    std::string backgroundMusic;
    bool        isBackgroundMusicLoop;
    // sound effect for main menu title
    std::string effectOnEnter;
} Audio;

typedef struct _mainMenuSpriteInfo
{
    std::string         image;
    int                 spriteTag;
    bool                visible;
    cocos2d::CCPoint    position;
    int                 zOrder;
    double              scale;
} MainMenuSpriteInfo;

typedef  struct _menuItem
{
    std::string         normalStateImage;
    std::string         tappedStateImage;

    std::string         storyMode;
    std::string         videoToPlay;
    std::string         url;
    
    MainMenuItemType    mainMenuItemType;
   
    cocos2d::CCPoint    position;
    double              scale;
    int                 zOrder;
} MainMenuItemInfo;

typedef struct _actionFadeIn
{
    float  duration;
    int    actionTag;
} ActionFadeIn;

typedef struct _actionToRunOnEnter
{
    int spriteTag;
    int actionTag;
} ActionToRunOnEnter;

class MainMenu
{
public:
    static MainMenuSpriteInfo* getSpriteInfoByTag(int spriteTag);
    static ActionFadeIn* getActionFadeInInfoByTag(int actionTag);
    // clear all recorded data
    static void clear();
public:
    // audio
    static Audio audio;
    // sprites
    static std::vector<MainMenuSpriteInfo*> sprites;
    // menu items
    static std::vector<MainMenuItemInfo*> menuItems;
    // actioins to run on enter
    static std::vector<ActionToRunOnEnter*> actionsToRunOnEnter;
    // CCFadeIn
    static std::vector<ActionFadeIn*> actionsFadeIn;
    // Falling object setting
    static FallingObjectSetting fallingObjectSetting;
};

#endif

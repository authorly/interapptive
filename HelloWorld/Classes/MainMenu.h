#ifndef interapptive_MainMenu_h
#define interapptive_MainMenu_h

#include "cocos2d.h"

#include <string>
#include <vector>

typedef struct _audio
{
    std::string backgroundMusic;
    int         backgroundMusicLoops;
    // sound effect for main menu title
    std::string soundEffect;
    int         soundEffectLoops;
} Audio;

typedef struct _mainMenuSpriteInfo
{
    std::string         image;
    int                 spriteTag;
    bool                visible;
    cocos2d::CCPoint    position;
} MainMenuSpriteInfo;

typedef  struct _menuItem
{
    std::string         normalStateImage;
    std::string         tappedStateImage;
    std::string         storyMode;
    cocos2d::CCPoint    position;
} MenuItem;

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
    // audio
    static Audio audio;
    // sprites
    static std::vector<MainMenuSpriteInfo*> sprites;
    // menu items
    static std::vector<MenuItem*> menuItems;
    // actioins to run on enter
    static std::vector<ActionToRunOnEnter*> actionsToRunOnEnter;
    // CCFadeIn
    static std::vector<ActionFadeIn*> actionsFadeIn;
};

#endif

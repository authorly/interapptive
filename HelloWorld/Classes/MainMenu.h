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

typedef struct _sprite
{
    std::string         image;
    int                 spriteTag;
    bool                visible;
    cocos2d::CCPoint    position;
} Sprite;

typedef  struct _menuItem
{
    std::string         normalStateImage;
    std::string         tappedStateImage;
    std::string         storyMode;
    cocos2d::CCPoint    position;
} MenuItem;

typedef struct _actionToRun
{
    int runAfterSwipeNumber;
    int spriteTag;
    int actionTag;
} ActionToRun;

class MainMenu
{
public:
    // audio
    static Audio audio;
    // sprites
    static std::vector<Sprite> sprites;
    // menu items
    static std::vector<MenuItem> menuItems;
    // api??
    // actioins to run on enter
    static std::vector<ActionToRun> actionsToRunOnEnter;
};

#endif

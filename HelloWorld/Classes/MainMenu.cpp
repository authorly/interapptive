#include "MainMenu.h"

using namespace std;

// audio
Audio MainMenu::audio = Audio();
// sprites
vector<MainMenuSpriteInfo*> MainMenu::sprites = vector<MainMenuSpriteInfo*>();
// menu items
vector<MainMenuItemInfo*> MainMenu::menuItems = vector<MainMenuItemInfo*>();
// actioins to run on enter
vector<ActionToRunOnEnter*> MainMenu::actionsToRunOnEnter = vector<ActionToRunOnEnter*>();
// CCFadeIn
vector<ActionFadeIn*> MainMenu::actionsFadeIn = vector<ActionFadeIn*>();
FallingObjectSetting MainMenu::fallingObjectSetting = FallingObjectSetting();

MainMenuSpriteInfo* MainMenu::getSpriteInfoByTag(int spriteTag)
{
    vector<MainMenuSpriteInfo*>::iterator iter;
    for (iter = sprites.begin(); iter != sprites.end(); ++iter)
    {
        if ((*iter)->spriteTag == spriteTag)
        {
            return  *iter;
        }
    }
    
    return NULL;
}

ActionFadeIn* MainMenu::getActionFadeInInfoByTag(int actionTag)
{
    vector<ActionFadeIn*>::iterator iter;
    for (iter = actionsFadeIn.begin(); iter != actionsFadeIn.end(); ++iter)
    {
        if ((*iter)->actionTag == actionTag)
        {
            return *iter;
        }
    }
    
    return NULL;
}
#include "MainMenu.h"

using namespace std;

// audio
Audio MainMenu::audio = Audio();
// sprites
vector<MainMenuSpriteInfo*> MainMenu::sprites = vector<MainMenuSpriteInfo*>();
// menu items
vector<MenuItem*> MainMenu::menuItems = vector<MenuItem*>();
// actioins to run on enter
vector<ActionToRunOnEnter*> MainMenu::actionsToRunOnEnter = vector<ActionToRunOnEnter*>();
// CCFadeIn
vector<ActionFadeIn*> MainMenu::actionsFadeIn = vector<ActionFadeIn*>();
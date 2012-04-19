#include "MainMenu.h"

using namespace std;

Audio MainMenu::audio = Audio();
vector<Sprite> MainMenu::sprites = vector<Sprite>();
vector<MenuItem> MainMenu::menuItems = vector<MenuItem>();
vector<ActionToRun> MainMenu::actionsToRunOnEnter = vector<ActionToRun>();


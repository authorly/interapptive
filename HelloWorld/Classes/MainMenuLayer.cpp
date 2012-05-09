#include "MainMenuLayer.h"
#include "MainMenu.h"
#include "PageManager.h"
#include "SharedGlobalData.h"

#include "SimpleAudioEngine.h"


#define MENU_ITEM_AOTU_PLAY_TAG     100
#define MENU_ITEM_READ_TO_ME_TAG    101
#define MENU_ITEM_READ_MYSELF_TAG   102

using namespace CocosDenshion;
using namespace cocos2d;

StoryMode MainMenuLayer::storyMode = kStoryModeReadItMyself;

MainMenuLayer::MainMenuLayer(): mydialog(NULL)
{
    init();
}

MainMenuLayer::~MainMenuLayer()
{
    CC_SAFE_RELEASE(mydialog);
}

void MainMenuLayer::init()
{
    // preload background music, effect
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(MainMenu::audio.backgroundMusic.c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(MainMenu::audio.soundEffect.c_str());
    
    // add sprites
    for (int i = 0; i < MainMenu::sprites.size(); ++i)
    {
        MainMenuSpriteInfo *spriteInfo = MainMenu::sprites[i];
        CCSprite *sprite = CCSprite::spriteWithFile(spriteInfo->image.c_str());
        
        sprite->setTag(spriteInfo->spriteTag);
        sprite->setIsVisible(spriteInfo->visible);
        sprite->setPosition(spriteInfo->position);
        
        addChild(sprite);
    }
    
    // add menu
    CCMenu *menu = CCMenu::node();
    for (int j = 0; j < MainMenu::menuItems.size(); ++j)
    {
        MenuItem *menuItemInfo = MainMenu::menuItems[j];
        CCMenuItemImage *item = CCMenuItemImage::itemFromNormalImage(menuItemInfo->normalStateImage.c_str(), 
                                                                     menuItemInfo->tappedStateImage.c_str(), 
                                                                     this, 
                                                                     menu_selector(MainMenuLayer::menuItemCallback)
                                                                     );
        // set tag
        if (menuItemInfo->storyMode == "autoPlay")
        {
            item->setTag(MENU_ITEM_AOTU_PLAY_TAG);
        }
        else if (menuItemInfo->storyMode == "readItMyself")
        {
            item->setTag(MENU_ITEM_READ_MYSELF_TAG);
        }
        else
        {
            item->setTag(MENU_ITEM_READ_TO_ME_TAG);
        }
        
        item->setPosition(menuItemInfo->position);
        menu->addChild(item, 1);
    }
    menu->setPosition(ccp(0,0));
    addChild(menu);
}

void MainMenuLayer::onEnter()
{
    bool musicLoop = MainMenu::audio.backgroundMusicLoops == 0 ? true : false;
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic(MainMenu::audio.backgroundMusic.c_str(), musicLoop);
    
    CCLayer::onEnter();
}

void MainMenuLayer::menuItemCallback(cocos2d::CCObject *sender)
{
    // only show dialog that page number greater than 1 last time left off 
    if (GlobalData::sharedGlobalData()->currentPageNumber > 1)
    {
        // should release previous dialog
        CC_SAFE_RELEASE(mydialog);
        
        // pop a dialog to select 
        vector<string> items;
        items.push_back("Resume");
        items.push_back("Start Over");
        items.push_back("Cacel");
        
        mydialog = new MyDialog();
        mydialog->initWithItems("Would you like to resume where you left off?", items, this);
        mydialog->popUp();  
    }
    else 
    {
        // start over
        buttonClicked(1);
    }
      

    // set story mode, no matter if user select "cancle"
    // because it will not turn not page if he selects "cancle"
    int tag = ((CCMenuItem*)sender)->getTag();
    switch (tag) {
        case MENU_ITEM_AOTU_PLAY_TAG:
            storyMode = kSotryModeAutoPlay;
            break;
        case MENU_ITEM_READ_MYSELF_TAG:
            storyMode = kStoryModeReadItMyself;
            break;
        case MENU_ITEM_READ_TO_ME_TAG:
            storyMode = kStoryModeReadToMe;
            break;
            
        default:
            // error
            assert(0);
            break;
    }
}

void MainMenuLayer::buttonClicked(int index)
{
    // resume
    if (index == 0)
    {
        PageManager::turnToPage(GlobalData::sharedGlobalData()->currentPageNumber, false);
    }
    
    // start over
    if (index == 1)
    {
        PageManager::turnToPage(1, false);

    }
}
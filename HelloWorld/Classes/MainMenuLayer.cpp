#include "SimpleAudioEngine.h"

#include "MainMenuLayer.h"
#include "MainMenu.h"

using namespace CocosDenshion;
using namespace cocos2d;

StoryMode MainMenuLayer::storyMode = kStoryModeReadItMyself;

MainMenuLayer::MainMenuLayer()
{
    init();
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
    
}
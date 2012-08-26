#include "MainMenuLayer.h"
#include "MainMenu.h"
#include "PageManager.h"
#include "SharedGlobalData.h"
#include "OpenUrlHelper.h"
#include "SnowmanGameScene.h"
#include "MyPageTurn.h"
#include "Configurations.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
using namespace cocos2d;

StoryMode MainMenuLayer::storyMode = kStoryModeReadItMyself;

MainMenuLayer::MainMenuLayer(): mydialog(NULL)
{
    init();
}

MainMenuLayer::~MainMenuLayer()
{
    CC_SAFE_RELEASE_NULL(mydialog);
}

void MainMenuLayer::init()
{
    // preload background music, effect if exists
    if (MainMenu::audio.backgroundMusic.size() > 0)
    {
        SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(MainMenu::audio.backgroundMusic.c_str());
    }
    if (MainMenu::audio.soundEffect.size() > 0)
    {
        SimpleAudioEngine::sharedEngine()->preloadEffect(MainMenu::audio.soundEffect.c_str());
    }    
    
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
        CCMenuItemImage *menuItemImage = NULL;
        
        MainMenuItemInfo *menuItemInfo = MainMenu::menuItems[j];
        switch (menuItemInfo->mainMenuItemType) {
            case kNormalMainMenuItemType:
                menuItemImage = CCMenuItemImage::itemFromNormalImage(menuItemInfo->normalStateImage.c_str(), 
                                                                             menuItemInfo->tappedStateImage.c_str(), 
                                                                             this, 
                                                                             menu_selector(MainMenuLayer::normalMainMenuItemTouched));
                break;
            case kPlayVideoMainMenuItemType:
                menuItemImage = CCMenuItemImage::itemFromNormalImage(menuItemInfo->normalStateImage.c_str(), 
                                                                     menuItemInfo->tappedStateImage.c_str(), 
                                                                     this, 
                                                                     menu_selector(MainMenuLayer::playVideoMainMenuItemTouched));
                break;
            case kUrlMainMenuItemType:
                menuItemImage = CCMenuItemImage::itemFromNormalImage(menuItemInfo->normalStateImage.c_str(), 
                                                                     menuItemInfo->tappedStateImage.c_str(), 
                                                                     this, 
                                                                     menu_selector(MainMenuLayer::urlMainMenuItemTouched));
                break;
                
            default:
                // error main menu item type
                assert(false);
                break;
        }
        
        menuItemImage->setUserData(menuItemInfo);
        
        menuItemImage->setPosition(menuItemInfo->position);
        menu->addChild(menuItemImage, 1);
    }
    
    // add build snow man menu item (830*90)
    CCMenuItemImage *buildASnowman = CCMenuItemImage::itemFromNormalImage("buildASnowman.png", 
                                                                         "buildASnowman-hover.png",
                                                                         this,
                                                                          menu_selector(MainMenuLayer::buildASnowmanMenuItemTouched));
    buildASnowman->setPosition(ccp(830*XSCALE, 90*YSCALE));
    menu->addChild(buildASnowman, 1);
    
    menu->setPosition(ccp(0,0));
    addChild(menu);
}

void MainMenuLayer::onEnter()
{  
    if (MainMenu::audio.backgroundMusic.size() > 0)
    {
        bool musicLoop = MainMenu::audio.backgroundMusicLoops == 0 ? true : false;
        SimpleAudioEngine::sharedEngine()->playBackgroundMusic(MainMenu::audio.backgroundMusic.c_str(), musicLoop);
    }

    // run action, action type should be CCFadeIn
    vector<ActionToRunOnEnter*> &actionsToRunOnEnter = MainMenu::actionsToRunOnEnter;
    vector<ActionToRunOnEnter*>::iterator iter;
    for (iter = actionsToRunOnEnter.begin(); iter != actionsToRunOnEnter.end(); ++iter)
    {
        // get sprite by tag
        CCSprite *sprite = (CCSprite*)getChildByTag((*iter)->spriteTag);
        assert(sprite != NULL);
        
        // set it visible and set the opacity to 0
        sprite->setOpacity(0);
        sprite->setIsVisible(true);
        
        // init action
        ActionFadeIn *actionInfo = MainMenu::getActionFadeInInfoByTag((*iter)->actionTag);
        CCFadeIn *fadeIn = CCFadeIn::actionWithDuration(actionInfo->duration);
        fadeIn->setTag(actionInfo->actionTag);
        
        // run action
        sprite->runAction(fadeIn);
    }
    
    CCLayer::onEnter();
}

void MainMenuLayer::normalMainMenuItemTouched(cocos2d::CCObject *sender)
{
    // should release previous dialog if it is exist
    CC_SAFE_RELEASE_NULL(mydialog);
    
    // only show dialog that page number greater than 1 last time left off 
    if (GlobalData::sharedGlobalData()->currentPageNumber > 1)
    {
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
    MainMenuItemInfo *mainMenuItemInfo = (MainMenuItemInfo*)((CCMenuItemImage*)sender)->getUserData();
    string &strStoryMode = mainMenuItemInfo->storyMode;
    if (strStoryMode == "autoPlay")
    {
        storyMode = kSotryModeAutoPlay;
    }
    if (strStoryMode == "readItMyself")
    {
        storyMode = kStoryModeReadItMyself;
    }
    if (strStoryMode == "readToMe")
    {
        storyMode = kStoryModeReadToMe;
    }
}

void MainMenuLayer::playVideoMainMenuItemTouched(cocos2d::CCObject *sender)
{
    // stop playing background music
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    
    MainMenuItemInfo *mainMenuItemInfo = (MainMenuItemInfo*)((CCMenuItemImage*)sender)->getUserData(); 
    VideoPlayer::sharedVideoPlayer()->playVideoByFilename(mainMenuItemInfo->videoToPlay.c_str(), true);
}

void MainMenuLayer::urlMainMenuItemTouched(cocos2d::CCObject *sender)
{
    MainMenuItemInfo *mainMenuItemInfo = (MainMenuItemInfo*)((CCMenuItemImage*)sender)->getUserData();  
    
    OpenUrlHelper::openUrl(mainMenuItemInfo->url.c_str());
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

void MainMenuLayer::buildASnowmanMenuItemTouched(cocos2d::CCObject *sender)
{
    // stop back ground music
    if (SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
    {
        SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect(Configurations::forwardEffect.c_str(), false);
    
    CCDirector::sharedDirector()->replaceScene(ccMyTransitionPageTurn::transitionWithDuration(Configurations::pageFlipTransitionDuration, SnowmanGameScene::node(), false));
}

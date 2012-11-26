#include "PageManager.h"
#include "JsonParser.h"
#include "cocos2d.h"
#include "PageLayer.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Configurations.h"
#include "MainMenuLayer.h"
#include "SharedGlobalData.h"
#include "MyScene.h"
#include "MyPageTurn.h"
#include "VideoPlayer.h"

using namespace cocos2d;
using namespace CocosDenshion;

map<int, Page*> PageManager::pages = map<int, Page*>();

void PageManager::parseJsonAndRun(const char* pathOfJasonFile)
{
	if (! JsonParser::parseJson(pathOfJasonFile))
	{
		return;
	}
    
    // preload effects
    SimpleAudioEngine::sharedEngine()->preloadEffect(Configurations::backwardEffect.c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(Configurations::forwardEffect.c_str());
    
    // show main menu
    CCScene *scene = CCScene::node();
    MainMenuLayer *mainMenuLayer = new MainMenuLayer();
    mainMenuLayer->autorelease();
    scene->addChild(mainMenuLayer);
    
    CCDirector::sharedDirector()->runWithScene(scene);
}

void PageManager::gotoMainMenu(void)
{
    // stop background music and effects
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
    
    CCScene *scene = CCScene::node();
    MainMenuLayer *mainMenuLayer = new MainMenuLayer();
    mainMenuLayer->autorelease();
    scene->addChild(mainMenuLayer);
    
    CCDirector::sharedDirector()->replaceScene(scene);
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    // Causes crash
    // SimpleAudioEngine::sharedEngine()->end();
}

Page* PageManager::getPageByPageNumber(int pageNumber)
{
	std::map<int, Page*>::iterator iter = pages.find(pageNumber);
	if (iter != pages.end())
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}

void PageManager::turnToPage(int pageNumber, bool backWards)
{
    // stop back ground music
    if (SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
    {
        SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    }

    CCScene *scene = createSceneByPageNumber(pageNumber);
    if (scene)
    {
        // SimpleAudioEngine::sharedEngine()->end();
        
        // play effect
        if (backWards)
        {
            SimpleAudioEngine::sharedEngine()->playEffect(Configurations::backwardEffect.c_str(), false);
        }
        else 
        {
            SimpleAudioEngine::sharedEngine()->playEffect(Configurations::forwardEffect.c_str(), false);
        }
        
        // turn page
        CCDirector::sharedDirector()->replaceScene(ccMyTransitionPageTurn::transitionWithDuration(Configurations::pageFlipTransitionDuration, scene, backWards));
        
        CCTextureCache::sharedTextureCache()->removeAllTextures();
    }
}

CCScene* PageManager::createSceneByPageNumber(int pageNumber)
{
	CCScene *scene = NULL;
	Page *page = getPageByPageNumber(pageNumber);

	if (page)
	{
		// create a scene
		scene = MyScene::node();
        //scene = CCScene::node();

		// create a layer because we want to receive touch event
		CCLayer *layer = PageLayer::pageLayerWithPage(page);;
		scene->addChild(layer);
        
        GlobalData::sharedGlobalData()->currentPageNumber = page->settings.number;
	}

	return scene;
}

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
    
    CCDirector::sharedDirector()->replaceScene(scene);
}

void PageManager::gotoMainMenu(void)
{
    // stop background music and effects
    SimpleAudioEngine::sharedEngine()->stopAllBackgroundMusic();
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
    SimpleAudioEngine::sharedEngine()->stopAllBackgroundMusic();

    CCScene *scene = createSceneByPageNumber(pageNumber);
    if (scene)
    {
        // turn page
        CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::transitionWithDuration(Configurations::pageFlipTransitionDuration, scene, backWards));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        CCTextureCache::sharedTextureCache()->removeAllTextures();
#else
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
#endif
        
        // play effect
        if (backWards)
        {
            SimpleAudioEngine::sharedEngine()->playEffect(Configurations::backwardEffect.c_str(), false);
        }
        else
        {
            SimpleAudioEngine::sharedEngine()->playEffect(Configurations::forwardEffect.c_str(), false);
        }
        
        GlobalData::sharedGlobalData()->save();
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

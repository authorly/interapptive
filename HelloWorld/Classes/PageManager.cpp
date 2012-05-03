#include "PageManager.h"
#include "JsonParser.h"
#include "cocos2d.h"
#include "PageLayer.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Configurations.h"
#include "MainMenuLayer.h"

using namespace cocos2d;
using namespace CocosDenshion;

map<int, Page*> PageManager::pages = map<int, Page*>();
int PageManager::currentIndexOfPage = 0;

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
    scene->addChild(new MainMenuLayer());
    
    CCDirector::sharedDirector()->runWithScene(scene);
}

void PageManager::gotoMainMenu(void)
{
    // stop background music and effects
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
    
    CCScene *scene = CCScene::node();
    scene->addChild(new MainMenuLayer());
    
    CCDirector::sharedDirector()->replaceScene(scene);
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
        CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::transitionWithDuration(Configurations::pageFlipTransitionDuration, scene, backWards));
    }
}

CCScene* PageManager::createSceneByPageNumber(int pageNumber)
{
	CCScene *scene = NULL;
	Page *page = getPageByPageNumber(pageNumber);

	if (page)
	{
		// create a scene
		scene = CCScene::node();

		// create a layer because we want to receive touch event
		CCLayer *layer = PageLayer::pageLayerWithPage(page);;
		scene->addChild(layer);
        
        currentIndexOfPage = page->settings.number;
	}

	return scene;
}

int PageManager::getCurrentIndexOfPage()
{
    return currentIndexOfPage;
}


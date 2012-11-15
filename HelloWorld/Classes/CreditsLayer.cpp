#include "CreditsLayer.h"
#include "SharedGlobalData.h"
#include "MainMenuLayer.h"

using namespace cocos2d;

void CreditsLayer::onEnter()
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // add background
    CCSprite *background = CCSprite::spriteWithFile("background0000.jpg");
    background->setPosition(ccp(winSize.width/2, winSize.height/2));
    addChild(background);
    
    // add text
    CCLabelTTF *text = CCLabelTTF::labelWithString("stranger in the wood", "PoeticaChanceryIII.ttf", 40);
    text->setPosition(ccp(300*XSCALE, 300*YSCALE));
    text->setColor(ccBLACK);
    addChild(text);
    
    // add menu to go to main menu layer
    CCMenu *menu = CCMenu::node();
    CCMenuItemImage *item = CCMenuItemImage::itemFromNormalImage("home-button.png",
                                                                 "home-button-over.png",
                                                                 this,
                                                                 menu_selector(CreditsLayer::buttonClicked));
    
    item->setPosition(item->getContentSize().width/2, item->getContentSize().height/2);
    menu->addChild(item, 1);
    menu->setPosition(ccp(0,0));
    addChild(menu);
    
    CCLayer::onEnter();
}

void CreditsLayer::buttonClicked(cocos2d::CCObject *sender)
{
    CCScene *scene = CCScene::node();
    MainMenuLayer *layer = new MainMenuLayer();
    layer->autorelease();
    scene->addChild(layer);
    
    CCDirector::sharedDirector()->replaceScene(scene);
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

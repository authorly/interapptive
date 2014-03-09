//
//  LoadingScene.cpp
//  subscription
//
//  Created by minggo on 3/9/14.
//
//

#include "LoadingLayer.h"
#include "PageManager.h"

using namespace cocos2d;

LoadingLayer::LoadingLayer()
{
    CCMenuItemFont *book1 = CCMenuItemFont::itemFromString("book1", this, menu_selector(LoadingLayer::buttonClicked));
    book1->setPosition(ccp(100, 100));
    CCMenu *menu = CCMenu::menuWithItem(book1);
    menu->setPosition(ccp(0,0));
    addChild(menu);
}

void LoadingLayer::buttonClicked(CCObject *sender)
{
    PageManager::parseJsonAndRun("structure-ipad.json");
}

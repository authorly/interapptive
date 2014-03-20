 //
//  LoadingScene.cpp
//  subscription
//
//  Created by minggo on 3/9/14.
//
//

#include "LoadingLayer.h"
#include "PageManager.h"
#include "SubscriptionApp.h"

using namespace cocos2d;

static CCPoint s_tCurPos = CCPointZero;

LoadingLayer::LoadingLayer()
: m_tBeginPos(CCPointZero)
{
    // Repeatable background
    CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("background-wood.png");
    ccTexParams textureParams = { GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT };
	texture->setTexParameters(&textureParams);
    
    m_bgSprite = CCSprite::spriteWithTexture(texture, CCRectMake(0, 0, 1024, 1536));
    m_bgSprite->setAnchorPoint(ccp(0, 0));
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    m_bgSprite->setPosition(ccp(0,winSize.height*-1));
    
    setIsTouchEnabled(true);
    
    addChild(m_bgSprite);
    
    // WIP: listing books from string array (defined externally in SubscriptionApp.h)
    for (int i = 0; i < BOOK_COUNT; ++i)
    {
        CCLog(g_bookTitles[i].c_str());
    }
    
    // Menu item for opening a storybook from its JSON file
    // Should make these dynamically from external list aforementioned (L35 comment)
    CCMenuItemFont *book1 = CCMenuItemFont::itemFromString("book1", this, menu_selector(LoadingLayer::buttonClicked));
    book1->setPosition(ccp(100, m_bgSprite->getPositionX() + 100));
    m_menu = CCMenu::menuWithItem(book1);
    m_menu->setPosition(ccp(0,0));
    addChild(m_menu);

}

void LoadingLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator iterator = pTouches->begin();
    CCTouch* touch = (CCTouch*)(*iterator);
    
    m_tBeginPos = touch->locationInView(touch->view());
    m_tBeginPos = CCDirector::sharedDirector()->convertToGL(m_tBeginPos);
}

void LoadingLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator iterator = pTouches->begin();
    CCTouch* touch = (CCTouch*)(*iterator);
    
    CCPoint touchLocation = touch->locationInView( touch->view() );
    touchLocation = CCDirector::sharedDirector()->convertToGL( touchLocation );
    float nMoveY = touchLocation.y - m_tBeginPos.y;
    
    CCPoint curPos  = m_bgSprite->getPosition();
    CCPoint nextPos = ccp(curPos.x, curPos.y + nMoveY);
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    if (nextPos.y > 0.0f)
    {
        m_bgSprite->setPosition(CCPointZero);
        m_menu->setPosition(ccp(CCPointZero.x, CCPointZero.y +  winSize.height));
        return;
    }
    
    if (nextPos.y < (winSize.height*-1))
    {
        m_bgSprite->setPosition(ccp(0, (winSize.height*-1)));
        m_menu->setPosition(ccp(0, (winSize.height*-1 + winSize.height)));
        return;
    }
    m_bgSprite->setPosition(nextPos);
    m_menu->setPosition(ccp(nextPos.x, nextPos.y + + winSize.height));
    m_tBeginPos = touchLocation;
    s_tCurPos   = nextPos;
}

void LoadingLayer::buttonClicked(CCObject *sender)
{
    PageManager::parseJsonAndRun("structure-ipad.json");
}

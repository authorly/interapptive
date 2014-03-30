//
//  LoadingScene.cpp
//  subscription
//
//  Created by minggo on 3/9/14.
//
//  Chris Whitman 3/29/14:
//            - Scrollable layer of books
//            - Iterates through an array of book cover titles and
//              create menu items for them
//

#include "LoadingLayer.h"
#include "PageManager.h"
#include "SubscriptionApp.h"

using namespace cocos2d;


int bookPositionX = 0;
int bookPositionY = 305;
bool isLastBookOfRow = false;

static CCPoint s_tCurPos = CCPointZero;

LoadingLayer::LoadingLayer()
: m_tBeginPos(CCPointZero)
{
    setIsTouchEnabled(true);
    
    // Create repeatable background from texture
    CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("background-wood.png");
    ccTexParams textureParams = { GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT };
	texture->setTexParameters(&textureParams);
    
    m_bgSprite = CCSprite::spriteWithTexture(texture, CCRectMake(0, 0, 1024, 3078));
    m_bgSprite->setAnchorPoint(ccp(0, 0));
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    m_bgSprite->setPosition(ccp(0,winSize.height*-3));
    
    addChild(m_bgSprite);
    
    // Create menu
    m_menu = CCMenu::node();
    m_menu->setPosition(ccp(0,0));
    addChild(m_menu);
    
    // Book covers on bookshelf
    // Iterates through an array of strings created in Subscription.h
    for (int i = 0; i < BOOK_COUNT; ++i)
    {
        // Add book spacing value to book width for positioning each book
        if(i%3 == 0){
            bookPositionX = bookPositionX + BOOK_SPACING;
        } else {
            bookPositionX = bookPositionX + BOOK_WIDTH + BOOK_SPACING;
        }
        
        // Concatenate book title with file extension of image
        std::stringstream bookTitle;
        bookTitle << g_bookTitles[i].c_str() << ".png";
        std::string bookCoverFile = bookTitle.str();
        
        // Create menu item for current iterations book
        CCMenuItemImage *bookCover = CCMenuItemImage::itemFromNormalImage(bookCoverFile.c_str(),
                                                                          bookCoverFile.c_str(),
                                                                          this,
                                                                          menu_selector(LoadingLayer::buttonClicked));
        bookCover->setPosition(ccp(bookPositionX * cocos2d::CCDirector::sharedDirector()->getXScale(),
                                   bookPositionY * cocos2d::CCDirector::sharedDirector()->getXScale()));
        bookCover->setAnchorPoint(ccp(0,0));
        
        // Add created menu item to the menu
        m_menu->addChild(bookCover, 10);
        
        // Make a new row for books
        if ((i+1) % 3 == 0) {
            bookPositionY = bookPositionY - 512;
            bookPositionX = 0;
        }
    }
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
        m_menu->setPosition(ccp(CCPointZero.x, CCPointZero.y +  winSize.height*3));
        return;
    }
    
    if (nextPos.y < (winSize.height*-3))
    {
        m_bgSprite->setPosition(ccp(0, (winSize.height*-3)));
        m_menu->setPosition(ccp(0, (winSize.height*-3 + winSize.height*3)));
        return;
    }
    m_bgSprite->setPosition(nextPos);
    m_menu->setPosition(ccp(nextPos.x, nextPos.y + winSize.height*3));
    m_tBeginPos = touchLocation;
    s_tCurPos   = nextPos;
}

void LoadingLayer::buttonClicked(CCObject *sender)
{
    PageManager::parseJsonAndRun("structure-ipad.json");
}

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
#include "MyDialog.h"

#define DISTANCE_OF_TO

using namespace cocos2d;

class MyMenu : public CCMenu
{
public:
    static MyMenu* node()
    {
        MyMenu *ret = new MyMenu();
        ret->autorelease();
        return ret;
    }
    
    MyMenu()
    {
        init();
    }
    
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event)
    {
        beginPos = touch->locationInView( touch->view() );
        beginPos = CCDirector::sharedDirector()->convertToGL( beginPos );
        
        return CCMenu::ccTouchBegan(touch, event);
    }
    
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event)
    {
        CCPoint endPos = touch->locationInView(touch->view());
        endPos = CCDirector::sharedDirector()->convertToGL(endPos);
        
        if (fabs(endPos.y - beginPos.y) > 20)
        {
            m_eState = kCCMenuStateWaiting;
            return;
        }
        else
        {
            CCMenu::ccTouchEnded(touch, event);
        }
    }
    
private:
    CCPoint beginPos;
};


int bookPositionX,
    bookPositionY;

static CCPoint s_tCurPos = CCPointZero;

LoadingLayer::LoadingLayer()
: m_tBeginPos(CCPointZero)
{
    bookPositionX = 0;
    bookPositionY = 305;
    
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
    m_menu = MyMenu::node();
    m_menu->setPosition(ccp(0,0));
    addChild(m_menu);
    
    // Book covers on bookshelf
    // Iterates through an array of strings created in Subscription.h
    for (int i = 0; i < BOOK_COUNT; ++i)
    {
        // Add book spacing value to book width for positioning each book
        if(i % 3 == 0){
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
        
        // Give sprite a tag that correlates with books index value from global array (g_bookTitles[])
        // So we can access the title from its tag later
        bookCover->setTag(i);
        
        
        
        // Add created menu item to the menu
        m_menu->addChild(bookCover, 0);
        
        // Make a new row for books
        if ((i+1) % 3 == 0) {
            bookPositionY = bookPositionY - 512;
            bookPositionX = 0;
        }
    }
}

void LoadingLayer::registerWithTouchDispatcher()
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, kCCMenuTouchPriority - 1, false);
}

bool LoadingLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{

    m_tBeginPos = pTouch->locationInView(pTouch->view());
    m_tBeginPos = CCDirector::sharedDirector()->convertToGL(m_tBeginPos);
    
    return true;
}

void LoadingLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint touchLocation = pTouch->locationInView( pTouch->view() );
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
    // Find which storybook was clicked by its tag.
    // The tag is it's index in the global array of book titles (g_bookTitles)
    CCMenuItem* pMenuItem = (CCMenuItem *)(sender);
    int tag = (int)pMenuItem->getTag();
    
    // Concatenate book title with JSON file extension
    std::stringstream jsonLocationStream;
    jsonLocationStream << g_bookTitles[tag].c_str() << ".json";
    std::string jsonLocation = jsonLocationStream.str();

    PageManager::clear();
    PageManager::parseJsonAndRun(jsonLocation.c_str());
}

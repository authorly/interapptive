#ifndef __subscription__LoadingScene__
#define __subscription__LoadingScene__

#include "cocos2d.h"
#include "SubscriptionApp.h"

using namespace cocos2d;

class LoadingLayer : public cocos2d::CCLayer
{
public:
    LoadingLayer();
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void registerWithTouchDispatcher();
    
    void buttonClicked(cocos2d::CCObject *sender);
    
private:
    CCPoint m_tBeginPos;
    CCMenu* m_menu;
    CCSprite* m_bgSprite;
};

#endif /* defined(__subscription__LoadingScene__) */

#ifndef __subscription__LoadingScene__
#define __subscription__LoadingScene__

#include "cocos2d.h"
#include "SubscriptionApp.h"

using namespace cocos2d;

class LoadingLayer : public cocos2d::CCLayer
{
public:
    LoadingLayer();
    
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    
    void buttonClicked(cocos2d::CCObject *sender);
    
private:
    CCPoint m_tBeginPos;
    CCMenu* m_menu;
    CCSprite* m_bgSprite;
};

#endif /* defined(__subscription__LoadingScene__) */

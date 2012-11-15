#include "cocos2d.h"

#ifndef interapptive_CreditsLayer_h
#define interapptive_CreditsLayer_h

class CreditsLayer : public cocos2d::CCLayer
{
public:
    virtual void onEnter();
    void buttonClicked(cocos2d::CCObject *sender);
};

#endif

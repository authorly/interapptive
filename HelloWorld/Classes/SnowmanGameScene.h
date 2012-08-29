#ifndef interapptive_SnowmanGameScene_h
#define interapptive_SnowmanGameScene_h

#include "cocos2d.h"
#include "CCTouchDelegateProtocol.h"
#include "MyDialog.h"

class SnowmanGameScene : public cocos2d::CCScene, public DialogProtocol, public cocos2d::CCTargetedTouchDelegate
{
public:
    ~SnowmanGameScene();
    virtual bool init();
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    virtual void buttonClicked(int index);
    
    void arrowTouched(cocos2d::CCObject *sender);
    // menu item may be: mouth, eye, nose, scarf, hat, mitten
    void snowmanPartsMenuTouched(cocos2d::CCObject *sender);
    void shareMenuTouched(cocos2d::CCObject *sender);
    void homeMenuTouched(cocos2d::CCObject *sender);
    
    SCENE_NODE_FUNC(SnowmanGameScene);
    
private:
    void addArrowMenuAndAddSprite();
    void addShareMenu();
    void addSnowmanPartsMenu();
    int nextIndex(int curIndex);
    int previousIndex(int curIndex);
    
    cocos2d::CCMenuItemSprite *noseMenuItem;
    cocos2d::CCMenuItemSprite *mouthMenuItem;
    cocos2d::CCMenuItemSprite *scarfMenuItem;
    cocos2d::CCMenuItemSprite *mittenMenuItem;
    cocos2d::CCMenuItemSprite *hatMenuItem;
    cocos2d::CCMenuItemSprite *eyeMenuItem;
    
    MyDialog *mydialog;
};

#endif

#ifndef interapptive_cpSprite_h
#define interapptive_cpSprite_h

#include "cocos2d.h"
#include "chipmunk.h"

class cpSprite : public cocos2d::CCSprite, public cocos2d::CCTargetedTouchDelegate
{
public:
    static cpSprite* spriteWithFile(const char *pszFileName);
    
    cpSprite();
    
    void setChipmunkInfo(cpSpace *space, cpBody *body);
    bool selected();
    
    virtual void setPosition(const cocos2d::CCPoint& pos);
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
	virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);  
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
	virtual void touchDelegateRetain();
	virtual void touchDelegateRelease();
    
private:
    void internalSetPosition(const cocos2d::CCPoint& pos);
    // weak ref
    cpBody *body;
    cpSpace *space;
    bool isSelected;
};

#endif

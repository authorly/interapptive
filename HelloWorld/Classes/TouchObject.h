#ifndef __TOUCH_OBJECT_H__
#define __TOUCH_OBJECT_H__

#include "cocos2d.h"

class TouchObject : public cocos2d::CCNode
{
public:
	static TouchObject* objectWithPositionRadiusTargetSel(cocos2d::CCPoint &p, float r, cocos2d::CCObject *t, cocos2d::SEL_SCHEDULE s, int flag);
public:
    TouchObject();
    
	float radius;
	cocos2d::CCObject *target;
	cocos2d::SEL_SCHEDULE selector;
	// flat to determine which touch zone to be touched
	int flag;
    bool touchable;
};

#endif // __TOUCH_OBJECT_H__

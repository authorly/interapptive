#include "TouchObject.h"

using namespace cocos2d;

TouchObject* TouchObject::objectWithPositionRadiusTargetSel(CCPoint &p, float r, CCObject *t, SEL_SCHEDULE s, int flag)
{
	TouchObject *touch = new TouchObject();
	touch->autorelease();
	
	touch->setPosition(p);
	touch->radius = r;
	touch->target = t;
	touch->selector = s;
	touch->flag = flag;

	return touch;
}

TouchObject::TouchObject()
: radius(0.f)
, target(NULL)
, selector(NULL)
, flag(0)
, touchable(true)
{
    
}
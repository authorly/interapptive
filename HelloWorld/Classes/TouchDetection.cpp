#include "TouchDetection.h"
#include "TouchObject.h"

using namespace cocos2d;

// the priority should low than pagelayer
#define TOUCHDETECTION_HANDLER_PRIORITY -10

TouchDetection::TouchDetection()
{
}

TouchDetection::~TouchDetection()
{
	touchZones->release();
}

void TouchDetection::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, TOUCHDETECTION_HANDLER_PRIORITY, true);
}

bool TouchDetection::init()
{
	this->touchZones = new CCMutableArray<TouchObject*>();
	this->touchedObject = NULL;

	return true;
}

TouchObject* TouchDetection::addZoneWithPositionRadiusTargetSel(cocos2d::CCPoint &p, float r, cocos2d::CCObject *t, cocos2d::SEL_SCHEDULE s, int flag)
{
	// create the zone
	TouchObject *obj = TouchObject::objectWithPositionRadiusTargetSel(p, r, t, s, flag);
	// add zone to the array of zones
	touchZones->addObject(obj);

	CCLOG("new zone added");

	return obj;
}

void TouchDetection::emptyZones()
{
	touchZones->removeAllObjects(true);
}

void TouchDetection::enableTouchByFlag(int flag, bool value)
{
    CCMutableArray<TouchObject*>::CCMutableArrayIterator iter;
	for (iter = touchZones->begin(); iter != touchZones->end(); ++iter)
	{
		if ((*iter)->flag == flag)
        {
            (*iter)->touchable = value;
        }
	}
}

bool TouchDetection::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
	CCPoint touchPoint = touch->locationInView(touch->view());
	touchPoint = CCDirector::sharedDirector()->convertToGL(touchPoint);

	// check if a touch is in the zones, will confirm it still is there in touch ended
    CCMutableArray<TouchObject*>::CCMutableArrayIterator iter;
	for (iter = touchZones->begin(); iter != touchZones->end(); ++iter)
	{
		if ((*iter)->touchable
            && ccpDistance(touchPoint, (*iter)->getPosition()) <= (*iter)->radius)
		{
			touchedObject = *iter;
			return true;
		}
	}

	return false;
}

void TouchDetection::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
	CCPoint touchPoint = touch->locationInView(touch->view());
	touchPoint = CCDirector::sharedDirector()->convertToGL(touchPoint);

	if (touchedObject != NULL
        && ccpDistance(touchPoint, touchedObject->getPosition()) <= touchedObject->radius)
	{
		CCObject *target = touchedObject->target;
		SEL_SCHEDULE selector = touchedObject->selector;
		(target->*selector)(touchedObject->flag);
	}

	touchedObject = NULL;
}

void TouchDetection::onExit()
{
	touchZones->removeAllObjects();

	CCTouchDispatcher::sharedDispatcher()->removeDelegate(this);

	CCLayer::onExit();
}

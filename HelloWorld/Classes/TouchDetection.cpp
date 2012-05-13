#include "TouchDetection.h"
#include "TouchObject.h"

using namespace cocos2d;

// the priority should low than pagelayer
#define TOUCHDETECTION_HANDLER_PRIORITY -10

TouchDetection::TouchDetection() : isDebugDrawing(false)
{
}

TouchDetection::~TouchDetection()
{
	touchZones->release();
}

void TouchDetection::onEnter()
{
	CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, TOUCHDETECTION_HANDLER_PRIORITY, true);

	CCLayer::onEnter();
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

/*
void TouchDetection::draw()
{
	if (isDebugDrawing)
	{
        glLineWidth(6);
        glColor4ub(0, 255, 0, 255);
        
		CCMutableArray<TouchObject*>::CCMutableArrayIterator iter;
		for (iter = touchZones->begin(); iter != touchZones->end(); ++iter)
		{			
			ccDrawCircle((*iter)->getPosition(), (*iter)->radius, 0, 20, false);
		}
        
        // restore original values
        glLineWidth(1);
        //glColor4ub(255,255,255,255);
        glColor4f(1.0, 1.0, 1.0, 1.0);
	}
	CCLayer::draw();
}
*/

bool TouchDetection::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
	CCPoint touchPoint = touch->locationInView(touch->view());
	touchPoint = CCDirector::sharedDirector()->convertToGL(touchPoint);

	// check if a touch is in the zones, will confirm it still is there in touch ended
    CCMutableArray<TouchObject*>::CCMutableArrayIterator iter;
	for (iter = touchZones->begin(); iter != touchZones->end(); ++iter)
	{
		if (ccpDistance(touchPoint, (*iter)->getPosition()) <= (*iter)->radius)
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

	if (ccpDistance(touchPoint, touchedObject->getPosition()) <= touchedObject->radius)
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

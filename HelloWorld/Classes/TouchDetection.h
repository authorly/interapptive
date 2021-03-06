#ifndef __TOUCH_DETECTION_H__
#define __TOUCH_DETECTION_H__

#include "cocos2d.h"

class TouchObject;

class TouchDetection : public cocos2d::CCLayer
{
public:
	TouchDetection();
	~TouchDetection();
	TouchObject* addZoneWithPositionRadiusTargetSel(cocos2d::CCPoint &p, float r, cocos2d::CCObject *t, cocos2d::SEL_SCHEDULE s, int flag);
	void emptyZones();
	virtual bool init();
    virtual void registerWithTouchDispatcher(void);
	virtual bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
	virtual void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
	virtual void onExit();
	LAYER_NODE_FUNC(TouchDetection);
    
    void enableTouchByFlag(int flag, bool value);

public:
	cocos2d::CCMutableArray<TouchObject*> *touchZones;
	TouchObject *touchedObject;
};

#endif // __TOUCH_DETECTION_H__

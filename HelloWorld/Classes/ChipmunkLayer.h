#ifndef interapptive_ChipmunkLayer_h
#define interapptive_ChipmunkLayer_h

#include "SharedGlobalData.h"
#include "CCLayer.h"
#include "chipmunk.h"

#include <vector>

class ChipmunkLayer : public cocos2d::CCLayer
{
public:
    static ChipmunkLayer* layerWithPage(FallingObjectSetting *fallingObjectSetting, StaticObjectSetting *staticObjectSetting);
    ~ChipmunkLayer();
    ChipmunkLayer();
    
    void newFallingObject(float dt);
    virtual void update(float dt);
    virtual void onEnter();
    virtual void onExit();
    virtual void didAccelerate(cocos2d::CCAcceleration* pAccelerationValue);
    
    std::vector<cpBody*>& getBodysToBeFreeArray();

private:
    bool init(FallingObjectSetting *fallingObjectSetting, StaticObjectSetting *staticObjectSetting);
    void setupSpace();
    void addFloor();
    void addWalls();
    void createStaticPhysicObject();
    void freeBodies();
private:
    cpSpace *space;

    int totalFallingObjects;
    
    float accX;
    float accY;
    
    bool isFallingObjectTouched;
    
    std::vector<cpBody*> staticBodyArray;
    std::vector<cpBody*> bodyTobeFreeArray;
    
    // weak ref
    FallingObjectSetting *fallingObjectSetting;
    StaticObjectSetting *staticObjectSetting;
};

#endif

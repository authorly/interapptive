#ifndef interapptive_ChipmunkLayer_h
#define interapptive_ChipmunkLayer_h

#include "Page.h"
#include "CCLayer.h"
#include "chipmunk.h"

#include <vector>

class ChipmunkLayer : public cocos2d::CCLayer
{
public:
    static ChipmunkLayer* layerWithPage(Page* page);
    ~ChipmunkLayer();
    ChipmunkLayer();
    
    void newFallingObject(float dt);
    virtual void update(float dt);
    virtual void onEnter();
    virtual void onExit();
    virtual void didAccelerate(cocos2d::CCAcceleration* pAccelerationValue);

private:
    bool init(Page *page);
    void setupSpace();
    void addFloor();
    void addWalls();
    void createStaticPhysicObject();
private:
    cpSpace *space;
    // weak ref
    Page *page;
    int totalFallingObjects;
    
    float accX;
    float accY;
    
    bool isFallingObjectTouched;
    
    std::vector<cpBody*> staticBodyArray;
};

#endif

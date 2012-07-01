#ifndef interapptive_ChipmunkLayer_h
#define interapptive_ChipmunkLayer_h

#include "Page.h"
#include "CCLayer.h"
#include "chipmunk.h"

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
private:
    bool init(Page *page);
    void setupSpace();
    void addFloor();
    void addWalls();
private:
    cpSpace *space;
    // weak ref
    Page *page;
    int totalFallingObjects;
};

#endif

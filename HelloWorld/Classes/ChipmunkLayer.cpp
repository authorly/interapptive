#include "ChipmunkLayer.h"
#include "GCpShapeCache.h"
#include "chipmunk.h"

using namespace cocos2d;
using namespace std;

// chipmunk can only be initialized one time
static void initChipmunk()
{
    static bool isChipmunkInitialized = false;
    if (! isChipmunkInitialized)
    {
        cpInitChipmunk();
        
        isChipmunkInitialized = true;
    }
}

ChipmunkLayer* ChipmunkLayer::layerWithPage(Page *page)
{
    ChipmunkLayer *ret = new ChipmunkLayer();
    if (ret && ret->init(page))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

ChipmunkLayer::~ChipmunkLayer()
{
    cpSpaceFree(space);
    space = NULL;
}

ChipmunkLayer::ChipmunkLayer()
: totalFallingObjects(0)
, page(NULL)
, space(NULL)
, accX(0.f)
, accY(0.f)
{}

bool ChipmunkLayer::init(Page *page)
{
    this->setIsAccelerometerEnabled(true);
    
    this->page = page;
    
    initChipmunk();
    setupSpace();
    
    FallingObjectSetting &fallingObjectSetting = page->settings.fallingObjectSetting;
    
    if (fallingObjectSetting.hasFloor)
    {
        addFloor();
    }
    
    if (fallingObjectSetting.hasWalls)
    {
        addWalls();
    }
    
    GCpShapeCache::sharedShapeCache()->addShapesWithFile(fallingObjectSetting.plistfilename.c_str());
    GCpShapeCache::sharedShapeCache()->addShapesWithFile(page->settings.staicObjectSetting.plistfilename.c_str());
    
    // add static object
    createStaticPhysicObject();
    
    return true;
}

void ChipmunkLayer::onExit()
{
    unscheduleAllSelectors();
    GCpShapeCache::sharedShapeCache()->purge();
    CCLayer::onExit();
}

void ChipmunkLayer::onEnter()
{
    schedule(schedule_selector(ChipmunkLayer::newFallingObject), 0.6f);
    scheduleUpdate();
    CCLayer::onEnter();
}

void ChipmunkLayer::didAccelerate(cocos2d::CCAcceleration* pAccelerationValue)
{
    accX = pAccelerationValue->x;
    accY = pAccelerationValue->y;
}

void ChipmunkLayer::newFallingObject(float dt)
{
    const char *name = GCpShapeCache::sharedShapeCache()->randomShapeName();
  
    // create and add sprite
    char tempName[50];
    sprintf(tempName, "%s.png", name);
    
    // don't create static object
    string strName = tempName;
    if (strName.compare(page->settings.staicObjectSetting.filename.c_str()) == 0)
    {
        return;
    }
    
	CCSprite *sprite = CCSprite::spriteWithFile(tempName);
    addChild(sprite);
    
    // set anchor point
    sprite->setAnchorPoint(GCpShapeCache::sharedShapeCache()->anchorPointForShape(name));
    
    // create physics shape
    cpBody *body = GCpShapeCache::sharedShapeCache()->createBodyWithName(name, space, sprite);
    
    // set position
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int x = rand() % (int)winSize.width;

    // y should not be 0, or the object may stay at top, because there is 
    // segment at top
    body->p = cpVectMake(x, winSize.height - sprite->getContentSize().height/2);
    sprite->setPosition(ccp(body->p.x, body->p.y));
    
    totalFallingObjects++;
    if (totalFallingObjects >= page->settings.fallingObjectSetting.maxNumber)
    {
        unschedule(schedule_selector(ChipmunkLayer::newFallingObject));
    }
}

void ChipmunkLayer::createStaticPhysicObject()
{
    // create and add sprite
    string &filename = page->settings.staicObjectSetting.filename;
	CCSprite *sprite = CCSprite::spriteWithFile(filename.c_str());
    addChild(sprite);
    
    // set anchor point
    string fixtureName = filename.substr(0, filename.find_last_of("."));
    sprite->setAnchorPoint(GCpShapeCache::sharedShapeCache()->anchorPointForShape(fixtureName.c_str()));
    
    // create physics shape
    cpBody *body = GCpShapeCache::sharedShapeCache()->createBodyWithName(fixtureName.c_str(), space, sprite, true);
    body->p = cpVectMake(page->settings.staicObjectSetting.position.x, page->settings.staicObjectSetting.position.y);
    sprite->setPosition(page->settings.staicObjectSetting.position);
}

static void eachBody(cpBody *body, void *data)
{    
	CCSprite *sprite = (CCSprite*)body->data;
	if( sprite )
    {
		sprite->setPosition(ccp(body->p.x, body->p.y));
        sprite->setRotation((float) CC_RADIANS_TO_DEGREES( -body->a));
	}
}

void ChipmunkLayer::update(float delta)
{
    int steps = 2;
	float dt = delta/(float)steps;
    
    space->gravity = cpVectMake(accX * page->settings.fallingObjectSetting.speedX, 
                                accY * page->settings.fallingObjectSetting.speedY);
	
	for(int i=0; i<steps; i++)
    {
		cpSpaceStep(space, dt);
	}
    cpSpaceEachBody(space, &eachBody, NULL);
}

void ChipmunkLayer::setupSpace()
{
    space = cpSpaceNew();
    cpSpaceResizeStaticHash(space, 400.0f, 40);
    cpSpaceResizeActiveHash(space, 100, 600);
    
    space->gravity = cpVectMake(0, 0);
    
    space->elasticIterations = space->iterations;
}

void ChipmunkLayer::addFloor()
{
    cpBody *staticBody = cpBodyNew(INFINITY, INFINITY);
    cpShape *shape;
    
    CCSize wins = CCDirector::sharedDirector()->getWinSize();
    
    // bottom
    shape = cpSegmentShapeNew(staticBody, cpVectMake(0,0), cpVectMake(wins.width,0), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
    
    // top
    shape = cpSegmentShapeNew(staticBody, cpVectMake(0,wins.height), cpVectMake(wins.width,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
}

void ChipmunkLayer::addWalls()
{
    cpBody *staticBody = cpBodyNew(INFINITY, INFINITY);
    cpShape *shape;
    
    CCSize wins = CCDirector::sharedDirector()->getWinSize();
    
    // left
    shape = cpSegmentShapeNew(staticBody, cpVectMake(0,0), cpVectMake(0,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
    
    // right
    shape = cpSegmentShapeNew(staticBody, cpVectMake(wins.width,0), cpVectMake(wins.width,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
}

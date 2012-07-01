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
{}

bool ChipmunkLayer::init(Page *page)
{
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
    
    GCpShapeCache::sharedShapeCache()->addShapesWithFile(fallingObjectSetting.filename.c_str());
    // todo: add static object
    
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

void ChipmunkLayer::newFallingObject(float dt)
{
    const char *name = GCpShapeCache::sharedShapeCache()->randomShapeName();
    
    // create and add sprite
    char tempName[50];
    sprintf(tempName, "%s.png", name);
	CCSprite *sprite = CCSprite::spriteWithFile(tempName);
    addChild(sprite);
    
    // set anchor point
    sprite->setAnchorPoint(GCpShapeCache::sharedShapeCache()->anchorPointForShape(name));
    
    // create physics shape
    cpBody *body = GCpShapeCache::sharedShapeCache()->createBodyWithName(name, space, sprite);
    
    // set position
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    body->p.x = rand() % (int)winSize.width;
    body->p.y = rand() % (int)winSize.height;
    sprite->setPosition(ccp(body->p.x, body->p.y));
    
    totalFallingObjects++;
    if (totalFallingObjects >= page->settings.fallingObjectSetting.maxNumber)
    {
        unschedule(schedule_selector(ChipmunkLayer::newFallingObject));
    }
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
    
    // no gravity, the spped depends on accelerometer
    space->gravity.x = 0;
    space->gravity.y = -900; // for test
    //space->gravity.y = 0;
    
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
    // should we need top?
    // if top is added, the body may stay on top
    /*
    shape = cpSegmentShapeNew(staticBody, cpVectMake(0,wins.height), cpVectMake(wins.width,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
     */
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

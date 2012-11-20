#include "ChipmunkLayer.h"
#include "GCpShapeCache.h"
#include "chipmunk.h"
#include "cpSprite.h"

#define DEFAULT_SPEED_Y   -10.0f

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

// used in eachBody()
static ChipmunkLayer* currentObj = NULL;

ChipmunkLayer* ChipmunkLayer::layerWithPage(FallingObjectSetting *fallingObjectSetting, StaticObjectSetting *staticObjectSetting)
{
    ChipmunkLayer *ret = new ChipmunkLayer();
    if (ret && ret->init(fallingObjectSetting, staticObjectSetting))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

ChipmunkLayer::~ChipmunkLayer()
{
    cpSpaceFreeChildren(space);
    cpSpaceFree(space);
    space = NULL;
    
    // free static bodies
    vector<cpBody*>::iterator iter;
    for (iter = staticBodyArray.begin(); iter != staticBodyArray.end(); ++iter)
    {
        cpBodyFree(*iter);
    }
}

ChipmunkLayer::ChipmunkLayer()
: totalFallingObjects(0)
, space(NULL)
, accX(0.f)
, accY(0.f)
, isFallingObjectTouched(false)
{}

bool ChipmunkLayer::init(FallingObjectSetting *fallingObjectSetting, StaticObjectSetting *staticObjectSetting)
{
    this->setIsAccelerometerEnabled(true);
    
    this->fallingObjectSetting = fallingObjectSetting;
    this->staticObjectSetting = staticObjectSetting;
    
    initChipmunk();
    setupSpace();
    
    if (fallingObjectSetting->hasFloor)
    {
        addFloor();
    }
    
    if (fallingObjectSetting->hasWalls)
    {
        addWalls();
    }
    
    GCpShapeCache::sharedShapeCache()->addShapesWithFile(fallingObjectSetting->plistfilename.c_str());
    if (staticObjectSetting)
    {
        GCpShapeCache::sharedShapeCache()->addShapesWithFile(staticObjectSetting->plistfilename.c_str());
        
        // add static object
        createStaticPhysicObject();
    }
    
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
    schedule(schedule_selector(ChipmunkLayer::newFallingObject), fallingObjectSetting->slowDownSpeed);
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
    bool draggable = fallingObjectSetting->draggble;
  
    // create and add sprite
    char tempName[50];
    sprintf(tempName, "%s.png", name);
    
    // don't create static object
    if (staticObjectSetting)
    {
        string strName = tempName;
        vector<StaticObjectInfo*> &staticObjectInfos = staticObjectSetting->staticObjects;
        vector<StaticObjectInfo*>::iterator iter;
        for (iter = staticObjectInfos.begin(); iter != staticObjectInfos.end(); ++iter)
        {
            if (strName.compare((*iter)->filename.c_str()) == 0)
            {
                return;
            }
        }
    }
    
	CCSprite *sprite = NULL;
    if (draggable)
    {
        sprite = cpSprite::spriteWithFile(tempName);
    }
    else 
    {
        sprite = CCSprite::spriteWithFile(tempName);
    }
    
    addChild(sprite);
    
    // set anchor point
    sprite->setAnchorPoint(GCpShapeCache::sharedShapeCache()->anchorPointForShape(name));
    
    // create physics shape
    cpBody *body = GCpShapeCache::sharedShapeCache()->createBodyWithName(name, space, sprite);
    
    // set position
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int x = rand() % (int)winSize.width;

    body->p = cpVectMake(x, winSize.height + sprite->getContentSize().height/2);
    sprite->setPosition(ccp(body->p.x, body->p.y));
    
    if (draggable)
    {
        ((cpSprite*)sprite)->setChipmunkInfo(space, body);
    }
    
    totalFallingObjects++;
    if (fallingObjectSetting->maxNumber != 0 &&totalFallingObjects >= fallingObjectSetting->maxNumber)
    {
        unschedule(schedule_selector(ChipmunkLayer::newFallingObject));
    }
}

void ChipmunkLayer::createStaticPhysicObject()
{
    vector<StaticObjectInfo*> &staticObjectInfos = staticObjectSetting->staticObjects;
    vector<StaticObjectInfo*>::iterator iter;
    for (iter = staticObjectInfos.begin(); iter != staticObjectInfos.end(); ++iter) 
    {
        // create and add sprite
        string &filename = (*iter)->filename;
        CCSprite *sprite = CCSprite::spriteWithFile(filename.c_str());
        addChild(sprite);
        
        // set anchor point
        string fixtureName = filename.substr(0, filename.find_last_of("."));
        sprite->setAnchorPoint(GCpShapeCache::sharedShapeCache()->anchorPointForShape(fixtureName.c_str()));
        
        // create physics shape
        cpBody *body = GCpShapeCache::sharedShapeCache()->createBodyWithName(fixtureName.c_str(), space, sprite, true);
        staticBodyArray.push_back(body);
        body->p = cpVectMake((*iter)->position.x, (*iter)->position.y);
        sprite->setPosition((*iter)->position);
    }
}

static void eachBody(cpBody *body, void *data)
{
    if (body->p.y < 0)
    {
        ChipmunkLayer *cl = (ChipmunkLayer*)data;
        cl->getBodysToBeFreeArray().push_back(body);
        return;
    }
    
	CCSprite *sprite = (CCSprite*)body->data;
	if( sprite)
    {
        // if it is an type of cpSprite*, it means that the falling object is draggalbe
        // because eachBody() is a local static function, so it can not get the information
        // if the falling object is draggable
        // fix me if dynamic_cast affect performance
        cpSprite *s = dynamic_cast<cpSprite*>(sprite);
        if (s && s->selected())
        {
            return;
        }
            
		sprite->setPosition(ccp(body->p.x, body->p.y));
        sprite->setRotation((float) CC_RADIANS_TO_DEGREES( -body->a));
	}
}

void ChipmunkLayer::update(float delta)
{
    int steps = 2;
	float dt = delta/(float)steps;
    
    float gravityY = accY * fallingObjectSetting->speedY;
    if (gravityY == 0)
    {
        gravityY = DEFAULT_SPEED_Y;
    }
    space->gravity = cpVectMake(accX * fallingObjectSetting->speedX, gravityY);
	
	for(int i=0; i<steps; i++)
    {
		cpSpaceStep(space, dt);
	}
    cpSpaceEachBody(space, &eachBody, this);
    
    // free bodies that go through the floor
    freeBodies();
}

vector<cpBody*>& ChipmunkLayer::getBodysToBeFreeArray()
{
    return bodyTobeFreeArray;
}

void ChipmunkLayer::freeBodies()
{
    vector<cpBody*>::iterator iter;
    for (iter = bodyTobeFreeArray.begin(); iter != bodyTobeFreeArray.end(); ++iter)
    {
        cpBody* body = *iter;
        
        // remove shapes that belong to a body from cpSpace
        vector<cpShape*> shapes = GCpShapeCache::sharedShapeCache()->getShapes(body);
        vector<cpShape*>::iterator iter2;
        for (iter2 = shapes.begin(); iter2 != shapes.end(); ++iter2)
        {
            cpSpaceRemoveShape(space, *iter2);
            cpShapeFree(*iter2);
        }
        
        // remove body from cpSpace
        cpSpaceRemoveBody(space, body);
        
        // remove body from map
        GCpShapeCache::sharedShapeCache()->getBodyShapesMap()->erase(body);
        
        // remove sprite
        ((CCSprite*)body->data)->removeFromParentAndCleanup(true);
        
        // free body
        cpBodyFree(body); 
    }
    
    bodyTobeFreeArray.clear();
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
    cpBody *floorBody = cpBodyNew(INFINITY, INFINITY);
    staticBodyArray.push_back(floorBody);
    cpShape *shape;
    
    CCSize wins = CCDirector::sharedDirector()->getWinSize();
    
    // bottom
    shape = cpSegmentShapeNew(floorBody, cpVectMake(0,0), cpVectMake(wins.width,0), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
    
    // top
    /*
    shape = cpSegmentShapeNew(staticBody, cpVectMake(0,wins.height), cpVectMake(wins.width,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
     */
}

void ChipmunkLayer::addWalls()
{
    cpBody *wallBody = cpBodyNew(INFINITY, INFINITY);
    staticBodyArray.push_back(wallBody);
    cpShape *shape;
    
    CCSize wins = CCDirector::sharedDirector()->getWinSize();
    
    // left
    shape = cpSegmentShapeNew(wallBody, cpVectMake(0,0), cpVectMake(0,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
    
    // right
    shape = cpSegmentShapeNew(wallBody, cpVectMake(wins.width,0), cpVectMake(wins.width,wins.height), 0.0f);
    shape->e = 1.0f; shape->u = 1.0f;
    cpSpaceAddStaticShape(space, shape);
}

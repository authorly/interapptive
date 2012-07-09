#include "GCpShapeCache.h"
#include "CCNS.h"
#include "SharedGlobalData.h"

using namespace cocos2d;
using namespace std;

cpVect cpVectMake(float x, float y)  
{ 
    cpVect vert; 
    vert.x = (x); 
    vert.y = (y); 
    return vert; 
}

static cpFloat area(cpVect *vertices, int numVertices)
{
	cpFloat area = 0.0f;
    
    int r = (numVertices-1);
	
	area += vertices[0].x*vertices[r].y-vertices[r].x*vertices[0].y;
	for (int i=0; i<numVertices-1; ++i)
    {
		area += vertices[r-i].x*vertices[r-(i+1)].y-vertices[r-(i+1)].x*vertices[r-i].y;
	}
	area *= .5f;
	return area;
}


typedef enum 
{
    GFIXTURE_POLYGON,
    GFIXTURE_CIRCLE
} GFixtureType;

class GPlolygon : public cocos2d::CCObject
{
public:
    GPlolygon() 
    {
        
    }
    
    ~GPlolygon()
    {
        free(vertices);
    }
public:
    cpVect *vertices;
    int numVertices;
    cpFloat area;
    cpFloat mass;
    cpFloat momentum;
};

class GFixtureData : public cocos2d::CCObject
{
public:
    GFixtureData()
    {
        polygons = new CCMutableArray<GPlolygon*>();
    }
    
    ~GFixtureData()
    {
        polygons->release();
    }
public:
    GFixtureType fixtureType;
    
    cpFloat mass;
    cpFloat elasticity;
    cpFloat friction;
    
    cpVect surfaceVelocity;
    
	cpCollisionType collisionType;
	cpGroup group;
	cpLayers layers;
    
    cpFloat area;
    cpFloat momentum;
    
    bool isSensor;
    
    // for circles
    cpVect center;
    cpFloat radius;
    
    // for polygons
    cocos2d::CCMutableArray<GPlolygon*> *polygons;
};

class GBodyDef : public cocos2d::CCObject
{
public:
    GBodyDef()
    {
        fixtures = new CCMutableArray<GFixtureData*>();
    }
    
    ~GBodyDef()
    {
         fixtures->release();
    }
public:
    cocos2d::CCPoint anchorPoint;
    cocos2d::CCMutableArray<GFixtureData*> *fixtures;
    cpFloat mass;
    cpFloat momentum;
};

static GCpShapeCache *sharedCache = 0;

// GCpShapeCache
GCpShapeCache* GCpShapeCache::sharedShapeCache()
{
    
    if (!sharedCache) {
        sharedCache = new GCpShapeCache();
    }
    return sharedCache;
}

GCpShapeCache::~GCpShapeCache()
{
    bodyDefs->release();
}

void GCpShapeCache::purge()
{
    CC_SAFE_RELEASE_NULL(sharedCache);
}

GCpShapeCache::GCpShapeCache()
{
    bodyDefs = new CCDictionary<std::string, CCObject*>();
}

CCPoint GCpShapeCache::anchorPointForShape(const char *shape)
{
    GBodyDef *bd = (GBodyDef*)bodyDefs->objectForKey(shape);
    assert(bd);
    return bd->anchorPoint;
}

cpBody* GCpShapeCache::createBodyWithName(const char *name, cpSpace *space, void *data, bool isStatic)
{
    GBodyDef *bd = (GBodyDef*)bodyDefs->objectForKey(name);
    assert(bd != 0);
    if (!bd) {
        return 0;
    }
    
    // create and add body to space
    cpBody *body = NULL;
    if (isStatic) {
        body = cpBodyNew(INFINITY, INFINITY);
    }
    else {
        body = cpBodyNew(bd->mass, bd->momentum);
    }
    
    // set the center point
    body->p = cpVectMake(bd->anchorPoint.x, bd->anchorPoint.y);
    
    // set the data
    body->data = data;
    
    // add space to body
    if (! isStatic) {
        cpSpaceAddBody(space, body);
    }
    
    // iterate over fixtures
    CCMutableArray<GFixtureData*>::CCMutableArrayIterator iter;
    for (iter = bd->fixtures->begin(); iter != bd->fixtures->end(); ++iter) {
        GFixtureData *fd = *iter;
        
        if (fd->fixtureType == GFIXTURE_CIRCLE) {
            cpShape *shape = cpCircleShapeNew(body, fd->radius, fd->center);
            
            // set values
            shape->e = fd->elasticity; 
            shape->u = fd->friction;
            shape->surface_v = fd->surfaceVelocity;
            shape->collision_type = fd->collisionType;
            shape->group = fd->group;
            shape->layers = fd->layers;
            shape->sensor = fd->isSensor;
            
            // add shape to space
            cpSpaceAddShape(space, shape);
        }
        else {
            // iterate over polygons
            CCMutableArray<GPlolygon*>::CCMutableArrayIterator iter;
            for (iter = fd->polygons->begin(); iter != fd->polygons->end(); ++iter) {
                GPlolygon *p = *iter;
                
                // create new shape
                cpShape* shape = cpPolyShapeNew(body, p->numVertices, p->vertices, cpVectMake(0, 0));
                
                // set values
                shape->e = fd->elasticity; 
                shape->u = fd->friction;
                shape->surface_v = fd->surfaceVelocity;
                shape->collision_type = fd->collisionType;
                shape->group = fd->group;
                shape->layers = fd->layers;
                shape->sensor = fd->isSensor;
                
                // add shape to space
                cpSpaceAddShape(space, shape);
            }
        }
    }
    
    return body;
}

static const char * valueForKey(const char *key, CCDictionary<string, CCObject*> *dict)
{
    if (dict)
    {
        CCString *pString = (CCString*)dict->objectForKey(std::string(key));
        return pString ? pString->m_sString.c_str() : "";
    }
    return "";
}

typedef CCDictionary<std::string, CCObject*> ObjectDict;

bool GCpShapeCache::addShapesWithFile(const char *filename)
{
    const char *fullName = CCFileUtils::fullPathFromRelativePath(filename);
    ObjectDict *dict = CCFileUtils::dictionaryWithContentsOfFile(fullName);
    assert(dict != NULL);
    assert(dict->count() != 0);
    
    ObjectDict *dictionary = (ObjectDict*)dict->objectForKey("metadata");
    int format = static_cast<CCString*>(dictionary->objectForKey("format"))->toInt();
    
    assert(format == 1);
    if (format != 1) {
        return false;
    }
    
    ObjectDict *bodyDict = (ObjectDict*)dict->objectForKey("bodies");
    
    bodyDict->begin();
    string bodyName;
    ObjectDict *bodyData;
    while ((bodyData = (ObjectDict *)bodyDict->next(&bodyName))) {
        // create body object
        GBodyDef *bodyDef = new GBodyDef();
        bodyDef->autorelease();
        
        // add the body element to the hash
        bodyDefs->setObject(bodyDef, bodyName.c_str()); 
        
        // set anchor point
        bodyDef->anchorPoint = CCPointFromString(static_cast<CCString *>(bodyData->objectForKey("anchorpoint"))->toStdString().c_str());
        
        // iterate through the fixtures
        CCMutableArray<ObjectDict *> *fixtureList = (CCMutableArray<ObjectDict *> *)(bodyData->objectForKey("fixtures"));
        
        float totalMass = 0.0f;
        cpFloat totalBodyMomentum = 0.0f;
        CCMutableArray<ObjectDict *>::CCMutableArrayIterator iter;
		for (iter = fixtureList->begin(); iter != fixtureList->end(); ++iter) {
            ObjectDict *fixtureData = *iter;
            
            // create fixture
            GFixtureData *fd = new GFixtureData();
            if (!fd) {
                return false;
            }
            fd->autorelease();
            
            // add the fixture to the body
            bodyDef->fixtures->addObject(fd);
            
            fd->friction = static_cast<CCString *>(fixtureData->objectForKey("friction"))->toFloat();
            fd->elasticity = static_cast<CCString *>(fixtureData->objectForKey("elasticity"))->toFloat();
            fd->mass = static_cast<CCString *>(fixtureData->objectForKey("mass"))->toFloat();
            CCPoint tempPoint = CCPointFromString(static_cast<CCString *>(fixtureData->objectForKey("surface_velocity"))->toStdString().c_str());
            fd->surfaceVelocity = cpVectMake(tempPoint.x, tempPoint.y);
            fd->layers = static_cast<CCString*>(fixtureData->objectForKey("layers"))->toInt();
            fd->group = static_cast<CCString*>(fixtureData->objectForKey("group"))->toInt();
            fd->collisionType = static_cast<CCString*>(fixtureData->objectForKey("collision_type"))->toInt();
            fd->isSensor = static_cast<CCString*>(fixtureData->objectForKey("isSensor"))->toStdString().compare("YES") == 0;
            
            string fixtureType = static_cast<CCString*>(fixtureData->objectForKey("fixture_type"))->toStdString();
            
            cpFloat totalArea = 0.0f;
            
            // sum up total mass for the body
            totalMass += fd->mass;
            
            // read polygon fixtures. One convave fixture may consist of several convex polygons
            if (fixtureType.compare("POLYGON") == 0) {
                CCMutableArray<ObjectDict *> *polygonsArray = (CCMutableArray<ObjectDict *> *)(fixtureData->objectForKey("polygons"));
                
                fd->fixtureType = GFIXTURE_POLYGON;
                
                CCMutableArray<ObjectDict *>::CCMutableArrayIterator iter;
                
				for (iter = polygonsArray->begin(); iter != polygonsArray->end(); ++iter) {
                    CCMutableArray<CCString *> *polygonArray = (CCMutableArray<CCString *> *)(*iter);
                    GPlolygon *poly = new GPlolygon();
                    poly->autorelease();
                    if (!poly) {
                        return false;
                    }
                    
                    // add the polygon to the fixture
                    fd->polygons->addObject(poly);
                    
                    // add vertices
                    poly->numVertices = polygonArray->count();
                    cpVect *vertices = poly->vertices = (cpVect *)malloc(sizeof(cpVect) * poly->numVertices);
                    if (!vertices) {
                        return false;
                    }
                    
                    int vindex=0;
                    CCMutableArray<CCString *>::CCMutableArrayIterator piter;
					
					for (piter = polygonArray->begin(); piter != polygonArray->end(); ++piter) {
                        CCPoint offset = CCPointFromString((*piter)->toStdString().c_str());
                        vertices[vindex] = cpVectMake(offset.x * XSCALE, offset.y * YSCALE);
                        vindex++;
                    }
                    
                    // calculate area of the polygon (needed to calculate the mass)
                    poly->area = area(vertices, poly->numVertices);
                    
                    // add up all area
                    totalArea += poly->area;
                }
            }
            else if (fixtureType.compare("CIRCLE") == 0)
            {
                fd->fixtureType = GFIXTURE_CIRCLE;
                
                ObjectDict *circleData = (ObjectDict *)fixtureData->objectForKey("circle");
                
                fd->radius = static_cast<CCString *>(circleData->objectForKey("radius"))->toFloat() * MIN_SCALE;
				CCPoint p = CCPointFromString(static_cast<CCString *>(circleData->objectForKey("position"))->toStdString().c_str());
                fd->center = cpVectMake(p.x, p.y);
                totalArea += 3.1415927*fd->radius*fd->radius;
            }
            else {
                // unknown type
                assert(0);
            }
            
            fd->area = totalArea;
            
            // update sub polygon's masses and momentum
            cpFloat totalFixtureMomentum = 0.0f;
            
            if(totalArea)
            {
                if(fd->fixtureType == GFIXTURE_CIRCLE)
                {
                    totalFixtureMomentum += cpMomentForCircle(fd->mass, fd->radius, fd->radius, fd->center);
                }
                else
                {
                    CCMutableArray<GPlolygon*>::CCMutableArrayIterator iter;
                    for(iter = fd->polygons->begin(); iter != fd->polygons->end(); ++iter)
                    {
                        GPlolygon *p = *iter;
                        
                        // update mass
                        p->mass = (p->area * fd->mass) / fd->area;                    
                        
                        // calculate momentum
                        p->momentum = cpMomentForPoly(p->mass, p->numVertices, p->vertices, cpVectMake(0, 0));
                        
                        // calculate total momentum
                        totalFixtureMomentum += p->momentum;
                    }                              
                }                
            }
            fd->momentum = totalFixtureMomentum;
            totalBodyMomentum = totalFixtureMomentum;
        }
        
        // set bodies total mass
        bodyDef->mass = totalMass;
        bodyDef->momentum = totalBodyMomentum;
    }
    bodyDict->end();
    
    return true;
}

const char* GCpShapeCache::randomShapeName()
{
    int randIndex = rand() % bodyDefs->count();
    return bodyDefs->allKeys()[randIndex].c_str();
}

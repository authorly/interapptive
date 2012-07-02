
#ifndef HelloWorld_GCpShapeCache_h
#define HelloWorld_GCpShapeCache_h

#include "cocos2d.h"
#include "chipmunk.h"
#include <string>

extern cpVect cpVectMake(float x, float y);

class GCpShapeCache : public cocos2d::CCObject
{
public:
    static GCpShapeCache* sharedShapeCache();
    GCpShapeCache();
    ~GCpShapeCache();
    bool addShapesWithFile(const char* filename);
    cpBody* createBodyWithName(const char* name, cpSpace* space, void* data);
    cocos2d::CCPoint anchorPointForShape(const char* shape);
    const char* randomShapeName();
    void purge();
private:
    cocos2d::CCDictionary<std::string, CCObject*> *bodyDefs;
};

#endif

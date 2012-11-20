
#ifndef HelloWorld_GCpShapeCache_h
#define HelloWorld_GCpShapeCache_h

#include "cocos2d.h"
#include "chipmunk.h"
#include <string>
#include <map>

extern cpVect cpVectMake(float x, float y);

class GCpShapeCache : public cocos2d::CCObject
{
public:
    static GCpShapeCache* sharedShapeCache();
    GCpShapeCache();
    ~GCpShapeCache();
    bool addShapesWithFile(const char* filename);
    cpBody* createBodyWithName(const char* name, cpSpace* space, void* data, bool isStatic = false);
    cocos2d::CCPoint anchorPointForShape(const char* shape);
    const char* randomShapeName();
    const std::vector<cpShape*>& getShapes(cpBody *body);
    std::map< cpBody*, std::vector<cpShape*> >* getBodyShapesMap();
    void purge();
private:
    cocos2d::CCDictionary<std::string, CCObject*> *bodyDefs;
    std::map< cpBody*, std::vector<cpShape*> > *bodyShapesMap;
};

#endif

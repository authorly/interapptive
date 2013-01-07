#ifndef interapptive_SharedGlobalData_h
#define interapptive_SharedGlobalData_h

#include "cocos2d.h"

#include <vector>
#include <string>

/** This class is used to record global data
 */

#define XSCALE      (GlobalData::sharedGlobalData()->xScale)
#define YSCALE      (GlobalData::sharedGlobalData()->yScale)
#define MIN_SCALE   (GlobalData::sharedGlobalData()->minScale)

class GlobalData
{
public:
    static GlobalData* sharedGlobalData();
    
    ~GlobalData();
    
    // save current page number
    void save();
    
private:
    GlobalData();
    void init();
    
public:
    // the page number of current scene, it should be recorded after app exits
    // and initialize it at app begins from recorded file
    int currentPageNumber;
    // scale factor of x coordinate
    float xScale;
    // scale factor of y coordinate
    float yScale;
    // min value of xScale and yScale
    float minScale;
    // max value of xScale and yScale
    float maxScale;
};

typedef struct fallingPhysicsSettings{
    bool    draggble;
    int     maxNumber;
    int     speedX;
    int     speedY;
    int     spinSpeed;
    float   slowDownSpeed;
    bool    hasFloor;
    bool    hasWalls;
    cocos2d::CCPoint    dropBetweenPoints;
    std::string         filename;
    std::string         plistfilename;
}FallingObjectSetting;

typedef struct staticObjectInfo{
    std::string         filename;
    cocos2d::CCPoint    position;
} StaticObjectInfo;

typedef struct staticPhysicsSettings{
    std::string                     plistfilename;
    std::vector<StaticObjectInfo*>   staticObjects;
}StaticObjectSetting;

#endif

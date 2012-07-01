#ifndef interapptive_SharedGlobalData_h
#define interapptive_SharedGlobalData_h

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

#endif

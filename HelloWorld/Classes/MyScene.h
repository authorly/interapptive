#ifndef interapptive_MyScene_h
#define interapptive_MyScene_h

#include "cocos2d.h"

class MyScene : public cocos2d::CCScene
{
public:
    SCENE_NODE_FUNC(MyScene);
    
    virtual bool init();
    virtual void onEnterTransitionDidFinish();
};

#endif

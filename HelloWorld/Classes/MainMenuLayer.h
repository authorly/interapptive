#ifndef interapptive_MainMenuLayer_h
#define interapptive_MainMenuLayer_h

#include "cocos2d.h"

enum StoryMode {
    kSotryModeAutoPlay,
    kStoryModeReadItMyself,
    kStoryModeReadToMe,
};

class MainMenuLayer : public cocos2d::CCLayer
{
public:
   MainMenuLayer();
    
    virtual void onEnter();
    void menuItemCallback(cocos2d::CCObject *sender);
    
private:
    void init();
    
public:
    static StoryMode storyMode;
};

#endif

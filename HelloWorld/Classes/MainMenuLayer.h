#ifndef interapptive_MainMenuLayer_h
#define interapptive_MainMenuLayer_h

#include "cocos2d.h"
#include "MyDialog.h"
#include "VideoPlayer.h"

enum StoryMode {
    kSotryModeAutoPlay,
    kStoryModeReadItMyself,
    kStoryModeReadToMe,
};

class MainMenuLayer : public cocos2d::CCLayerColor, public DialogProtocol, public VideoPlayProtocol
{
public:
   MainMenuLayer();
    ~MainMenuLayer();
    
    virtual void onEnter();
    // DialogProtocol
    virtual void buttonClicked(int index);
    
    void normalMainMenuItemTouched(cocos2d::CCObject *sender);
    void playVideoMainMenuItemTouched(cocos2d::CCObject *sender);
    void urlMainMenuItemTouched(cocos2d::CCObject *sender);
    //void buildASnowmanMenuItemTouched(cocos2d::CCObject *sender);
    void creditsItemTouched(cocos2d::CCObject *sender);
    
private:
    void init();
    
public:
    static StoryMode storyMode;
    
private:
    MyDialog *mydialog;
};

#endif

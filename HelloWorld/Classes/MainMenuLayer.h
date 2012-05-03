#ifndef interapptive_MainMenuLayer_h
#define interapptive_MainMenuLayer_h

#include "cocos2d.h"
#include "MyDialog.h"

enum StoryMode {
    kSotryModeAutoPlay,
    kStoryModeReadItMyself,
    kStoryModeReadToMe,
};

class MainMenuLayer : public cocos2d::CCLayer, public DialogProtocol
{
public:
   MainMenuLayer();
    ~MainMenuLayer();
    
    virtual void onEnter();
    virtual void buttonClicked(int index);
    void menuItemCallback(cocos2d::CCObject *sender);
    
private:
    void init();
    
public:
    static StoryMode storyMode;
    
private:
    MyDialog *mydialog;
};

#endif

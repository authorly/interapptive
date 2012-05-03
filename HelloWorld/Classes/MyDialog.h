#ifndef interapptive_MyDialog_h
#define interapptive_MyDialog_h

#include <string>
#include <vector>

#include "cocos2d.h"

class DialogProtocol
{
public:
    virtual void buttonClicked(int index) = 0;
};

class MyDialog : public cocos2d::CCObject
{
public:
    MyDialog();
    ~MyDialog();
    void initWithItems(std::string title, std::vector<std::string> &buttons, DialogProtocol *dialogProtocol);
    void popUp();
    
private:
    void *internalObject;
};

#endif

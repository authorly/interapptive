#ifndef __authorly__LoginLayer__
#define __authorly__LoginLayer__

#include <iostream>

#include "cocos2d.h"
#include "MyDialog.h"

class LoginLayer : public cocos2d::CCLayer, LoginProtocol
{
public:
    LoginLayer();
    virtual void buttonClicked(const char* userName, const char* password);
    
private:
    void showLogin();
    
    cocos2d::CCLabelTTF *errorMessage;
};

#endif /* defined(__authorly__LoginLayer__) */

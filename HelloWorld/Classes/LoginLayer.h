#ifndef __authorly__LoginLayer__
#define __authorly__LoginLayer__

#include <curl/curl.h>
#include "cocos2d.h"
#include "MyDialog.h"

class LoginLayer : public cocos2d::CCLayer, LoginProtocol
{
public:
    LoginLayer();
    ~LoginLayer();
    virtual void buttonClicked(const char* userName, const char* password);
    void goToBookshelf(cocos2d::CCObject *sender);
    
private:
    void showLogin(const char* userName, const char* password);
    
    cocos2d::CCLabelTTF *errorMessage;
    CURL *curl;
};

#endif /* defined(__authorly__LoginLayer__) */

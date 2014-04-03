#ifndef __authorly__LoginLayer__
#define __authorly__LoginLayer__

#include <curl/curl.h>
#include "cocos2d.h"
#include "MyDialog.h"

// This macro is used to control whether to add a menu to pass login stage.
// Should comment it when releasing.
#define LOGIN_DEBUG

class LoginLayer : public cocos2d::CCLayer, LoginProtocol
{
public:
    LoginLayer();
    ~LoginLayer();
    virtual void buttonClicked(const char* userName, const char* password);
    
#ifdef LOGIN_DEBUG
    void goToBookshelf(cocos2d::CCObject *sender);
#endif
    
private:
    void showLogin(const char* userName, const char* password);
    
    cocos2d::CCLabelTTF *errorMessage;
    CURL *curl;
};

#endif /* defined(__authorly__LoginLayer__) */

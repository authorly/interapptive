#include "LoginLayer.h"
#include <string>
#include "MyDialog.h"
#include "LoadingLayer.h"
#include "CCUserDefault.h"
#include "SharedGlobalData.h"
#include "FlurryX.h"

using namespace cocos2d;

LoginLayer::LoginLayer()
: errorMessage(NULL)
{
    // White background
    if( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)) ) //RGBA
    {
        return;
    }
    
    errorMessage = CCLabelTTF::labelWithString("", "Arial", 35);
    errorMessage->setColor(ccc3(0,0,0));
    errorMessage->retain();
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    errorMessage->setPosition(ccp(size.width/2, size.height/4 - 50));
    addChild(errorMessage);
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    showLogin("", "");
    
    
#ifdef LOGIN_DEBUG
    CCLabelTTF *label = CCLabelTTF::labelWithString("go to bookshelf", "Arial", 30);
    CCMenuItemLabel *item = CCMenuItemLabel::itemWithLabel(label, this, menu_selector(LoginLayer::goToBookshelf));
    item->setPosition(ccp(200, 50));
    CCMenu *menu = CCMenu::menuWithItem(item);
    menu->setPosition(ccp(0, 0));
    addChild(menu);
#endif
}

#ifdef LOGIN_DEBUG
void LoginLayer::goToBookshelf(cocos2d::CCObject *sender)
{
    CCScene *scene = CCScene::node();
    LoadingLayer *loadingLayer = new LoadingLayer();
    scene->addChild(loadingLayer);
    loadingLayer->release();
    CCDirector::sharedDirector()->replaceScene(scene);
    Login::sharedLogin()->hide();
}
#endif

LoginLayer::~LoginLayer()
{
    errorMessage->release();
    
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void LoginLayer::buttonClicked(const char *userName, const char *password)
{
#ifdef LOGIN_DEBUG
    userName =  "waseem@authorly.com";
    password = "tester";=
#endif
    
    if (strcmp(userName, "") == 0)
    {
        errorMessage->setString("please input user name");
        showLogin(userName, "");
        return;
    }
    
    if (strcmp(password, "") == 0)
    {
        errorMessage->setString("please input password");
        showLogin("", password);
        return;
    }
    
    // send user name and password to server
    
    // construct data, its format is "email=xxx&password=yyy"
    std::string data("email=");
    data.append(userName);
    data.append("&");
    data.append("password=");
    data.append(password);
    
    curl_easy_setopt(curl, CURLOPT_URL, "http://alpha.bookfairapp.com/api/sessions.json");
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    CURLcode res = curl_easy_perform(curl);
    
    long httpCode = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (httpCode == 200 && res != CURLE_ABORTED_BY_CALLBACK)
    {
        //Succeeded
        
        // go to bookshelf
        CCScene *scene = CCScene::node();
        LoadingLayer *loadingLayer = new LoadingLayer();
        scene->addChild(loadingLayer);
        loadingLayer->release();
        CCDirector::sharedDirector()->replaceScene(scene);
        
        Login::sharedLogin()->setDelegate(NULL);
        
        // record that, user has already logined
        CCUserDefault::sharedUserDefault()->setBoolForKey(HAS_LOGINED, true);
    }
    else
    {
        //Failed
        
        CCLOG("user name is %s, password is %s", userName, password);
        errorMessage->setString("user name or password error");
        
        showLogin(userName, password);
    }
}

void LoginLayer::showLogin(const char* userName, const char* password)
{
    Login *loginView = Login::sharedLogin();
    loginView->setDelegate(this);
    loginView->popUp(userName, password);
}

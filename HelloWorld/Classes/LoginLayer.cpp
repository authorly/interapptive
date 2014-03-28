#include "LoginLayer.h"
#include "MyDialog.h"
#include "LoadingLayer.h"

using namespace cocos2d;

LoginLayer::LoginLayer() : errorMessage(NULL)
{
    showLogin();
}

void LoginLayer::buttonClicked(const char *userName, const char *password)
{
    if (strcmp(userName, "") == 0 && strcmp(password, "") == 0)
    {
        Login::sharedLogin()->setDelegate(NULL);
        // go to loading scene
        CCScene* loadingScene = CCScene::node();
        CCLayer* loadingLayer = new LoadingLayer();
        loadingScene->addChild(loadingLayer);
        loadingLayer->release();
        CCDirector::sharedDirector()->replaceScene(loadingScene);
    }
    else
    {
        CCLog("user name is %s, password is %s", userName, password);
        errorMessage = CCLabelTTF::labelWithString("user name or password error", "Arial", 35);
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        errorMessage->setPosition(ccp(size.width/2, size.height/2  -50));
        addChild(errorMessage);
        
        showLogin();
    }
}

void LoginLayer::showLogin()
{
    Login *loginView = Login::sharedLogin();
    loginView->setDelegate(this);
    loginView->popUp();
}

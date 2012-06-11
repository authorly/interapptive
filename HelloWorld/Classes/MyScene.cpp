#include "MyScene.h"

using namespace cocos2d;

bool MyScene::init()
{
    if (CCScene::init())
    {
        CCDirector::sharedDirector()->setDepthTest(true);
        
        return true;
    }
    
    return false;
}

void MyScene::onEnterTransitionDidFinish()
{
    CCScene::onEnterTransitionDidFinish();
    CCDirector::sharedDirector()->setDepthTest(false);  
}

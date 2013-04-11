#include "Share.h"
#include "jni.h"
#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"
#include <string>

using namespace cocos2d;

#define PATH_TO_SAVE

static std::string getExternalPath()
{
    JniMethodInfo methodInfo;
    
    if (JniHelper::getStaticMethodInfo(methodInfo,
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "getExternalPath",
                                       "()Ljava/lang/String;")
        )
    {
        // invoke java method
        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        return JniHelper::jstring2string(str);
    }
}

static void saveScreenShot()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCRenderTexture* texture = CCRenderTexture::renderTextureWithWidthAndHeight((int)size.width, (int)size.height);
    texture->setPosition(ccp(size.width/2, size.height/2));
    texture->begin();
    CCDirector::sharedDirector()->myDrawScene();
    texture->end();
    
    // get path to saved
    
    texture->saveBuffer(getExternalPath().c_str(), kCCImageFormatPNG);
}

void shareOnFaceBook()
{
    JniMethodInfo methodInfo;
    
    saveScreenShot();
    
    if (JniHelper::getStaticMethodInfo(methodInfo, 
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "shareOnFaceBook",
                                       "()V")
        )
    {
        // invoke java method
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
}

void shareOnTwitter()
{
    JniMethodInfo methodInfo;
    
    saveScreenShot();
    
    if (JniHelper::getStaticMethodInfo(methodInfo, 
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "shareOnTwitter",
                                       "()V")
        )
    {
        // invoke java method
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
}

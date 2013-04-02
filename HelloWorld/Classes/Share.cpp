#include "Share.h"
#include "jni.h"
#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

void shareOnFaceBook()
{
    JniMethodInfo methodInfo;
    
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

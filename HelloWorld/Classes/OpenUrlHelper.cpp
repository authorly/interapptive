#include "OpenUrlHelper.h"

#include "cocos2d.h"

#include "jni.h"
#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

void OpenUrlHelper::openUrl(const char *url)
{
    JniMethodInfo methodInfo;
    
    if (JniHelper::getStaticMethodInfo(methodInfo, 
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "gotoUrl",
                                       "(Ljava/lang/String;)V")
        )
    {
        // url
        jstring jstrUrl = methodInfo.env->NewStringUTF(url);
        
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrUrl);
        
        // release reference
        methodInfo.env->DeleteLocalRef(jstrUrl);
    }
}

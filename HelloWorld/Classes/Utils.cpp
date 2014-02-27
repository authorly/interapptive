//
//  Utils.cpp
//  authorly
//
//  Created by minggo on 3/8/14.
//
//

#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"
#include "jni.h"
#include "Utils.h"

using namespace cocos2d;

int Utils::getDPI()
{
    JniMethodInfo methodInfo;
    
    if (JniHelper::getStaticMethodInfo(methodInfo, 
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "getDPI",
                                       "(V)I")
        )
    {
        // invoke java method
        return methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
    }
    else
    {
        return 160;
    }
}

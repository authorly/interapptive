#include "VideoPlayer.h"
#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"
#include "jni.h"

using namespace cocos2d;

static VideoPlayer* g_videoPlayer = 0;

VideoPlayer* VideoPlayer::sharedVideoPlayer()
{
    if (g_videoPlayer == 0)
    {
        g_videoPlayer = new VideoPlayer();
    }
    
    return g_videoPlayer;
}

void VideoPlayer::playVideoByFilename(const char *fileName, bool showControl, VideoPlayProtocol *delegate)
{
    JniMethodInfo methodInfo;
    
    if (JniHelper::getStaticMethodInfo(methodInfo, 
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "playVideoJNI",
                                       "(Ljava/lang/String;Z)V")
        )
    {
        // file name
        jstring jstrFileName = methodInfo.env->NewStringUTF(fileName);
        
        // invoke java method
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrFileName, showControl);
        
        // release reference
        methodInfo.env->DeleteLocalRef(jstrFileName);
    }
}

void VideoPlayer::stopPlay()
{

}

bool VideoPlayer::touch()
{
    return false;
}

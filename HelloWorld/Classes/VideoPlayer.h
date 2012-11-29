#ifndef interapptive_VideoPlayer_h
#define interapptive_VideoPlayer_h

#include "cocos2d.h"
#include <string>

class VideoPlayProtocol
{
public:
    virtual void moviePlayBackDidFinish(const char *videoName) {};
};

/* Can only play one video at a time
 */
class VideoPlayer
{
public:
    static VideoPlayer* sharedVideoPlayer();
    void playVideoByFilename(const char* fileName, bool showControl, VideoPlayProtocol *delegate = NULL);
    void stopPlay();
    bool touch();
    
    bool showControl;
    bool isVideoPlaying;
    
    VideoPlayProtocol *delegate;
    std::string fileName;
};

#endif

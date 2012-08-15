#ifndef interapptive_VideoPlayer_h
#define interapptive_VideoPlayer_h

#include "cocos2d.h"

/* Can only play one video at a time
 */
class VideoPlayer
{
public:
    static VideoPlayer* sharedVideoPlayer();
    void playVideoByFilename(const char* fileName, bool showControl);
    void stopPlay();
    bool touch();
    
    bool showControl;
    bool isVideoPlaying;
};

#endif

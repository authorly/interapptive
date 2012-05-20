#ifndef interapptive_VideoPlayer_h
#define interapptive_VideoPlayer_h

class VideoPlayerProtocol
{
public:
    virtual void moviePlayBackDidFinish() = 0;
};

/* Can only play one video at a time
 */
class VideoPlayer
{
public:
    static VideoPlayer* sharedVideoPlayer();
    void playVideoByFilename(VideoPlayerProtocol *delegate, const char* fileName, bool showControl);
    void stopPlay();

private:
    VideoPlayer();
};

#endif

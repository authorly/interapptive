#ifndef interapptive_VideoPlayer_h
#define interapptive_VideoPlayer_h

#include "cocos2d.h"

/* Can only play one video at a time
 */
class VideoPlayer : public cocos2d::CCLayer
{
public:
    static VideoPlayer* sharedVideoPlayer();
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void registerWithTouchDispatcher(void);
    void playVideoByFilename(cocos2d::CCNode *parent, const char* fileName, bool showControl);
    void stopPlay();
    
private:
    bool showControl;
};

#endif

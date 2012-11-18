#import "VideoPlayer.h"
#import "EAGLView.h"

#import "cocos2d.h"

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>

static id g_intervalVideoPlayer = nil;
static MPMoviePlayerController *g_moviePlayer = nil;
static VideoPlayer* g_sharedVideoPlayer = NULL;

// internal oc class to receive call back message MPMoviePlayerController
// when it is finished
@interface InternalVideoPlayer : NSObject
{
}

-(void) moviePlayBackDidFinish:(NSNotification*)notification;
@end

@implementation InternalVideoPlayer


-(void) moviePlayBackDidFinish:(NSNotification*)notification
{
    MPMoviePlayerController *moviePlayer = [notification object]; 
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:MPMoviePlayerPlaybackDidFinishNotification
                                                  object:moviePlayer];
    
    if ([moviePlayer respondsToSelector:@selector(setFullscreen:animated:)])
    {
        // remove observer for message MPMoviePlayerDidExitFullscreenNotification
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:MPMoviePlayerDidExitFullscreenNotification
                                                      object:moviePlayer];
        
        moviePlayer.fullscreen = false;
        //[moviePlayer.view removeFromSuperview];
    }
    else 
    {
        
        [moviePlayer release];
    }
    
    [g_moviePlayer release];
    g_moviePlayer = nil;
    
    // release self
    [g_intervalVideoPlayer release];
    g_intervalVideoPlayer = nil;
    
    g_sharedVideoPlayer->isVideoPlaying = false;
    
    if (VideoPlayer::sharedVideoPlayer()->delegate)
    {
        VideoPlayer::sharedVideoPlayer()->delegate->swipeLeft();
        VideoPlayer::sharedVideoPlayer()->delegate = NULL;
    }
}

@end

using namespace cocos2d;


VideoPlayer* VideoPlayer::sharedVideoPlayer()
{
    if (! g_sharedVideoPlayer)
    {
        g_sharedVideoPlayer = new VideoPlayer();
    }
    
    return g_sharedVideoPlayer;
}

void VideoPlayer::playVideoByFilename(const char *fileName, bool showControl, PageLayer *delegate)
{
    this->showControl = showControl;
    this->isVideoPlaying = true;
    this->delegate = delegate;
    
    NSString *nsFilePath = [NSString stringWithUTF8String:fileName];
    NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:nsFilePath ofType:@""]];
    g_moviePlayer = [[MPMoviePlayerController alloc] initWithContentURL:url];
    
    // register internal oc class to receive call back message
    g_intervalVideoPlayer = [InternalVideoPlayer alloc];
    
    [[NSNotificationCenter defaultCenter] addObserver:g_intervalVideoPlayer
                                             selector:@selector(moviePlayBackDidFinish:)
                                                 name:MPMoviePlayerPlaybackDidFinishNotification
                                               object:g_moviePlayer];
    
    if ([g_moviePlayer respondsToSelector:@selector(setFullscreen:animated:)])
    {
        // use 3.2 style API
        if (showControl)
        {
            g_moviePlayer.controlStyle = MPMovieControlStyleEmbedded;
            
            // listen the message that user click done button
            [[NSNotificationCenter defaultCenter] addObserver:g_intervalVideoPlayer
                                                     selector:@selector(moviePlayBackDidFinish:)
                                                         name:MPMoviePlayerDidExitFullscreenNotification
                                                       object:g_moviePlayer];
        }
        else 
        {
            g_moviePlayer.controlStyle = MPMovieControlStyleNone;
        }
        

        g_moviePlayer.shouldAutoplay = YES;
        [[EAGLView sharedEGLView] addSubview:g_moviePlayer.view];
        [g_moviePlayer setFullscreen:YES animated:YES];
        
        [g_moviePlayer play];
    }
    else 
    {
        // use 2.0 style API
        if (showControl)
        {
            g_moviePlayer.movieControlMode = MPMovieControlModeDefault;
        }
        else 
        {
            g_moviePlayer.movieControlMode = MPMovieControlModeHidden;
        }
        
        [g_moviePlayer play];
    }
}

bool VideoPlayer::touch()
{
    
    if (isVideoPlaying)
    {
        // playing video and not show control, should stop playing
        if (!showControl)
        {
            stopPlay();
            isVideoPlaying = false;
            showControl = false;
        }
        
        // playing video and show control, should absorb the touch message
        // can only stop playing video by touch "Done"
        return true;
    }
    
    return false;
}

void VideoPlayer::stopPlay()
{
    isVideoPlaying = false;
    
    if (g_intervalVideoPlayer)
    {
        [g_moviePlayer stop];
        
        [[NSNotificationCenter defaultCenter] removeObserver:g_intervalVideoPlayer
                                                        name:MPMoviePlayerPlaybackDidFinishNotification
         
                                                      object:g_moviePlayer];
        // release internal oc object
        [g_intervalVideoPlayer release];
        g_intervalVideoPlayer = nil;
        
        // release movie player
        if ([g_moviePlayer respondsToSelector:@selector(setFullscreen:animated:)])
        {
            [[NSNotificationCenter defaultCenter] removeObserver:g_intervalVideoPlayer
                                                            name:MPMoviePlayerDidExitFullscreenNotification
                                                          object:g_moviePlayer];
            
            [g_moviePlayer.view removeFromSuperview];
        }
        else 
        {
            [g_moviePlayer release];
        }
        
        g_moviePlayer = nil;
    }
    
    if (VideoPlayer::sharedVideoPlayer()->delegate)
    {
        VideoPlayer::sharedVideoPlayer()->delegate->swipeLeft();
        VideoPlayer::sharedVideoPlayer()->delegate = NULL;
    }
}

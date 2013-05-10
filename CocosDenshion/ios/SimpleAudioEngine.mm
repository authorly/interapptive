/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "SimpleAudioEngine.h"
#include "SimpleAudioEngine_objc.h"

static void static_end()
{
    [SimpleAudioEngine  end];
}

static void static_preloadBackgroundMusic(const char* pszFilePath)
{
    [[SimpleAudioEngine sharedEngine] preloadBackgroundMusic: [NSString stringWithUTF8String: pszFilePath]];
}

static void static_playBackgroundMusic(const char* pszFilePath, bool bLoop)
{
    [[SimpleAudioEngine sharedEngine] playBackgroundMusic: [NSString stringWithUTF8String: pszFilePath] loop: bLoop];
}

static void static_stopBackgroundMusic()
{
    [[SimpleAudioEngine sharedEngine] stopBackgroundMusic];
}

static void static_pauseBackgroundMusic()
{
     [[SimpleAudioEngine sharedEngine] pauseBackgroundMusic];
}

static void static_resumeBackgroundMusic()
{
    [[SimpleAudioEngine sharedEngine] resumeBackgroundMusic];
} 

static void static_rewindBackgroundMusic()
{
    [[SimpleAudioEngine sharedEngine] rewindBackgroundMusic];
}

static bool static_willPlayBackgroundMusic()
{
    return [[SimpleAudioEngine sharedEngine] willPlayBackgroundMusic];
}

static bool static_isBackgroundMusicPlaying()
{
    return [[SimpleAudioEngine sharedEngine] isBackgroundMusicPlaying];
}

static float static_getBackgroundMusicVolume()
{
    return [[SimpleAudioEngine sharedEngine] backgroundMusicVolume];
}

static void static_setBackgroundMusicVolume(float volume)
{
    volume = MAX( MIN(volume, 1.0), 0 );
    [SimpleAudioEngine sharedEngine].backgroundMusicVolume = volume;
}
     
static float static_getEffectsVolume()
{
    return [[SimpleAudioEngine sharedEngine] effectsVolume];
}
     
static void static_setEffectsVolume(float volume)
{
    volume = MAX( MIN(volume, 1.0), 0 );
    [SimpleAudioEngine sharedEngine].effectsVolume = volume;
}
     
static unsigned int static_playEffect(const char* pszFilePath, bool bLoop)
{
    return [[SimpleAudioEngine sharedEngine] playEffect:[NSString stringWithUTF8String: pszFilePath] loop:bLoop]; 
}
     
static void static_stopEffect(int nSoundId)
{
    [[SimpleAudioEngine sharedEngine] stopEffect: nSoundId];
}
     
static void static_preloadEffect(const char* pszFilePath)
{
    [[SimpleAudioEngine sharedEngine] preloadEffect: [NSString stringWithUTF8String: pszFilePath]];
}
     
static void static_unloadEffect(const char* pszFilePath)
{
    [[SimpleAudioEngine sharedEngine] unloadEffect: [NSString stringWithUTF8String: pszFilePath]];
}

static void static_pauseEffect(unsigned int uSoundId)
{
    [[SimpleAudioEngine sharedEngine] pauseEffect: uSoundId];
}

static void static_pauseAllEffects()
{
    [[SimpleAudioEngine sharedEngine] pauseAllEffects];
}

static void static_resumeEffect(unsigned int uSoundId)
{
    [[SimpleAudioEngine sharedEngine] resumeEffect: uSoundId];
}

static void static_resumeAllEffects()
{
    [[SimpleAudioEngine sharedEngine] resumeAllEffects];
}

static void static_stopAllEffects()
{
    [[SimpleAudioEngine sharedEngine] stopAllEffects];
}

namespace CocosDenshion
{
	static SimpleAudioEngine *s_pEngine;

	SimpleAudioEngine::SimpleAudioEngine()
	{

	}

	SimpleAudioEngine::~SimpleAudioEngine()
	{

	}

	SimpleAudioEngine* SimpleAudioEngine::sharedEngine()
	{
		if (! s_pEngine)
		{
			s_pEngine = new SimpleAudioEngine();
		}
        
		return s_pEngine;
	}

	void SimpleAudioEngine::end()
	{
		if (s_pEngine)
		{
			delete s_pEngine;
			s_pEngine = NULL;
		}
        
                static_end();
	}

	void SimpleAudioEngine::setResource(const char* pszZipFileName)
	{

	}
    
    void SimpleAudioEngine::preloadBackgroundMusic(const char* pszFilePath)
	{
#ifdef DISABLE_AUDIO
        return;
#else
		static_preloadBackgroundMusic(pszFilePath);
#endif
	}

	void SimpleAudioEngine::playBackgroundMusic(const char* pszFilePath, bool bLoop, bool playeffect)
	{
#ifdef DISABLE_AUDIO
        return;
#else
		static_playBackgroundMusic(pszFilePath, bLoop);
#endif
	}

	void SimpleAudioEngine::stopBackgroundMusic(bool bReleaseData, bool playeffect)
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_stopBackgroundMusic();
#endif
        
	}

	void SimpleAudioEngine::pauseBackgroundMusic()
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_pauseBackgroundMusic();
#endif
	}

	void SimpleAudioEngine::resumeBackgroundMusic()
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_resumeBackgroundMusic();
#endif
	} 

	void SimpleAudioEngine::rewindBackgroundMusic()
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_rewindBackgroundMusic();
#endif
	}

	bool SimpleAudioEngine::willPlayBackgroundMusic()
	{
#ifdef DISABLE_AUDIO
        return false;
#else
        return static_willPlayBackgroundMusic();
#endif
	}

	bool SimpleAudioEngine::isBackgroundMusicPlaying()
	{
#ifdef DISABLE_AUDIO
        return false;
#else
        return static_isBackgroundMusicPlaying();
#endif
	}

	float SimpleAudioEngine::getBackgroundMusicVolume()
	{
#ifdef DISABLE_AUDIO
        return 0;
#else
        return static_getBackgroundMusicVolume();
#endif
	}

	void SimpleAudioEngine::setBackgroundMusicVolume(float volume)
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_setBackgroundMusicVolume(volume);
#endif
	}

	float SimpleAudioEngine::getEffectsVolume()
	{
#ifdef DISABLE_AUDIO
        return 0;
#else
        return static_getEffectsVolume();
#endif
	}

	void SimpleAudioEngine::setEffectsVolume(float volume)
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_setEffectsVolume(volume);
#endif
	}

	unsigned int SimpleAudioEngine::playEffect(const char* pszFilePath, bool bLoop)
	{
#ifdef DISABLE_AUDIO
        return 0;
#else
        return static_playEffect(pszFilePath, bLoop);
#endif
	}

	void SimpleAudioEngine::stopEffect(unsigned int nSoundId)
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_stopEffect(nSoundId);
#endif
	}

	void SimpleAudioEngine::preloadEffect(const char* pszFilePath)
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_preloadEffect(pszFilePath);
#endif
	}

	void SimpleAudioEngine::unloadEffect(const char* pszFilePath)
	{
#ifdef DISABLE_AUDIO
        return;
#else
        static_unloadEffect(pszFilePath);
#endif
	}
	
	void SimpleAudioEngine::pauseEffect(unsigned int uSoundId)
	{
#ifdef DISABLE_AUDIO
        return;
#else
	    static_pauseEffect(uSoundId);
#endif
	}
	
	void SimpleAudioEngine::resumeEffect(unsigned int uSoundId)
	{
#ifdef DISABLE_AUDIO
        return;
#else
	    static_resumeEffect(uSoundId);
#endif
	}
	
	void SimpleAudioEngine::pauseAllEffects()
	{
#ifdef DISABLE_AUDIO
        return;
#else
	    static_pauseAllEffects();
#endif
	}
	
	void SimpleAudioEngine::resumeAllEffects()
	{
#ifdef DISABLE_AUDIO
        return;
#else
	    static_resumeAllEffects();
#endif
	}
	
	void SimpleAudioEngine::stopAllEffects()
	{
#ifdef DISABLE_AUDIO
        return;
#else
	    static_stopAllEffects();
#endif
	}
}
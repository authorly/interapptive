#ifndef interapptive_Configurations_h
#define interapptive_Configurations_h

#include "cocos2d.h"

#include <string>

class Configurations
{
public:
    static std::string forwardEffect;
    static std::string backwardEffect;
    
    static float pageFlipTransitionDuration;
    static float paragraphTextFadeDuration;
    
    static std::string homeButtonNormalStateImage;
    static std::string homeButtonTappedStateImage;
    
    static cocos2d::CCPoint homeButtonPosition;
    static double homeButtonScale;
    
    /** 
     * Whether or not text scaling ratio is different from picture
     * true: text scaling ratio is the same as picture
     * false: text scaling ratio is different from picture
     */
    static bool retainTextScalingRatio;
    
    static bool skipAnimationOnSwipe;
};

#endif

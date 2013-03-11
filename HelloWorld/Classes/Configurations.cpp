#include "Configurations.h"

using namespace std;
using namespace cocos2d;

string Configurations::forwardEffect = "";
string Configurations::backwardEffect = "";

float Configurations::pageFlipTransitionDuration = 0.0f;
float Configurations::paragraphTextFadeDuration = 0.0f;

string Configurations::homeButtonNormalStateImage = "";
string Configurations::homeButtonTappedStateImage = "";

CCPoint Configurations::homeButtonPosition = CCPointZero;

bool Configurations::retainTextScalingRatio = false;
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
double Configurations::homeButtonScale = 1.0;
bool Configurations::retainTextScalingRatio = false;

bool Configurations::skipAnimationOnSwipe = false;
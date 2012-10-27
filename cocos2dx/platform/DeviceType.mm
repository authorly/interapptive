#include "DeviceType.h"

bool DeviceType::isIphone4S()
{
    NSString *deviceType = [UIDevice currentDevice].model;
    
    return [deviceType isEqualToString:@"iPhone4,1"];
}

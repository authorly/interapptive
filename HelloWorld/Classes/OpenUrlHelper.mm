#include "OpenUrlHelper.h"

#import <UIKit/UIKit.h>

void OpenUrlHelper::openUrl(const char *url)
{
    NSString *nsUrl = [NSString stringWithUTF8String:url];
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:nsUrl]];
}

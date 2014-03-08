//
//  Utils.cpp
//  authorly
//
//  Created by minggo on 3/8/14.
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "Utils.h"

int Utils::getDPI()
{
    float scale = 1;
    if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
        scale = [[UIScreen mainScreen] scale];
    }
    float dpi;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
        dpi = 132 * scale;
    } else if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
        dpi = 163 * scale;
    } else {
        dpi = 160 * scale;
    }
    
    return dpi;
}

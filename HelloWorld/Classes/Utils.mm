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
//    float scale = 1;
//    if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
//        scale = [[UIScreen mainScreen] scale];
//    }
//    float dpi;
//    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
//        dpi = 132 * scale;
//    } else if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
//        dpi = 163 * scale;
//    } else {
//        dpi = 160 * scale;
//    }
//    
//    return dpi;
    
    // Don't have to consider retina, becuase touch Point value is the same
    // no matter it is retina or not.
    
    float dpi = 0.f; // dpi of iPad normal
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
        dpi = 132;
    } else if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
        dpi = 163;
    } else {
        dpi = 160;
    }
    
    return dpi;
}

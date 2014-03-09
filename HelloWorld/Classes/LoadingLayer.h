//
//  LoadingScene.h
//  subscription
//
//  Created by minggo on 3/9/14.
//
//

#ifndef __subscription__LoadingScene__
#define __subscription__LoadingScene__

#include "cocos2d.h"

class LoadingLayer : public cocos2d::CCLayer
{
public:
    LoadingLayer();
    
    void buttonClicked(cocos2d::CCObject *sender);
    
    LAYER_NODE_FUNC(LoadingLayer);
};

#endif /* defined(__subscription__LoadingScene__) */

//
//  MyPageTurn.h
//  Copyright 2011 Headlight Software, Inc.
//  http://www.headlightinc.com/
//  by Michael Burford
// 
//  Adapted the Cocos Page Turn with stuff from here:
//  http://www.cocos2d-iphone.org/forum/topic/15523
//  Plus my own additions to that for backside coloring and corner selection
/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef interapptive_MyPageTurn_h
#define interapptive_MyPageTurn_h

#include "cocos2d.h"
#include "CCGrid.h"

enum {
	MyPageTurnCornerLowerRight = 0,
	MyPageTurnCornerLowerLeft,
	MyPageTurnCornerUpperRight,
	MyPageTurnCornerUpperLeft
};

class ccMyGrid3D : public cocos2d::CCGrid3D
{
private:
    float animationProgress;
public:
    static ccMyGrid3D* gridWithSize(const cocos2d::ccGridSize& gridSize, cocos2d::CCTexture2D *pTexture, bool bFlipped);
    static ccMyGrid3D* gridWithSize(const cocos2d::ccGridSize& gridSize);
    
    virtual void blit();
    void setProgress(float progress);
};

class ccMyGrid3dAction : public cocos2d::CCGrid3DAction
{
public:
    void setProgress(float progress);
    virtual cocos2d::CCGridBase* getGrid(void);
};

class ccMyPageTurn3d : public ccMyGrid3dAction
{
private:
    cocos2d::CCSize winSize;
    int turnCorner;
    
public:
    static ccMyPageTurn3d* actionWithSize(cocos2d::ccGridSize size, float duration, int fromCorner);
    bool initWithSize(cocos2d::ccGridSize size, float duration, int fromCorner);
    
    virtual void update(float time);
};

class ccMyTransitionPageTurn : public cocos2d::CCTransitionScene
{
private:
    int turnCorner;
    bool back;
    
public:
    static ccMyTransitionPageTurn* transitionWithDuration(float t, cocos2d::CCScene *s, int fromCorner, bool back);
    static ccMyTransitionPageTurn* transitionWithDuration(float t, cocos2d::CCScene *s, bool back);
    static ccMyTransitionPageTurn* transitionWithDuration(float t, cocos2d::CCScene *s);
    bool initWithDuration(float t, cocos2d::CCScene *s, int fromCorner, bool back);
    cocos2d::CCActionInterval* actionWithSize(cocos2d::ccGridSize vector);
    virtual void sceneOrder();
    virtual void onEnter();
};

#endif

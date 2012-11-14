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

#include "MyPageTurn.h"

using namespace cocos2d;

// ccMyGrid3D

ccMyGrid3D* ccMyGrid3D::gridWithSize(const ccGridSize& gridSize, CCTexture2D *pTexture, bool bFlipped)
{
    ccMyGrid3D *pRet= new ccMyGrid3D();
    
    if (pRet)
    {
        if (pRet->initWithSize(gridSize, pTexture, bFlipped))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = NULL;
        }
    }
    
    return pRet;
}

ccMyGrid3D* ccMyGrid3D::gridWithSize(const ccGridSize& gridSize)
{
    ccMyGrid3D *pRet= new ccMyGrid3D();
    
    if (pRet)
    {
        if (pRet->initWithSize(gridSize))
        {
            pRet->autorelease();
        }
        else
        {
            delete pRet;
            pRet = NULL;
        }
    }
    
    return pRet;
}
	

void ccMyGrid3D::blit()
{
    int n = m_sGridSize.x * m_sGridSize.y;
	
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Unneeded states: GL_COLOR_ARRAY
	//enable culling, the default cull face is back
	glEnable(GL_CULL_FACE);
	//now only the front facing polygons are drawn
	glDisableClientState(GL_COLOR_ARRAY);	
	
	glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, m_pTexCoordinates);
	glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, m_pIndices);
	
	//change the winding of what OpenGl considers front facing
	//only the back facing polygons will be drawn
	//this works better then changing the cull face
	glFrontFace(GL_CW);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor4ub(255-(animationProgress*96.0), 255-(animationProgress*96.0), 255-(animationProgress*96.0), 255);
	glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, m_pIndices);
	
	//restore GL default states
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

void ccMyGrid3D::setProgress(float progress)
{
    this->animationProgress = progress;
}

// ccMyGrid3dAction

CCGridBase* ccMyGrid3dAction::getGrid(void)
{
    return ccMyGrid3D::gridWithSize(m_sGridSize);
}

void ccMyGrid3dAction::setProgress(float progress)
{
    ccMyGrid3D *g = (ccMyGrid3D*)m_pTarget->getGrid();
    g->setProgress(progress);
}

// ccMyPageTurn3d

ccMyPageTurn3d* ccMyPageTurn3d::actionWithSize(cocos2d::ccGridSize size, float duration, int fromCorner)
{
    ccMyPageTurn3d *ret = new ccMyPageTurn3d();
    if (ret && ret->initWithSize(size, duration, fromCorner)) {
        ret->autorelease();
    }
    else {
        CC_SAFE_FREE(ret);
    }
 
    return  ret;
}

bool ccMyPageTurn3d::initWithSize(cocos2d::ccGridSize size, float duration, int fromCorner)
{
    ccMyGrid3dAction::initWithDuration(duration);
    
    this->turnCorner = fromCorner;
    this->m_sGridSize = size;
    this->winSize = CCDirector::sharedDirector()->getWinSize();
    
    return true;
}

void ccMyPageTurn3d::update(float time)
{
    //Copied pretty much intact from the PageTurn3D, added the progress and different corners.
	float tt = MAX( 0, time - 0.25f );
	float deltaAy = ( tt * tt * 500);
	float ay = -100 - deltaAy;
	
	float deltaTheta = - (float) M_PI_2 * sqrtf( time) ;
	float theta = /*0.01f*/ + (float) M_PI_2 +deltaTheta;
	
	float sinTheta = sinf(theta);
	float cosTheta = cosf(theta);
	
	//Setting progress adjusts the gray color of the background page
    this->setProgress(time);
	
	for( int i = 0; i <=m_sGridSize.x; i++ )
	{
		for( int j = 0; j <= m_sGridSize.y; j++ )
		{
			// Get original vertex
			ccVertex3F	p = this->originalVertex(ccg(i,j));
			
			//Yeah, some duplicate stuff in each of these that could be cleaned up....but not today.			
			if (turnCorner==MyPageTurnCornerLowerRight) {			
				float R = sqrtf(p.x*p.x + (p.y - ay) * (p.y - ay));
				float r = R * sinTheta;
				float alpha = asinf( p.x / R );
				float beta = alpha / sinTheta;
				float cosBeta = cosf( beta );
				
				// If beta > PI then we've wrapped around the cone
				// Reduce the radius to stop these points interfering with others
				if( beta <= M_PI)
					p.x = ( r * sinf(beta));
				else
				{
					// Force X = 0 to stop wrapped
					// points
					p.x = 0;
				}
				
				p.y = ( R + ay - ( r*(1 - cosBeta)*sinTheta));
				
				// We scale z here to avoid the animation being
				// too much bigger than the screen due to perspectve transform
				p.z = (r * ( 1 - cosBeta ) * cosTheta) / 7; // "100" didn't work for
				
			} else if (turnCorner==MyPageTurnCornerUpperRight) {			
				float	px = p.x;
				float	py = winSize.height - p.y;
				float R = sqrtf(px*px + (py - ay) * (py - ay));
				float r = R * sinTheta;
				float alpha = asinf( px / R );
				float beta = alpha / sinTheta;
				float cosBeta = cosf( beta );
				
				// If beta > PI then we've wrapped around the cone
				// Reduce the radius to stop these points interfering with others
				if( beta <= M_PI)
					p.x = ( r * sinf(beta));
				else
				{
					// Force X = 0 to stop wrapped
					// points
					p.x = 0;
				}
				
				p.y = winSize.height - ( R + ay - ( r*(1 - cosBeta)*sinTheta));
				
				// We scale z here to avoid the animation being
				// too much bigger than the screen due to perspectve transform
				p.z = (r * ( 1 - cosBeta ) * cosTheta) / 7; // "100" didn't work for
				
			} else if (turnCorner==MyPageTurnCornerUpperLeft) {			
				float	px = winSize.width - p.x;
				float	py = winSize.height - p.y;
				float R = sqrtf(px*px + (py - ay) * (py - ay));
				float r = R * sinTheta;
				float alpha = asinf( px / R );
				float beta = alpha / sinTheta;
				float cosBeta = cosf( beta );
				
				// If beta > PI then we've wrapped around the cone
				// Reduce the radius to stop these points interfering with others
				if( beta <= M_PI)
					p.x = winSize.width - ( r * sinf(beta));
				else
				{
					// Force X = 0 to stop wrapped
					// points
					p.x = winSize.width;
				}
				
				p.y = winSize.height - ( R + ay - ( r*(1 - cosBeta)*sinTheta));
				
				// We scale z here to avoid the animation being
				// too much bigger than the screen due to perspectve transform
				p.z = (r * ( 1 - cosBeta ) * cosTheta) / 7; // "100" didn't work for
                
			} else if (turnCorner==MyPageTurnCornerLowerLeft) {			
				float	px = winSize.width - p.x;
				float	py = p.y;
				float R = sqrtf(px*px + (py - ay) * (py - ay));
				float r = R * sinTheta;
				float alpha = asinf( px / R );
				float beta = alpha / sinTheta;
				float cosBeta = cosf( beta );
				
				// If beta > PI then we've wrapped around the cone
				// Reduce the radius to stop these points interfering with others
				if( beta <= M_PI)
					p.x = winSize.width - ( r * sinf(beta));
				else
				{
					// Force X = 0 to stop wrapped
					// points
					p.x = winSize.width;
				}
				
				p.y = ( R + ay - ( r*(1 - cosBeta)*sinTheta));
				
				// We scale z here to avoid the animation being
				// too much bigger than the screen due to perspectve transform
				p.z = (r * ( 1 - cosBeta ) * cosTheta) / 7; // "100" didn't work for
			}
			
			// Stop z coord from dropping beneath underlying page in a transition
			// issue #751				
			if (p.z<0.5f)
				p.z = 0.5f;
			
			// Set new coords
            this->setVertex(ccg(i,j), p);
		}
	}
}

// ccMyTransitionPageTurn

ccMyTransitionPageTurn* ccMyTransitionPageTurn::transitionWithDuration(float t, cocos2d::CCScene *s, int fromCorner, bool back)
{
    ccMyTransitionPageTurn* ret = new ccMyTransitionPageTurn();
    if (ret && ret->initWithDuration(t, s, fromCorner, back)) {
        ret->autorelease();
    }
    else {
        CC_SAFE_FREE(ret);
    }
    
    return ret;
}

ccMyTransitionPageTurn* ccMyTransitionPageTurn::transitionWithDuration(float t, cocos2d::CCScene *s, bool back)
{
    ccMyTransitionPageTurn* ret = new ccMyTransitionPageTurn();
    if (ret && ret->initWithDuration(t, s, MyPageTurnCornerLowerRight, back)) {
        ret->autorelease();
    }
    else {
        CC_SAFE_FREE(ret);
    }
    
    return ret;
}

ccMyTransitionPageTurn* ccMyTransitionPageTurn::transitionWithDuration(float t, cocos2d::CCScene *s)
{
    ccMyTransitionPageTurn* ret = new ccMyTransitionPageTurn();
    if (ret && ret->initWithDuration(t, s, MyPageTurnCornerLowerRight, false)) {
        ret->autorelease();
    }
    else {
        CC_SAFE_FREE(ret);
    }
    
    return ret;
}

bool ccMyTransitionPageTurn::initWithDuration(float t, cocos2d::CCScene *s, int fromCorner, bool back)
{
    // XXX: needed before [super init]
	this->back = back;
	
	if (CCTransitionScene::initWithDuration(t, s)) {
        this->turnCorner = fromCorner;
        return true;
    }
    
    return false;
}

void ccMyTransitionPageTurn::sceneOrder()
{
    m_bIsInSceneOnTop = back;
}

void ccMyTransitionPageTurn::addShadow()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    CCScene *scene = back ? m_pOutScene : m_pInScene;
    
    CCLayerGradient * shadow = CCLayerGradient::layerWithColor(ccc4(0,0,0,255), ccc4(0,0,0,0), ccp(1.f,0.f));
    
    bool rightToLeft;
    int topToBottom;
    float startAngle, destAngle, startX, destX, move_duration, startY;
    
    if (turnCorner == MyPageTurnCornerUpperRight || turnCorner == MyPageTurnCornerLowerRight) {
        
        rightToLeft = !back;
        
        topToBottom = (turnCorner == MyPageTurnCornerUpperRight) ? 1 : -1;
        
        startAngle    = rightToLeft ?   -45 * topToBottom : 0;
        destAngle     = rightToLeft ?   +45 * topToBottom : -45 * topToBottom; //rotate 45 degrees
        startX        = rightToLeft ?   0 :  -s.width;
        destX         = rightToLeft ?   -s.width :  s.width; //move the full width of screen
        move_duration = rightToLeft ?   1.3 : 1; // if forwards go slower - gives nice offset
        startY        = topToBottom == 1 ?  -.5 * s.height : 0;
    }
    
    else if (turnCorner == MyPageTurnCornerUpperLeft || turnCorner == MyPageTurnCornerLowerLeft) {
        
        rightToLeft = back;
        
        topToBottom = (turnCorner == MyPageTurnCornerUpperLeft) ? 1 : -1;
        
        startAngle    = rightToLeft ?   180 : 180 + 45 * topToBottom;
        destAngle     = rightToLeft ?   +45 * topToBottom : -45 * topToBottom; //rotate 45 degrees
        startX        = rightToLeft ?   s.width : 0;
        destX         = rightToLeft ?   -s.width :  s.width; //move the full width of screen
        move_duration = rightToLeft ?   1.3 : 1; // if forwards go slower - gives nice offset
        startY        = topToBottom ==  1 ?  -.5 * s.height :  0;
    }
    
    shadow->setRotation(startAngle);
    shadow->changeHeight(s.height * 1.5);
    shadow->setPosition(ccp(startX, startY));
    
    CCMoveBy *move = CCMoveBy::actionWithDuration(m_fDuration*move_duration, ccp(destX, 0));
    CCRotateBy *rotate = CCRotateBy::actionWithDuration(m_fDuration, destAngle);
    CCFiniteTimeAction *spawn = CCSpawn::actions(move, rotate, NULL);
    scene->addChild(shadow, 1);
    shadow->runAction(spawn);
}

void ccMyTransitionPageTurn::onEnter()
{
    CCTransitionScene::onEnter();
    // addShadow();
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
	int x, y;
	if (s.width>=1024 || s.height>=1024) {
		//iPad, do a smaller grid, looks a little smoother to my eyes.
		if (s.width > s.height)	{
			x = 32;
			y = 24;
		} else {
			x = 24;
			y = 32;
		}
	} else {
		if (s.width > s.height)	{
			x = 16;
			y = 12;
		} else {
			x = 12;
			y = 16;
		}
	}
	
    CCActionInterval* action = this->actionWithSize(ccg(x,y));
	
	if (! back)	{
        m_pOutScene->runAction(CCSequence::actions(action,
                                                   CCCallFunc::actionWithTarget(this, callfunc_selector(CCTransitionScene::finish)),
                                                   CCStopGrid::action(),
                                                   NULL));
	} else {
		// to prevent initial flicker
        m_pInScene->setIsVisible(false);
        m_pInScene->runAction(CCSequence::actions(CCShow::action(),
                                                  action,
                                                  CCCallFunc::actionWithTarget(this, callfunc_selector(CCTransitionScene::finish)),
                                                  CCStopGrid::action(),
                                                  NULL));
	}
}

CCActionInterval* ccMyTransitionPageTurn::actionWithSize(cocos2d::ccGridSize v)
{
    if (back)	{
		return CCReverseTime::actionWithAction(ccMyPageTurn3d::actionWithSize(v, m_fDuration, turnCorner));
	} else {
		return ccMyPageTurn3d::actionWithSize(v, m_fDuration, turnCorner);
	}
}

#include "cpSprite.h"
#include "GCpShapeCache.h"

using namespace cocos2d;

cpSprite* cpSprite::spriteWithFile(const char *pszFileName)
{
    cpSprite *pobSprite = new cpSprite();
    if (pobSprite && pobSprite->initWithFile(pszFileName))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
	return NULL;
}

cpSprite::cpSprite()
: body(NULL)
, isSelected(false)
{}

void cpSprite::setChipmunkInfo(cpSpace *space, cpBody *body)
{
    this->body = body;
    this->space = space;
}

void cpSprite::setPosition(const CCPoint& pos)
{
    CCSprite::setPosition(pos);
    
    if (body) {
        body->p.x = pos.x;
        body->p.y = pos.y;
    }
}

void cpSprite::internalSetPosition(const CCPoint &pos)
{
    if (cpSpacePointQueryFirst(space, cpVectMake(pos.x, pos.y), CP_ALL_LAYERS, CP_NO_GROUP) != NULL)
    {
        return;
    }
    
    setPosition(pos);
}

void cpSprite::onEnter()
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 0, true);
	CCSprite::onEnter();
}

bool cpSprite::selected()
{
    return isSelected;
}

bool cpSprite::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CCSize s = getTexture()->getContentSize();
	CCRect rect =  CCRectMake(-s.width / 2, -s.height / 2, s.width, s.height);
    
    isSelected = CCRect::CCRectContainsPoint(rect, convertTouchToNodeSpaceAR(touch));
	return isSelected;
}

void cpSprite::ccTouchMoved(CCTouch* touch, CCEvent* event)
{		
	CCPoint touchPoint = touch->locationInView(touch->view());
    touchPoint = CCDirector::sharedDirector()->convertToGL(touchPoint);
	
	internalSetPosition(CCPointMake(touchPoint.x, touchPoint.y));
}

void cpSprite::ccTouchEnded(CCTouch* touch, CCEvent* event)
{		
	isSelected = false;
}

void cpSprite::touchDelegateRetain()
{
	this->retain();
}

void cpSprite::touchDelegateRelease()
{
	this->release();
}

#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "cocos2d.h"

class Page;

class PageLayer : public cocos2d::CCLayer
{
public:
	static PageLayer* pageLayerWithPage(Page* page);
	void touchCallBack(float flag);
    virtual void onEnterTransitionDidFinish();
    virtual void registerWithTouchDispatcher(void);
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    //virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
private:
    PageLayer();
	// helper functions to initialize layer
	void createSprites();
	void createParagraph(int index);
	void playBackgroundMusic();
    bool isSwipeLeft(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos);
    bool isSwipeRight(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos);
    void showNextParagraph();
    void showPrevioisParagrph();
private:
    // record touch began position
    cocos2d::CCPoint beginPoint;
    int currentIndexOfParagraph;
    // the page that pageLayer use, weak ref
    Page *page;
};

#endif // __PAGE_LAYER_H__

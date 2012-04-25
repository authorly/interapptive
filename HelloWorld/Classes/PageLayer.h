#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "cocos2d.h"

#include <vector>

class Page;

class PageLayer : public cocos2d::CCLayer
{
public:
	static PageLayer* pageLayerWithPage(Page* page);
	void touchCallBack(float flag);
    virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
    virtual void registerWithTouchDispatcher(void);
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    //virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    void highlightParagraph();
    void highlightSchedule(cocos2d::ccTime dt);
    void setParagraphVisible();
private:
    PageLayer();
    //~PageLayer();
    void init(Page *page);
    
	// helper functions to initialize layer
	void createSprites();
	void createParagraph(int index);
	void playBackgroundMusic();
    
    bool isSwipeLeft(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos);
    bool isSwipeRight(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos);
    void swipeLeft();
    void swipeRight();
    /**
     * @swipteLeft if it is swipe left
     * @ret total delay time for showing paragraph
     */
    float swipeEndedOperationAndCalculateTotalDelay(bool swipeLeft);
    
    float calculateDelayTimeOnEnter();
    
    // helper functions of handling paragraph
    void showParagraph(float delay);
    
    void stopHightAndPlayEffect();
    
private:
    // record touch began position
    cocos2d::CCPoint beginPoint;
    int currentIndexOfParagraph;
    // the page that pageLayer use, weak ref
    Page *page;
    // record all labels
    std::vector<cocos2d::CCLabelTTF*> wordsOfParagraph;
    unsigned int highlightCallbackTimes;
    cocos2d::CCLayer *paragraphLayer;
};

#endif // __PAGE_LAYER_H__

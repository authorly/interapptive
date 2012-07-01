#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "MyDialog.h"
#include "VideoPlayer.h"
#include "cocos2d.h"

#include <vector>

class Page;

class PageLayer : public cocos2d::CCLayer, public DialogProtocol, public VideoPlayerProtocol
{
public:
    virtual ~PageLayer();
	static PageLayer* pageLayerWithPage(Page* page);
	void touchCallback(float flag);
    void mainMenuItemCallback(cocos2d::CCObject *sender);
    virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
    virtual void registerWithTouchDispatcher(void);
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    void highlightParagraph();
    void setParagraphVisible();
    void changeColor(cocos2d::CCObject *sender);
    void changeColorBack(cocos2d::CCObject *sender);
    
    // dialog protocol
    virtual void buttonClicked(int index);
    
    // video player protocol
    virtual void moviePlayBackDidFinish();
private:
    PageLayer();
    void init(Page *page);
    
	// helper functions to initialize layer
	void createSprites();
	void createParagraph(int index);
    void createMainMenuItem();
    void createPhysicsLayer();
	void playBackgroundMusic();    
    
    bool isSwipeLeft(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos);
    bool isSwipeRight(cocos2d::CCPoint &beginPos, cocos2d::CCPoint &endPos);
    void swipeLeft();
    void swipeRight();
    void swipEndCallBack();
    /**
     * @swipteLeft if it is swipe left
     * @ret total delay time for showing paragraph
     */
    float swipeEndedOperationAndCalculateTotalDelay(bool swipeLeft);
    
    float calculateDelayTimeOnEnter();
    
    // helper functions of handling paragraph
    void showParagraph(float delay);
    
    void stopHighlightEffect();
    
private:
    // record touch began position
    cocos2d::CCPoint beginPoint;
    int currentIndexOfParagraph;
    // the page that pageLayer use, weak ref
    Page *page;
    // record all labels
    std::vector<cocos2d::CCLabelTTF*> wordsOfParagraph;
    cocos2d::CCLayer *paragraphLayer;

    MyDialog *mydialog;
    bool isVideoPlaying;
    
    bool isSwiping;
};

#endif // __PAGE_LAYER_H__

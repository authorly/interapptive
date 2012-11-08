#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "MyDialog.h"
#include "cocos2d.h"
#include "Page.h"
#include "TouchDetection.h"

#include <vector>

typedef struct
{
    StoryTouchableNode* touchNode;
    cocos2d::CCParticleSystem *partileSystem;
} DelayForTextTouchNodeInfo;

class PageLayer : public cocos2d::CCLayer, public DialogProtocol
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
    void stopHighlightParagraph();
    void changeColor(cocos2d::CCObject *sender);
    void changeColorBack(cocos2d::CCObject *sender);
    
    void doSwipeLeftAfterDelay(cocos2d::CCObject *sender);
    
    void addTouchNode();
    void enableDelayForAnimationTouchNode(cocos2d::CCObject *sender);
    void enableDelayForTextTouchNode();
    
    // dialog protocol
    virtual void buttonClicked(int index);
    
    // may be called when video play finished
    void swipeLeft();
    
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
    void swipeRight();
    void swipEndCallBack();
    /**
     * @swipteLeft if it is swipe left
     * @ret total delay time for showing paragraph
     */
    float swipeEndedOperationAndCalculateTotalDelay(bool isSwipeLeft);
    
    float calculateDelayTimeOnEnter();
    
    // helper functions of handling paragraph
    void showParagraph(float delay);
    
    void stopHighlightEffect();
    
    // determine add or minus text space according the y coordinate 
    // of the first line text
    // @true add text space
    // @false minus text space
    bool addTextSpace(float yFirstLineText);
    
private:
    // record touch began position
    cocos2d::CCPoint beginPoint;
    int currentIndexOfParagraph;
    // the page that pageLayer use, weak ref
    Page *page;
    // record all labels
    std::vector<cocos2d::CCLabelTTF*> wordsOfParagraph;
    cocos2d::CCLayer *paragraphLayer;
    
    // some sprites may run action when onEnter
    // so should calcuate delay time
    unsigned int delayOfAnimation;
    
    std::vector<DelayForTextTouchNodeInfo> touchableNodeDelayForTextArray;
    std::vector<StoryTouchableNode*> touchableNodeDelayForAnimationArray;
    TouchDetection *touchDetector;

    MyDialog *mydialog;
    
    bool isSwiping;
    
    unsigned int touchSoundId;
};

#endif // __PAGE_LAYER_H__

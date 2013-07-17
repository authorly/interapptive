#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "MyDialog.h"
#include "cocos2d.h"
#include "Page.h"
#include "TouchDetection.h"
#include "VideoPlayer.h"

#include <vector>

class PageLayer : public cocos2d::CCLayerColor, public DialogProtocol, public VideoPlayProtocol
{
public:
    virtual ~PageLayer();
	static PageLayer* pageLayerWithPage(Page* page);
	void storyTouchCallback(float flag);
    void hotspotCallback(float flag);
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
    
    void addStoryTouchNode();
    void enableDelayForAnimationTouchNode(cocos2d::CCObject *sender);
    void enableDelayForTextTouchNode();
    
    void delaySwipeAfterPlayingVideo(cocos2d::CCObject *sender);
    
    // delay of animation call back
    void animationDelayCallback(cocos2d::CCObject *sender);
    
    // dialog protocol
    virtual void buttonClicked(int index);
    
    virtual void moviePlayBackDidFinish(const char *videoName, bool isParagraphHotspot);
    
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
    void swipeLeft();
    void swipEndCallBack();
    // can not swipe
    // 1. in swipping
    // 2. sprites run actions
    bool canSwipe();
    /**
     * @swipteLeft if it is swipe left
     * @ret total delay time for showing paragraph
     */
    float swipeEndedOperationAndCalculateTotalDelay(bool isSwipeLeft);
    
    float calculateDelayTimeOnEnter();
    
    // helper functions of handling paragraph
    void showParagraph(float delay);
    
    void stopHighlightEffect();
    
    HotspotInfo* getHotspotInfo(const std::string &videoName, bool isParagraphHotspot);
    
    // determine add or minus text space according the y coordinate 
    // of the first line text
    // @true add text space
    // @false minus text space
    bool addTextSpace(float yFirstLineText);
    
    void clearLabelIndex();
    LineText* getLineTextByLabel(cocos2d::CCLabelTTF*);
    
    // story touch node or paragraph hotspot call back
    void doHotspotTouched(HotspotInfo *hotspot, bool isParagraphHotspot);
    
    void addParagraphText(int index);
    void addParagraphHotspot(int index);
    
private:
    
    typedef struct {
        int paragraphIndex; // which paragraph a text belongs to
        int lineIndex;  // which text line a label belongs to
        cocos2d::CCLabelTTF *label;
    }LabelIndex;
    
    // record touch began position
    cocos2d::CCPoint beginPoint;
    int currentIndexOfParagraph;
    // the page that pageLayer use, weak ref
    Page *page;
    // record all labels
    std::vector<LabelIndex*> wordsOfParagraph;
    cocos2d::CCLayer *paragraphLayer;
    
    // some sprites may run action when onEnter
    // so should calcuate delay time
    unsigned int delayOfAnimation;

    // should use two dectectors, because the touch flags will be resued in story touch node and paragraph hotspot
    TouchDetection *storyTouchDetector;

    MyDialog *mydialog;
    
    bool isSwiping;
    bool isVideoPlaying;
    bool isHighLighting;
    bool isPlayingAnimation;
    
    unsigned int touchSoundId;
};

#endif // __PAGE_LAYER_H__

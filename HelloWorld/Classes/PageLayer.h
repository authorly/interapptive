#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "cocos2d.h"

class Page;

class PageLayer : public cocos2d::CCLayer
{
public:
	static PageLayer* pageLayerWithPage(Page* page);
	void touchCallBack(float flag);
private:
	// helper functions to initialize layer
	void createSprites(Page *page);
	void createParagraphs(Page *page);
	void playBackgroundMusic(Page *page);
};

#endif // __PAGE_LAYER_H__

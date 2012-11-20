#ifndef __PAGE_H__
#define __PAGE_H__

#include "cocos2d.h"
#include "SharedGlobalData.h"

#include <string>
#include <vector>
#include <map>

typedef struct settings
{
	// font info
	int                number;
    int                autoplayDelayBeforePageTurn;
	std::string        fontType;
	cocos2d::ccColor3B fontColor;
    cocos2d::ccColor3B fontHighlightColor;
	float              fontSize;

	// background music info
    bool        loop; // -1 means loop forever
	std::string audioFilePath;
    
    // falling physics object setting
    FallingObjectSetting fallingObjectSetting;
    // static physics object setting
    StaticObjectSetting staticObjectSetting;
} Setting;

// a line of a paragraph
typedef struct lineText
{
	std::string         text;
	int                 yOffset;
	int                 xOffset;
    std::vector<string> words;
} LineText;

typedef struct paragraph
{
	std::vector<float>       highlightingTimes;
	std::vector<LineText*>   linesOfTest;
    std::string              voiceAudioFile;
} Paragraph;

typedef struct sprite
{
	std::string               image;
	int                       spriteTag;
	cocos2d::CCPoint          position;
	std::vector<int>          actions; // actionTags
}SpriteInfo;

typedef struct storyTouchableNodeActionsToRun
{
	int spriteTag;
	int actionTag;
} StoryTouchableNodeActionsToRun;

typedef struct storyTouchableNode
{
	bool                      glitterIndicator;
    bool                      stopEffectIndicator;
    bool                      delayForText;
    bool                      delayForAnimation;
	cocos2d::CCPoint          position;
	int                       radius;
	std::string               videoToPlay;
	std::string               soundToPlay;
	int                       touchFlag;

	std::vector<StoryTouchableNodeActionsToRun*> actionsToRun;
} StoryTouchableNode;

typedef struct storySwipeEndedActionsToRun
{
	int                 runAfterSwipeNumber;
	int                 spriteTag;
    std::vector<int>    actionTags;
} StorySwipeEndedActionsToRun;

typedef struct storySwipeEnded
{
	std::vector<int> spritesToAdd;
	std::vector<int> spritesToMove;
	std::vector<StorySwipeEndedActionsToRun*> actionsToRun;
} StorySwipeEnded;

/**
 * This class is to store page informage
 * page = page + API (json data)
 * 
 * Declare all member vaviables as public, because it is more convenient, and we know what we do.
 */
class Page : public cocos2d::CCScene
{
public:
    ~Page();
    
	void addAction(int actionTag, cocos2d::CCAction *action);
	cocos2d::CCAction* getActionByTag(int actionTag);
    SpriteInfo* getSpriteInfoByTag(int spriteTag);
    StoryTouchableNode* getSotryTouchableNodeByFlag(int touchFlag);
    std::vector<StorySwipeEndedActionsToRun*>* getStorySwipeEndedActionToRun(int swipeNumber);
    void splitText(LineText *textLine);
public:
	// settings
	Setting settings;

	// text info: include some paragraphs
	std::vector<Paragraph*> paragraphs;

	// sprites in the scene
	std::vector<SpriteInfo*> sprites;

	// actions 
	// CCTimer doesn't have target and handler
	std::map<int, cocos2d::CCAction*> actions;

	// CCStoryTouchableNodes
	std::vector<StoryTouchableNode*> storyTouchableNodes;

	StorySwipeEnded storySwipeEnded;
};

#endif // __PAGE_H__

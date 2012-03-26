#ifndef __PAGE_H__
#define __PAGE_H__

#include "cocos2d.h"

#include <string>
#include <vector>
#include <map>

typedef struct settings
{
	// font info
	int                number;
	std::string        fontType;
	cocos2d::ccColor3B fontColor;
	int                fontSize;

	// background music info
    int         numberTimesToLoop; // -1 means loop forever
	std::string audioFilePath;
} Setting;

// a line of a paragraph
typedef struct lineText
{
	std::string text;
	int         yOffset;
	int         xOffset;
} LineText;

typedef struct paragraph
{
	std::vector<float>       highlightingTimes;
	std::vector<LineText*>   linesOfTest;
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
	cocos2d::CCPoint          position;
	int                       radius;
	std::string               videoToPlay;
	std::string               soundToPlay;

	std::vector<StoryTouchableNodeActionsToRun*> actionsToRun;
} StoryTouchableNode;

typedef struct storySwipeEndedActionsToRun
{
	int runAfterSwipeNumber;
	int spriteTag;
	int actionTag;
} StorySwipeEndedActionsToRun;

typedef struct storySwipeEnded
{
	std::vector<int> spritesToAdd;
	std::vector<int> spritesToMove;
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
	void addAction(int actionTag, cocos2d::CCAction *action);
	cocos2d::CCAction* getActionByTag(int actionTag);
public:
	// settings
	Setting settings;

	// text info: include some paragraphs
	std::vector<Paragraph*> paragraphs;

	// sprites in the scene
	std::vector<SpriteInfo*> sprites;

	// actions 
	std::map<int, cocos2d::CCAction*> actions;

	// CCStoryTouchableNodes
	std::vector<StoryTouchableNode*> storyTouchableNodes;

	StorySwipeEnded storySwipeEnded;
};

#endif // __PAGE_H__

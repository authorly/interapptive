#include "Page.h"

using namespace cocos2d;
using namespace std;

Page::~Page()
{
    // free paragraphs
    for (int i = 0; i < paragraphs.size(); ++i)
    {
        delete paragraphs[i];
    }
    
    // free sprite info
    for (int j = 0; j < sprites.size(); ++j)
    {
        delete sprites[j];
    }
    
    // free actions
    for (int k = 0; k < actions.size(); ++k)
    {
        actions[k]->release();
    }
    
    // free StoryTouchableNode
    for (int l = 0; l < storyTouchableNodes.size(); ++l)
    {
        delete storyTouchableNodes[l];
    }
    
    // free StorySwipeEnded
    for (int m = 0; m < storySwipeEnded.actionsToRun.size(); ++m)
    {
        delete storySwipeEnded.actionsToRun[m];
    }
}

void Page::splitText(LineText *textLine)
{
    string &text = textLine->text;
    
    // if text is null, return
    if (text.size() == 0)
    {
        return;
    }
    
    int startPos = 0;
    int endPos = text.find(' ', startPos);
    
    // if have only one word
    if (endPos == string::npos)
    {
        textLine->words.push_back(text);
        return;
    }
    
    // split text into words
    while (endPos != string::npos) {
        // include space
        textLine->words.push_back(text.substr(startPos, endPos - startPos + 1));
        startPos = endPos + 1;
        endPos = text.find_first_of(' ', startPos);
    }
    
    // add last word
    textLine->words.push_back(text.substr(startPos, text.size() - startPos));
}

void Page::addAction(int actionTag, CCAction *action)
{
	if (action)
	{
		actions[actionTag] = action;
		action->retain();
	}	
}

CCAction* Page::getActionByTag(int actionTag)
{
	return actions[actionTag];
}

SpriteInfo* Page::getSpriteInfoByTag(int spriteTag)
{
    vector<SpriteInfo*>::iterator iter;
    for (iter = sprites.begin(); iter != sprites.end(); ++iter)
    {
        if ((*iter)->spriteTag == spriteTag)
        {
            return *iter;
        }
    }
    
    return NULL;
}

vector<StorySwipeEndedActionsToRun*>* Page::getStorySwipeEndedActionToRun(int swipeNumber)
{
    vector<StorySwipeEndedActionsToRun*>* arrayActionsToRun = NULL;
    
    for (int i = 0; i < storySwipeEnded.actionsToRun.size(); ++i)
    {
        StorySwipeEndedActionsToRun* actionToRun = storySwipeEnded.actionsToRun[i];
        if (actionToRun->runAfterSwipeNumber == swipeNumber)
        {
            if (! arrayActionsToRun)
            {
                arrayActionsToRun = new vector<StorySwipeEndedActionsToRun*>();
            }
            
            arrayActionsToRun->push_back(actionToRun);
        }
    }
    
    return arrayActionsToRun;
}

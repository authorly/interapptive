#include "Page.h"

using namespace cocos2d;
using namespace std;

Page::~Page()
{
    // free paragraphs
    for (int i = 0; i < paragraphs.size(); ++i)
    {
        vector<LineText*> &linesOfTest = paragraphs[i]->linesOfTest;
        vector<LineText*>::iterator iter;
        for (iter = linesOfTest.begin(); iter != linesOfTest.end(); ++iter) 
        {
            delete *iter;
        }
        
        vector<HotspotInfo*> &hotspotInfo = paragraphs[i]->hotspots;
        vector<HotspotInfo*>::iterator iter2;
        for (iter2 = hotspotInfo.begin(); iter2 != hotspotInfo.end(); ++iter2)
        {
            delete *iter2;
        }
        
        delete paragraphs[i];
    }
    
    // free sprite info
    for (int j = 0; j < sprites.size(); ++j)
    {
        delete sprites[j];
    }
    
    // free actions
    map<int, cocos2d::CCAction*>::iterator actionIter;
    for (actionIter = actions.begin(); actionIter != actions.end(); ++actionIter)
    {
        actionIter->second->release();
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
    
    // free static object information
    vector<StaticObjectInfo*> &staticObjectInfos = settings.staticObjectSetting.staticObjects;
    vector<StaticObjectInfo*>::iterator iter;
    for (iter = staticObjectInfos.begin(); iter != staticObjectInfos.end(); ++iter) 
    {
        delete *iter;
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

StoryTouchableNode* Page::getSotryTouchableNode(int touchFlag)
{
    vector<StoryTouchableNode*>::iterator iter;
    for (iter = storyTouchableNodes.begin(); iter != storyTouchableNodes.end(); ++iter)
    {
        if ((*iter)->hotspotInfo.touchFlag == touchFlag)
        {
            return *iter;
        } 
    }
    
    return NULL;
}

StoryTouchableNode* Page::getStoryTouchableNode(const string &videoName)
{
    vector<StoryTouchableNode*>::iterator iter;
	for (iter = storyTouchableNodes.begin(); iter != storyTouchableNodes.end(); ++iter)
    {
        if ((*iter)->hotspotInfo.videoToPlay == videoName)
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

HotspotInfo* Page::getParagraphHotspotInfo(int paragraphIndex, int touchFlag)
{
    assert(paragraphIndex < paragraphs.size());
    
    Paragraph *paragraph = paragraphs[paragraphIndex];
    vector<HotspotInfo*>::iterator iter;
    for (iter = paragraph->hotspots.begin(); iter != paragraph->hotspots.end(); ++iter)
    {
        if ((*iter)->touchFlag == touchFlag)
        {
            return *iter;
        }
    }
    
    return NULL;
}

HotspotInfo* Page::getParagraphHotspotInfo(int paragraphIndex, const std::string &videoName)
{
    vector<HotspotInfo*> &hotspots = paragraphs[paragraphIndex]->hotspots;
    vector<HotspotInfo*>::iterator iter;
    for (iter = hotspots.begin(); iter != hotspots.end(); ++iter)
    {
        if ((*iter)->videoToPlay == videoName)
        {
            return *iter;
        }
    }
    
    return NULL;
}


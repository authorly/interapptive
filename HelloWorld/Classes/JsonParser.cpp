#include "JsonParser.h"
#include "PageManager.h"
#include "cocos2d.h"

#include <json/value.h>
#include <json/reader.h>
#include <string>

using namespace cocos2d;
using namespace std;

float JsonParser::xScale = 1.0f;
float JsonParser::yScale = 1.0f;
float JsonParser::scale = 1.0f;

bool JsonParser::parseJson(const char* pathOfJasonFile)
{
    // initialize scale factors
    xScale = CCDirector::sharedDirector()->getXScale();
    yScale = CCDirector::sharedDirector()->getYScale();
    scale = xScale > yScale ? xScale : yScale;
    
	// read json file
	std::string doc;
	unsigned long size;
    string fullPath = CCFileUtils::fullPathFromRelativePath(pathOfJasonFile);
	doc = (char*)CCFileUtils::getFileData(fullPath.c_str(), "r", &size);

	// read root
	Json::Reader reader;
	Json::Value root;
	if (! reader.parse(doc, root))
	{
		return false;
	}

	// parse elements
	// root is objectValue "Pages"
	//Json::Value pages = root.get("Pages", "UTF-8");
	Json::Value pages = root["Pages"];
	for(unsigned int index = 0; index < pages.size(); ++index)
	{
		// every element include page and API
		Page *page = new Page();
		
		Json::Value elementPage = pages[index]["Page"];		
		Json::Value elementAPI = pages[index]["API"];
		parseWithPage(page, elementPage);
		parseWithAPI(page, elementAPI);

		PageManager::pages[page->settings.number] = page;
	}

	return true;;
}

void JsonParser::parseWithPage(Page* page, Json::Value &jsonPage)
{
	// settings
	Json::Value settings = jsonPage["settings"];
	parseWithSettings(page, settings);
	// text
	Json::Value text = jsonPage["text"];
	parseWithText(page, text);
}

void JsonParser::parseWithSettings(Page* page, Json::Value &jsonSettings)
{
	Setting &settings = page->settings;

	// number
	settings.number = jsonSettings["number"].asInt();
	// fontType
	settings.fontType = jsonSettings["fontType"].asCString();
	// fontColor(r,g,b)
	Json::Value fontColor = jsonSettings["fontColor"];
	int r = 0, g = 1, b = 2;
	settings.fontColor.r = fontColor[r].asUInt();
	settings.fontColor.g = fontColor[g].asUInt();
	settings.fontColor.b = fontColor[b].asUInt();
	// font size
	settings.fontSize = jsonSettings["fontSize"].asInt() * scale;
	// background music file
	Json::Value backgroundMusicInfo = jsonSettings["backgroundMusicFile"];
    if (! backgroundMusicInfo.isNull())
    {
        settings.loop = backgroundMusicInfo["loop"].asBool();
        settings.audioFilePath = backgroundMusicInfo["audioFilePath"].asCString();
    }	
    else 
    {
        // no background music
        settings.audioFilePath = "";
    }
}

void JsonParser::parseWithText(Page* page, Json::Value &jsonText)
{
	vector<Paragraph*> &paragraphs = page->paragraphs;

	// paragraphs
	Json::Value jsonParagraphs = jsonText["paragraphs"];
	for (unsigned int i = 0; i < jsonParagraphs.size(); ++i)
	{
		Paragraph *paragraph = new Paragraph();
		Json::Value jsonParagraph = jsonParagraphs[i];

		// highlightingTimes
		Json::Value highlightingTimes = jsonParagraph["highlightingTimes"];
		for (unsigned int j = 0; j < highlightingTimes.size(); ++j)
		{
			paragraph->highlightingTimes.push_back((float)highlightingTimes[j].asDouble());
		}
		// linesOfText
		Json::Value linesOfText = jsonParagraph["linesOfText"];
		for (unsigned int k = 0; k < linesOfText.size(); ++k)
		{
			Json::Value jsonLineText = linesOfText[k];
			LineText *lineText = new LineText();
			lineText->text = jsonLineText["text"].asCString();
			lineText->xOffset = jsonLineText["xOffset"].asInt() * xScale;
			lineText->yOffset = jsonLineText["yOffset"].asInt() * yScale;

			paragraph->linesOfTest.push_back(lineText);
		}

		// add a new paragraph
		paragraphs.push_back(paragraph);	
	}

	
}

void JsonParser::parseWithAPI(Page* page, Json::Value &jsonAPI)
{
	// CCBezierBy
	Json::Value bezierBy = jsonAPI["CCBezierBy"];
	parseWithBezierByOrBezierTo(page, bezierBy, true);

	// CCBezierTo
	Json::Value bezierTo = jsonAPI["CCBezierTo"];
	parseWithBezierByOrBezierTo(page, bezierTo, false);

	// CCFadeIn
	Json::Value fadeIn = jsonAPI["CCFadeIn"];
	parseWithFadeInOrFadeOut(page, fadeIn, true);

	// CCFadeOut
	Json::Value fadeOut = jsonAPI["CCFadeOut"];
	parseWithFadeInOrFadeOut(page, fadeOut, false);

	// CCFadeTo
	Json::Value fadeTo = jsonAPI["CCFadeTo"];
	parseWithFadeTo(page, fadeTo);

	// CCFlipX
	Json::Value flipX = jsonAPI["CCFlipX"];
	parseWithFlipxOrFlipy(page, flipX, true);

	// CCFlipY
	Json::Value flipY = jsonAPI["CCFlipY"];
	parseWithFlipxOrFlipy(page, flipY, false);

	// CCFlipX3D
	Json::Value flipX3D = jsonAPI["CCFlipX3D"];
	parseWithFlipx3dOrFlipy3d(page, flipX3D, true);

	// CCFlipY3D
	Json::Value flipY3D = jsonAPI["CCFlipY3D"];
	parseWithFlipx3dOrFlipy3d(page, flipY3D, false);

	// CCJumpTo
	Json::Value jumpTo = jsonAPI["CCJumpTo"];
	parseWithJumpToOrJumpBy(page, jumpTo, true);

	// CCJumpBy
	Json::Value jumpBy = jsonAPI["CCJumpBy"];
	parseWithJumpToOrJumpBy(page, jumpBy, false);

	// CCRotateTo
	Json::Value rotateTo = jsonAPI["CCRotateTo"];
	parseWithRotateToOrRotateBy(page, rotateTo, true);

	// CCRotateBy
	Json::Value rotateBy = jsonAPI["CCRotateBy"];
	parseWithRotateToOrRotateBy(page, rotateBy, false);

	// CCSkewTo
	Json::Value skewTo = jsonAPI["CCSkewTo"];
	parseWithSkewToOrSkewBy(page, skewTo, true);

	// CCSkewBy
	Json::Value skewBy = jsonAPI["CCSkewBy"];
	parseWithSkewToOrSkewBy(page, skewBy, false);

	// CCMoveTo
	Json::Value moveTo = jsonAPI["CCMoveTo"];
	parseWithMoveToOrMoveBy(page, moveTo, true);

	// CCMoveBy
	Json::Value moveBy = jsonAPI["CCMoveBy"];
	parseWithMoveToOrMoveBy(page, moveBy, false);

	// CCScaleTo
	Json::Value scaleTo = jsonAPI["CCScaleTo"];
	parseWithScaleToOrScaleBy(page, scaleTo, true);

	// CCScaleBy
	Json::Value scaleBy = jsonAPI["CCScaleBy"];
	parseWithScaleToOrScaleBy(page, scaleBy, false);

	// CCDelayTime
	Json::Value delayTime = jsonAPI["CCDelayTime"];
	parseWithDelayTime(page, delayTime);

	// CCSequence
	// should parse CCSequence later, it uses other action
	Json::Value sequence = jsonAPI["CCSequence"];
	parseWithSequence(page, sequence);

	// CCSprites
	Json::Value sprites = jsonAPI["CCSprites"];
	parseWithSprites(page, sprites);

	// CCStoryTouchableNode
	Json::Value storyTouchableNode = jsonAPI["CCStoryTouchableNode"];
	parseWithStoryTouchableNode(page, storyTouchableNode);

	// CCStorySwipeEnded
	Json::Value storySwipeEnded = jsonAPI["CCStorySwipeEnded"];
	parseWithStorySwipeEnded(page, storySwipeEnded);

	// should parse CCEaseXXX later, because it will use other action

    // CCEaseBounceIn
	Json::Value easeBounceIn = jsonAPI["CCEaseBounceIn"];	
	parseWithEaseBounceInOrEaseBounceOut(page, easeBounceIn, true);

	// CCEaseBounceOut
	Json::Value easeBounceOut = jsonAPI["CCEaseBounceIn"];	
	parseWithEaseBounceInOrEaseBounceOut(page, easeBounceOut, false);

	// CCEaseIn
	Json::Value easeIn = jsonAPI["CCEaseIn"];
	parseWithEaseInOrEaseOut(page, easeIn, true);

	// CCEaseOut
	Json::Value easeOut = jsonAPI["CCEaseOut"];
	parseWithEaseInOrEaseOut(page, easeOut, false);
}

void JsonParser::parseWithBezierByOrBezierTo(Page *page, Json::Value &value, bool isBezierBy)
{
	if (! value.isNull())
	{	
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value jsonBezierByOrBezierTo = value[i];

			ccBezierConfig config;

		    // actionTag
		    int actionTag = jsonBezierByOrBezierTo["actionTag"].asInt();

		    // duration
		    float duration = (float)jsonBezierByOrBezierTo["duration"].asDouble();

		    // endPosition
		    Json::Value jsonEndPosition = jsonBezierByOrBezierTo["config"]["endPosition"];
		    int x = 0, y = 1;
		    config.endPosition.x = (float)jsonEndPosition[x].asInt() * xScale;
		    config.endPosition.y = (float)jsonEndPosition[y].asInt() * yScale;
       
		    // controlPosition1
		    Json::Value jsonControlPosition1 = jsonBezierByOrBezierTo["config"]["controlPosition_1"];
		    config.controlPoint_1.x = (float)jsonEndPosition[x].asInt() * xScale;
		    config.controlPoint_1.y = (float)jsonEndPosition[y].asInt() * yScale;

		    // controlPosition2
		    Json::Value jsonControlPosition2 = jsonBezierByOrBezierTo["config"]["controlPosition_2"];
		    config.controlPoint_2.x = (float)jsonEndPosition[x].asInt() * xScale;
		    config.controlPoint_2.y = (float)jsonEndPosition[y].asInt() * yScale;

		    // create action and add it into page
		    CCAction *action;
		    if (isBezierBy)
		    {
			    action = CCBezierBy::actionWithDuration(duration, config);
		    }
		    else
		    {
			    action = CCBezierTo::actionWithDuration(duration, config);
		    }
		    page->addAction(actionTag, action);
		}	
	}
}


void JsonParser::parseWithEaseBounceInOrEaseBounceOut(Page *page, Json::Value &value, bool isEaseBounceIn)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value easeBounceInOrOut = value[i];

            // actionTag
		    int actionTag = easeBounceInOrOut["actionTag"].asInt();
		    // interalActionTag
		    int internalActionTag = easeBounceInOrOut["internalActionTag"].asInt();

		    // get action that is parameter of CCEaseBounceIn/Out
		    CCAction *action = page->getActionByTag(internalActionTag);
		    assert(action != NULL);
		    assert(dynamic_cast<CCActionInterval*>(action) != NULL);

		    // create and add action to page
		    CCAction *actionToAdd;
		    if (isEaseBounceIn)
		    {
			    actionToAdd = CCEaseBounceIn::actionWithAction((CCActionInterval*)action);
		    }
		    else
		    {
			    actionToAdd = CCEaseBounceOut::actionWithAction((CCActionInterval*)action);
		    }
		    page->addAction(actionTag, actionToAdd);
		}
	}
}

void JsonParser::parseWithEaseInOrEaseOut(Page *page, Json::Value &value, bool isEaseIn)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value easeInOrOut = value[i];

			// actionTag
		    int actionTag = easeInOrOut["actionTag"].asInt();
		    // internalActionTag
		    int internalActionTag = easeInOrOut["internalActionTag"].asInt();
		    // rate
		    float rate = (float)easeInOrOut["rate"].asDouble();

		    // get action that is parameter of CCEaseBounceIn/Out
		    CCAction *action = page->getActionByTag(internalActionTag);
		    assert(action != NULL);
		    assert(dynamic_cast<CCActionInterval*>(action) != NULL);

		    // create and add action to page
		    CCAction *actionToAdd;
		    if (isEaseIn)
		    {
			    actionToAdd = CCEaseIn::actionWithAction((CCActionInterval*)action, rate);
		    }
		    else
		    {
			    actionToAdd = CCEaseOut::actionWithAction((CCActionInterval*)action, rate);
		    }
		    page->addAction(actionTag, actionToAdd);
		}
	}
}


void JsonParser::parseWithFadeInOrFadeOut(Page *page, Json::Value &value, bool isFadeIn)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value fadeInOrOut = value[i];

			// duration
		    float duration = (float)fadeInOrOut["duration"].asDouble();
		    // actionTag
		    int actionTag = fadeInOrOut["actionTag"].asInt();

		    CCAction *action;
		    if (isFadeIn)
		    {
			    action = CCFadeIn::actionWithDuration(duration);
		    }
		    else
		    {
			    action = CCFadeOut::actionWithDuration(duration);
		    }
		    page->addAction(actionTag, action);
		}
	}
}



void JsonParser::parseWithFlipxOrFlipy(Page *page, Json::Value &value, bool isFlipx)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value flipxOrFlipy = value[i];
 
            // actionTag
		    int actionTag = flipxOrFlipy["actionTag"].asInt();
		    // flip
		    bool flip = flipxOrFlipy["flip"].asBool();

		    CCAction *action;
		    if (isFlipx)
		    {
			    action = CCFlipX::actionWithFlipX(flip);
		    }
		    else
		    {
			    action = CCFlipY::actionWithFlipY(flip);
		    }
		    page->addAction(actionTag, action);
		}
	}
}


void JsonParser::parseWithFlipx3dOrFlipy3d(Page *page, Json::Value &value, bool isFlipx3d)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value flipx3dOrFlipy3d = value[i];

            // duration
		    float duration = (float)flipx3dOrFlipy3d["duration"].asDouble();
		    // actionTag
		    int actionTag = flipx3dOrFlipy3d["actionTag"].asInt();

		    CCAction *action;
		    if (isFlipx3d)
		    {
			    action = CCFlipX3D::actionWithDuration(duration);
		    }
		    else
		    {
			    action = CCFlipY3D::actionWithDuration(duration);
		    }
		    page->addAction(actionTag, action);
		}
	}
}

void JsonParser::parseWithFadeTo(Page *page, Json::Value &value)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value fadeTo = value[i];

            // opacity
		    int opacity = fadeTo["opacity"].asInt();
		    // duration
		    float duration = (float)fadeTo["duration"].asDouble();
		    // actionTag
		    int actionTag = fadeTo["actionTag"].asInt();

		    CCFadeTo *fadeToAction = CCFadeTo::actionWithDuration(duration, (unsigned char)opacity);
		    page->addAction(actionTag, fadeToAction);
		}
	}
}


void JsonParser::parseWithJumpToOrJumpBy(Page *page, Json::Value &value, bool isJumpTo)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value jumpToOrJumpBy = value[i];

			// position
			int x = 0, y = 1;
            CCPoint position;
			position.x = (float)jumpToOrJumpBy["position"][x].asInt() * xScale;
			position.y = (float)jumpToOrJumpBy["position"][y].asInt() * yScale;

			// duration
			float duration = (float)jumpToOrJumpBy["duration"].asDouble();
			// actionTag
			int actionTag = jumpToOrJumpBy["actionTag"].asInt();
			// height
			float height = (float)jumpToOrJumpBy["height"].asDouble() * yScale;
			// jumps
			int jumps = jumpToOrJumpBy["jumps"].asInt();

			CCAction *action;
			if (isJumpTo)
			{
				action = CCJumpTo::actionWithDuration(duration, position, height, jumps);
			}
			else
			{
				action = CCJumpBy::actionWithDuration(duration, position, height, jumps);
			}
			page->addAction(actionTag, action);
		}
	}
}

void JsonParser::parseWithRotateToOrRotateBy(Page *page, Json::Value &value, bool isRotateTo)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value rotateToOrBy = value[i];

			// degrees
			int degrees = rotateToOrBy["degrees"].asInt();
			// duration
			float duration = (float)rotateToOrBy["duration"].asDouble();
			// actionTag
			int actionTag = rotateToOrBy["actionTag"].asInt();

			CCAction *action;
			if (isRotateTo)
			{
				action = CCRotateTo::actionWithDuration(duration, degrees);
			}
			else
			{
				action = CCRotateBy::actionWithDuration(duration, degrees);
			}
			page->addAction(actionTag, action);
		}
	}
}

void JsonParser::parseWithSkewToOrSkewBy(Page *page, Json::Value &value, bool isSkewTo)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value skewToOrBy = value[i];

			// duration
			float duration = (float)skewToOrBy["duration"].asDouble();
			// deltaSkewX
			int deltaSkewX = skewToOrBy["deltaSkewX"].asInt();
			// deltaSkewY
			int deltaSkewY = skewToOrBy["deltaSkewY"].asInt();
			// actionTag
			int actionTag = skewToOrBy["actionTag"].asInt();

			CCAction *action;
			if (isSkewTo)
			{
				action = CCSkewTo::actionWithDuration(duration, deltaSkewX, deltaSkewY);
			}
			else
			{
				action = CCSkewBy::actionWithDuration(duration, deltaSkewX, deltaSkewY);
			}
			page->addAction(actionTag, action);
		}
	}
}

void JsonParser::parseWithMoveToOrMoveBy(Page *page, Json::Value &value, bool isMoveTo)
{
	if (! value.isNull())
	{        
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value moveToOrBy = value[i];

			// position, adjust it with scale factor
			int x = 0, y = 1;
			CCPoint position;
			position.x = moveToOrBy["position"][x].asInt() * xScale;
			position.y = moveToOrBy["position"][y].asInt() * yScale;
			// duration
			float duration = (float)moveToOrBy["duration"].asDouble();
			// actionTag
			int actionTag = moveToOrBy["actionTag"].asInt();

			CCAction *action;
			if (isMoveTo)
			{
				action = CCMoveTo::actionWithDuration(duration, position);
			}
			else
			{
				action = CCMoveBy::actionWithDuration(duration, position);
			}
			page->addAction(actionTag, action);
		}
	}
}

void JsonParser::parseWithScaleToOrScaleBy(Page *page, Json::Value &value, bool isScaleTo)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value scaleToOrBy = value[i];

			// intensity
			float intensity = (float)scaleToOrBy["intensity"].asDouble();
			// duration
			float duration = (float)scaleToOrBy["duration"].asDouble();
			// actionTag
			int actionTag = scaleToOrBy["actionTag"].asInt();

            CCAction *action;
			if (isScaleTo)
			{
				action = CCScaleTo::actionWithDuration(duration, intensity);
			}
			else
			{
				action = CCScaleBy::actionWithDuration(duration, intensity);
			}
			page->addAction(actionTag, action);
		}
	}
}

void JsonParser::parseWithSequence(Page *page, Json::Value &value)
{
	if (! value.isNull())
	{
		// actionTag
		int actionTag = value["actionTag"].asInt();
		// actions
		Json::Value actions = value["actions"];
		CCArray *arr = CCArray::array();
		for (unsigned int i = 0; i < actions.size(); ++i)
		{
			int actionToAddTag = actions[i]["actionTag"].asInt();
			CCAction *actionToAdd = page->getActionByTag(actionToAddTag);
			assert(actionToAdd != NULL);
			assert(dynamic_cast<CCFiniteTimeAction*>(actionToAdd) != NULL);
			arr->addObject((CCFiniteTimeAction*)actionToAdd);
		}

		CCAction *sequence = CCSequence::actionsWithArray(arr);
		page->addAction(actionTag, sequence);
	}
}

void JsonParser::parseWithSprites(Page *page, Json::Value &value)
{
	if (! value.isNull())
	{
	    for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value sprite = value[i];
			SpriteInfo *spriteInfo = new SpriteInfo();

			// image
			spriteInfo->image = sprite["image"].asCString();
			// spriteTag
			spriteInfo->spriteTag = sprite["spriteTag"].asInt();
            // position
			int x = 0, y = 1;
			spriteInfo->position.x = sprite["position"][x].asInt() * xScale;
			spriteInfo->position.y = sprite["position"][y].asInt() * yScale;
			// actions
			Json::Value actions = sprite["actions"];
			for (unsigned int j = 0; j < actions.size(); ++j)
			{
				spriteInfo->actions.push_back(actions[j].asInt());
			}
			
			page->sprites.push_back(spriteInfo);
		}
	}
}

void JsonParser::parseWithStoryTouchableNode(Page *page, Json::Value &value)
{
	if (! value.isNull())
	{
		Json::Value nodes = value["nodes"];
		for (unsigned int i = 0; i < nodes.size(); ++i)
		{
			Json::Value node = nodes[i];
			StoryTouchableNode * storyTouchableNode = new StoryTouchableNode();

			// glitterIndicator
			storyTouchableNode->glitterIndicator = node["glitterIndicator"].asBool();
			// position
			int x = 0, y = 1;
			storyTouchableNode->position.x = node["position"][x].asInt() * xScale;
			storyTouchableNode->position.y = node["position"][y].asInt() * yScale;
			// radius
			storyTouchableNode->radius = node["radius"].asInt() * scale;
			// videoToPlay
			storyTouchableNode->videoToPlay = node["videoToPlay"].asCString();
			// soundToPlay
			storyTouchableNode->soundToPlay = node["soundToPlay"].asCString();
			// touchFlag
			storyTouchableNode->touchFlag = node["touchFlag"].asInt();
			// runAction
			Json::Value runAction = node["runAction"];
			
			for (unsigned int j = 0; j < runAction.size(); ++j)
			{
				StoryTouchableNodeActionsToRun *storyTouchableNodeActionsToRun = new StoryTouchableNodeActionsToRun();
				storyTouchableNodeActionsToRun->actionTag = runAction[j]["actionTag"].asInt();
				storyTouchableNodeActionsToRun->spriteTag = runAction[j]["spriteTag"].asInt();
				storyTouchableNode->actionsToRun.push_back(storyTouchableNodeActionsToRun);
			}

			page->storyTouchableNodes.push_back(storyTouchableNode);
		}
	}
}

void JsonParser::parseWithStorySwipeEnded(Page *page, Json::Value &value)
{
	if (! value.isNull())
	{
		// runAction
		Json::Value runAction = value["runAction"];
		for (unsigned int i = 0; i < runAction.size(); ++i)
		{			
			StorySwipeEndedActionsToRun *action = new StorySwipeEndedActionsToRun();

			// runAfterSwipeNumber
			action->runAfterSwipeNumber = runAction[i]["runAfterSwipeNumber"].asInt();
			// spriteTag
			action->spriteTag = runAction[i]["spriteTag"].asInt();
			// actionTag
			action->actionTag = runAction[i]["actionTag"].asInt();

			page->storySwipeEnded.actionsToRun.push_back(action);
		}

		// addChild
		Json::Value childrenToAdd = value["addChild"]["children"];
		for (unsigned int j = 0; j < childrenToAdd.size(); ++j)
		{
			page->storySwipeEnded.spritesToAdd.push_back(childrenToAdd[j]["spriteTag"].asInt());
		}

		// removeChild
		Json::Value childrenToRemove = value["removeChild"]["children"];
		for (unsigned int k = 0; k < childrenToRemove.size(); ++k)
		{
			page->storySwipeEnded.spritesToMove.push_back(childrenToRemove[k]["spriteTag"].asInt());
		}
	}
}

void JsonParser::parseWithDelayTime(Page *page, Json::Value &value)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value delayTime = value[i];

			// duration
		    float duration = (float)delayTime["duratioin"].asDouble();
		    // actionTag
		    int actionTag = delayTime["actionTag"].asInt();

		    CCAction *action = CCDelayTime::actionWithDuration(duration);
		    page->addAction(actionTag, action);
		}		
	}
}

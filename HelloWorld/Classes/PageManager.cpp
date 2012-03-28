#include "PageManager.h"
#include "cocos2d.h"
#include "platform/CCFileUtils.h"
#include "SimpleAudioEngine.h"

#include <json/value.h>
#include <json/reader.h>
#include <string>

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

const char* PageManager::pathJsonFile = "pages/structure.json";
map<int, Page*> PageManager::pages = map<int, Page*>();

bool PageManager::parseJson()
{
	// read json file
	std::string doc;
	unsigned long size;
	doc = (char*)CCFileUtils::getFileData(pathJsonFile, "r", &size);

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

void PageManager::parseWithPage(Page* page, Json::Value &jsonPage)
{
	// settings
	Json::Value settings = jsonPage["settings"];
	parseWithSettings(page, settings);
	// text
	Json::Value text = jsonPage["text"];
	parseWithText(page, text);
}

void PageManager::parseWithSettings(Page* page, Json::Value &jsonSettings)
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
	settings.fontSize = jsonSettings["fontSize"].asInt();
	// background music file
	Json::Value backgroundMusicInfo = jsonSettings["backgroundMusicFile"];
	settings.loop = backgroundMusicInfo["loop"].asBool();
	settings.audioFilePath = backgroundMusicInfo["audioFilePath"].asCString();
}

void PageManager::parseWithText(Page* page, Json::Value &jsonText)
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
			lineText->xOffset = jsonLineText["xOffset"].asInt();
			lineText->yOffset = jsonLineText["yOffset"].asInt();

			paragraph->linesOfTest.push_back(lineText);
		}

		// add a new paragraph
		paragraphs.push_back(paragraph);	
	}

	
}

void PageManager::parseWithAPI(Page* page, Json::Value &jsonAPI)
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

void PageManager::parseWithBezierByOrBezierTo(Page *page, Json::Value &value, bool isBezierBy)
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
		    config.endPosition.x = (float)jsonEndPosition[x].asInt();
		    config.endPosition.y = (float)jsonEndPosition[y].asInt();
       
		    // controlPosition1
		    Json::Value jsonControlPosition1 = jsonBezierByOrBezierTo["config"]["controlPosition_1"];
		    config.controlPoint_1.x = (float)jsonEndPosition[x].asInt();
		    config.controlPoint_1.y = (float)jsonEndPosition[y].asInt();

		    // controlPosition2
		    Json::Value jsonControlPosition2 = jsonBezierByOrBezierTo["config"]["controlPosition_2"];
		    config.controlPoint_2.x = (float)jsonEndPosition[x].asInt();
		    config.controlPoint_2.y = (float)jsonEndPosition[y].asInt();

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


void PageManager::parseWithEaseBounceInOrEaseBounceOut(Page *page, Json::Value &value, bool isEaseBounceIn)
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

void PageManager::parseWithEaseInOrEaseOut(Page *page, Json::Value &value, bool isEaseIn)
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


void PageManager::parseWithFadeInOrFadeOut(Page *page, Json::Value &value, bool isFadeIn)
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



void PageManager::parseWithFlipxOrFlipy(Page *page, Json::Value &value, bool isFlipx)
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


void PageManager::parseWithFlipx3dOrFlipy3d(Page *page, Json::Value &value, bool isFlipx3d)
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

void PageManager::parseWithFadeTo(Page *page, Json::Value &value)
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


void PageManager::parseWithJumpToOrJumpBy(Page *page, Json::Value &value, bool isJumpTo)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value jumpToOrJumpBy = value[i];

			// position
			int x = 0, y = 1;
            CCPoint position;
			position.x = (float)jumpToOrJumpBy["position"][x].asInt();
			position.y = (float)jumpToOrJumpBy["position"][y].asInt();

			// duration
			float duration = (float)jumpToOrJumpBy["duration"].asDouble();
			// actionTag
			int actionTag = jumpToOrJumpBy["actionTag"].asInt();
			// height
			float height = (float)jumpToOrJumpBy["height"].asDouble();
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

void PageManager::parseWithRotateToOrRotateBy(Page *page, Json::Value &value, bool isRotateTo)
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

void PageManager::parseWithSkewToOrSkewBy(Page *page, Json::Value &value, bool isSkewTo)
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

void PageManager::parseWithMoveToOrMoveBy(Page *page, Json::Value &value, bool isMoveTo)
{
	if (! value.isNull())
	{
		for (unsigned int i = 0; i < value.size(); ++i)
		{
			Json::Value moveToOrBy = value[i];

			// position
			int x = 0, y = 1;
			CCPoint position;
			position.x = moveToOrBy["position"][x].asInt();
			position.y = moveToOrBy["position"][y].asInt();
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

void PageManager::parseWithScaleToOrScaleBy(Page *page, Json::Value &value, bool isScaleTo)
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

void PageManager::parseWithSequence(Page *page, Json::Value &value)
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

void PageManager::parseWithSprites(Page *page, Json::Value &value)
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
			spriteInfo->position.x = sprite["position"][x].asInt();
			spriteInfo->position.y = sprite["position"][y].asInt();
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

void PageManager::parseWithStoryTouchableNode(Page *page, Json::Value &value)
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
			storyTouchableNode->position.x = node["position"][x].asInt();
			storyTouchableNode->position.y = node["position"][y].asInt();
			// radius
			storyTouchableNode->radius = node["radius"].asInt();
			// videoToPlay
			storyTouchableNode->videoToPlay = node["videoToPlay"].asCString();
			// soundToPlay
			storyTouchableNode->soundToPlay = node["soundToPlay"].asCString();
			// runAction
			Json::Value runAction = node["runAction"];
			for (unsigned int j = 0; j < runAction.size(); ++j)
			{
				StoryTouchableNodeActionsToRun *storyTouchableNodeActionsToRun = new StoryTouchableNodeActionsToRun();
				storyTouchableNodeActionsToRun->actionTag = runAction[j]["actionTag"].asInt();
				storyTouchableNodeActionsToRun->spriteTag = runAction[j]["spriteTag"].asInt();
				storyTouchableNode->actionsToRun.push_back(storyTouchableNodeActionsToRun);
			}
		}
	}
}

void PageManager::parseWithStorySwipeEnded(Page *page, Json::Value &value)
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

void PageManager::parseWithDelayTime(Page *page, Json::Value &value)
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

void PageManager::insertPageWithPageNumber(int pageNumber, Page *page)
{
}

Page* PageManager::getPageByPageNumber(int pageNumber)
{
	std::map<int, Page*>::iterator iter = pages.find(pageNumber);
	if (iter != pages.end())
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}

CCScene* PageManager::createSceneByPageNumber(int pageNumber)
{
	CCScene *scene = NULL;
	Page *page = getPageByPageNumber(pageNumber);

	if (page)
	{
		// create a scene
		scene = CCScene::node();

		// create a layer because we want to receive touch event
		CCLayer *layer = CCLayer::node();
		scene->addChild(layer);

		createSprites(page, layer);
        createParagraphs(page, layer);
		playBackgroundMusic(page);
	}

	return scene;
}

void PageManager::createSprites(Page *page, cocos2d::CCLayer *layer)
{
	// create sprites and add to layer
	vector<SpriteInfo*>::iterator iter;
	for (iter = page->sprites.begin(); iter != page->sprites.end(); ++iter)
	{
		SpriteInfo* spriteInfo = *iter;

		//std::string               image;
	    //int                       spriteTag;
	    //cocos2d::CCPoint          position;
	    //std::vector<int>          actions; // actionTags

		CCSprite *sprite = CCSprite::spriteWithFile(spriteInfo->image.c_str());
		sprite->setTag(spriteInfo->spriteTag);
		sprite->setPosition(spriteInfo->position);

		// runAction
		for (int i = 0; i < spriteInfo->actions.size(); ++i)
		{
			CCAction *action = page->actions[spriteInfo->actions[i]];
			assert(action != NULL);
			sprite->runAction(action);
		}

		layer->addChild(sprite);
	}
}

void PageManager::createParagraphs(Page *page, cocos2d::CCLayer *layer)
{
	// font settings
	const char* fontName = page->settings.fontType.c_str();
	ccColor3B &fontColor = page->settings.fontColor;
	int fontSize = page->settings.fontSize;

	// add paragraphs
	vector<Paragraph*>::iterator iter;
	for (iter = page->paragraphs.begin(); iter != page->paragraphs.end(); ++iter)
	{
		vector<LineText*> &linesOfText = (*iter)->linesOfTest;

		vector<LineText*>::iterator lineTextIter;
		for (lineTextIter = linesOfText.begin(); lineTextIter != linesOfText.end(); ++lineTextIter)
		{
			LineText* lineText = *lineTextIter;
			CCLabelTTF *label = CCLabelTTF::labelWithString(lineText->text.c_str(), fontName, fontSize);
			label->setColor(fontColor);
			label->setPosition(ccp(lineText->xOffset, lineText->yOffset));

			layer->addChild(label);
		}
	}
}

void PageManager::playBackgroundMusic(Page *page)
{
	// file path
	const char *musicFilePath = page->settings.audioFilePath.c_str();
	bool loop = page->settings.loop;

	if (loop)
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(musicFilePath, true);
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(musicFilePath, false);
	}
}


#include "JsonParser.h"
#include "PageManager.h"
#include "MainMenu.h"
#include "Configurations.h"
#include "cocos2d.h"
#include "SharedGlobalData.h"

#include <json/value.h>
#include <json/reader.h>
#include <string>

using namespace cocos2d;
using namespace std;



bool JsonParser::parseJson(const char* pathOfJasonFile)
{
	// read json file
	unsigned long size;
    string fullPath = CCFileUtils::fullPathFromRelativePath(pathOfJasonFile);
    unsigned char* data = CCFileUtils::getFileData(fullPath.c_str(), "r", &size);
    std::string doc((char*)data);
    delete [] data;

	// read root
	Json::Reader reader;
	Json::Value root;
	if (! reader.parse(doc, root))
	{
		return false;
	}

    // parse json data
    parseConfigurations(root);
    parseMainMenu(root);
	parsePages(root);

	return true;;
}

void JsonParser::parseConfigurations(Json::Value &root)
{
    Json::Value configurations = root["Configurations"];
    // forward effect
    Configurations::forwardEffect = configurations["pageFlipSound"]["forward"].asCString();
    // backward effect
    Configurations::backwardEffect = configurations["pageFlipSound"]["backward"].asCString();
    // page flip transition duration
    Configurations::pageFlipTransitionDuration = (float)configurations["pageFlipTransitionDuration"].asDouble();
    // paragraph text fade duration
    Configurations::paragraphTextFadeDuration = (float)configurations["paragraphTextFadeDuration"].asDouble();
    // whether text scaling ratio the same as picture
    Configurations::retainTextScalingRatio = configurations["retainTextScalingRatio"].asBool();
    // whether or not to skip animation on swip
    // XXX: is "skipAnimationOnSwipe" optional?
    Json::Value skipAnimation = configurations["skipAnimationOnSwipe"];
    if (! skipAnimation.isNull())
        Configurations::skipAnimationOnSwipe = skipAnimation.asBool();
    else
        Configurations::skipAnimationOnSwipe = false;
    
    // home menu for pages
    
    Json::Value homeMenuForPages = configurations["homeMenuForPages"];
    // normal state image
    Configurations::homeButtonNormalStateImage = homeMenuForPages["normalStateImage"].asCString();
    // tapped state iamge
    Configurations::homeButtonTappedStateImage = homeMenuForPages["tappedStateImage"].asCString();
    // position
    int x = 0, y = 1;
    Configurations::homeButtonPosition.x = (float)homeMenuForPages["position"][x].asDouble() * XSCALE;
    Configurations::homeButtonPosition.y = (float)homeMenuForPages["position"][y].asDouble() * YSCALE;
    Configurations::homeButtonScale = homeMenuForPages["scale"].asDouble();
}

void JsonParser::parseMainMenu(Json::Value &root)
{
    Json::Value mainMenu = root["MainMenu"];
    
    // parse audio
    Json::Value sounds = mainMenu["sounds"];
    if (! sounds.isNull())
    {
        parseMainMenuAodio(sounds);
    }
    
    // parse CCSprites
    Json::Value sprites = mainMenu["CCSprites"];
    parseMainMenuSprites(sprites);
    
    // prase menu items
    Json::Value menuItems = mainMenu["MenuItems"];
    parseMenuItems(menuItems);
    
    // parse API
    Json::Value api = mainMenu["API"];
    parseMainMenuAPI(api);
    
    // parse runActionsOnEnter
    Json::Value runActionsOnEnterJson = mainMenu["runActionsOnEnter"];
    parseMainMenuRunActionsOnEnter(runActionsOnEnterJson);
    
    // parse falling physics setting
    parseMainMenuFallingObjectSetting(mainMenu);
}

void JsonParser::parseMainMenuAodio(Json::Value &sounds)
{
    // backgroundMusic
    Json::Value background = sounds["background"];
    if (! background.isNull())
    {
        MainMenu::audio.backgroundMusic = background["file"].asCString();
        // backgroundMusicLoops
        MainMenu::audio.isBackgroundMusicLoop = background["loop"].asBool();
    }
    
    Json::Value effectOnEnter = sounds["effectOnEnter"];
    if (! effectOnEnter.isNull())
    {
        // soundEffect
        MainMenu::audio.effectOnEnter = effectOnEnter.asCString();
    }    
}

void JsonParser::parseMainMenuSprites(Json::Value &value)
{
    for (int i = 0; i < value.size(); ++i)
    {
        Json::Value spriteJson = value[i];
        MainMenuSpriteInfo *spriteInfo = new MainMenuSpriteInfo(); 
        
        // image
        spriteInfo->image = spriteJson["image"].asCString();
        // spriteTag
        spriteInfo->spriteTag = spriteJson["spriteTag"].asInt();
        // visible
        spriteInfo->visible = spriteJson["visible"].asBool();
        // position
        int x = 0, y = 1;
        spriteInfo->position.x = spriteJson["position"][x].asInt() * XSCALE;
        spriteInfo->position.y = spriteJson["position"][y].asInt() * YSCALE;
        // scale
        spriteInfo->scale = spriteJson["scale"].asDouble();
        
        // zOrder
        spriteInfo->zOrder = spriteJson["zOrder"].asInt();
        
        MainMenu::sprites.push_back(spriteInfo);
    }
}

void JsonParser::parseMenuItems(Json::Value &value)
{
    for (int i = 0; i < value.size(); ++i)
    {
        Json::Value menuItemJson = value[i];
        MainMenuItemInfo *menuItem = new MainMenuItemInfo();
        
        // normalStateImage
        menuItem->normalStateImage = menuItemJson["normalStateImage"].asCString();
        // tappedstateImage
        menuItem->tappedStateImage = menuItemJson["tappedStateImage"].asCString();
        // story mode
        if (! menuItemJson["storyMode"].isNull())
        {
            menuItem->storyMode = menuItemJson["storyMode"].asCString();
            menuItem->mainMenuItemType = kNormalMainMenuItemType;
        }
        if (! menuItemJson["videoToPlay"].isNull())
        {
            menuItem->videoToPlay = menuItemJson["videoToPlay"].asCString();
            menuItem->mainMenuItemType = kPlayVideoMainMenuItemType;
        }
        if (! menuItemJson["url"].isNull())
        {
            menuItem->url = menuItemJson["url"].asCString();
            menuItem->mainMenuItemType = kUrlMainMenuItemType;
        }
        // position
        int x = 0, y = 1;
        menuItem->position.x = menuItemJson["position"][x].asInt() * XSCALE;
        menuItem->position.y = menuItemJson["position"][y].asInt() * YSCALE;
        // scale
        menuItem->scale = menuItemJson["scale"].asDouble();
        // zOrder
        menuItem->zOrder = menuItemJson["zOrder"].asInt();
        
        MainMenu::menuItems.push_back(menuItem);
    }
}

void JsonParser::parseMainMenuAPI(Json::Value &value)
{
    // CCFadeIn
    Json::Value fadeInJson = value["CCFadeIn"];
    for (int i = 0; i < fadeInJson.size(); ++i)
    {
        ActionFadeIn *actionFadeIn = new ActionFadeIn();
        
        // duration
        actionFadeIn->duration = fadeInJson[i]["duration"].asInt();
        // actionTag
        actionFadeIn->actionTag = fadeInJson[i]["actionTag"].asInt();
        
        MainMenu::actionsFadeIn.push_back(actionFadeIn);
    }
}

void JsonParser::parseMainMenuRunActionsOnEnter(Json::Value &value)
{
    // run actions on enter    
    for (int j = 0; j < value.size(); ++j)
    {
        ActionToRunOnEnter *actionToRun = new ActionToRunOnEnter();
        
        // spriteTag
        actionToRun->spriteTag = value[j]["spriteTag"].asInt();
        // actionTag
        actionToRun->actionTag = value[j]["actionTag"].asInt();
        
        MainMenu::actionsToRunOnEnter.push_back(actionToRun);
    }
}

void JsonParser::parseMainMenuFallingObjectSetting(Json::Value &value)
{
    MainMenu::fallingObjectSetting = FallingObjectSetting();
    Json::Value fallingPhysicsSettingJson = value["fallingPhysicsSettings"];
    MainMenu::fallingObjectSetting.draggble = fallingPhysicsSettingJson["draggable"].asBool();
    MainMenu::fallingObjectSetting.maxNumber = fallingPhysicsSettingJson["maxNumber"].asInt();
    MainMenu::fallingObjectSetting.speedX = fallingPhysicsSettingJson["sppedX"].asDouble() * XSCALE;
    MainMenu::fallingObjectSetting.speedY = fallingPhysicsSettingJson["speedY"].asDouble() * YSCALE;
    MainMenu::fallingObjectSetting.slowDownSpeed = fallingPhysicsSettingJson["slowDownSpeed"].asDouble() * XSCALE;
    MainMenu::fallingObjectSetting.hasFloor = fallingPhysicsSettingJson["hasFloor"].asBool();
    MainMenu::fallingObjectSetting.hasWalls = fallingPhysicsSettingJson["hasWalls"].asBool();
    int x = 0;
    int y = 1;
    MainMenu::fallingObjectSetting.dropBetweenPoints.x = fallingPhysicsSettingJson["dropBetweenPoints"][x].asDouble() * XSCALE;
    MainMenu::fallingObjectSetting.dropBetweenPoints.y = fallingPhysicsSettingJson["dropBetweenPoints"][y].asDouble() * YSCALE;
    MainMenu::fallingObjectSetting.plistfilename = fallingPhysicsSettingJson["plistfilename"].asCString();
}

void JsonParser::parsePages(Json::Value &root)
{
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
    
	// background music file
    // may not have background music to play, so should check it
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
    
    // falling physics object setting
    Json::Value fallingPhysicsSettings = jsonSettings["fallingPhysicsSettings"];
    if (! fallingPhysicsSettings.isNull())
    {
        FallingObjectSetting &fallingSet = settings.fallingObjectSetting;
        
        fallingSet.draggble = fallingPhysicsSettings["draggable"].asBool();
        fallingSet.maxNumber = fallingPhysicsSettings["maxNumber"].asInt();
        fallingSet.speedX = fallingPhysicsSettings["speedX"].asInt();
        fallingSet.speedY = fallingPhysicsSettings["speedY"].asInt();
        fallingSet.spinSpeed = fallingPhysicsSettings["spinSpeed"].asInt();
        fallingSet.slowDownSpeed = fallingPhysicsSettings["slowDownSpeed"].asDouble();
        fallingSet.hasFloor = fallingPhysicsSettings["hasFloor"].asBool();
        fallingSet.hasWalls = fallingPhysicsSettings["hasWalls"].asBool();
        
        int x = 0;
        int y = 1;
        fallingSet.dropBetweenPoints.x = fallingPhysicsSettings["dropBetweenPoints"][x].asDouble() * XSCALE;
        fallingSet.dropBetweenPoints.y = fallingPhysicsSettings["dropBetweenPoints"][y].asDouble() * YSCALE;
        
        fallingSet.plistfilename = fallingPhysicsSettings["plistfilename"].asCString();
    }
    
    // static physics object setting
    Json::Value staticPhysicsSettings = jsonSettings["staticPhysicsSettings"];
    if (! staticPhysicsSettings.isNull())
    {
        StaticObjectSetting &staticSet = settings.staticObjectSetting;

        staticSet.plistfilename = staticPhysicsSettings["plistfilename"].asCString();
        
        Json::Value staticObjectInfo = staticPhysicsSettings["objects"];
        for (unsigned int i = 0; i < staticObjectInfo.size(); ++i) 
        {
            StaticObjectInfo *info = new StaticObjectInfo();
            info->filename = staticObjectInfo[i]["filename"].asCString();
            
            int x = 0;
            int y = 1;
            info->position.x = staticObjectInfo[i]["position"][x].asDouble() * XSCALE;
            info->position.y = staticObjectInfo[i]["position"][y].asDouble() * YSCALE;
            
            page->settings.staticObjectSetting.staticObjects.push_back(info);
        }
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
        
        // autoplayDuration
        paragraph->autoplayDuration = jsonParagraph["autoplayDuration"].asDouble();
        
        // delayBeforeShowingText
        paragraph->delayBeforeShowingText = jsonParagraph["delayBeforeShowingText"].asDouble();
        
		// linesOfText
		Json::Value linesOfText = jsonParagraph["linesOfText"];
		for (unsigned int k = 0; k < linesOfText.size(); ++k)
		{
			Json::Value jsonLineText = linesOfText[k];
            string text = jsonLineText["text"].asCString();
            
            Json::Value anchorPointValue = jsonLineText["anchorPoint"];
            int xAnchor = 0, yAnchor = 1;
            cocos2d::CCPoint anchorPoint = ccp(anchorPointValue[xAnchor].asDouble(), anchorPointValue[yAnchor].asDouble());


            // split text into lines
            std::vector<string> lines;
            int startPos = 0;
            int endPos = text.find('\n', startPos);
            while (endPos != string::npos) {
                // exclude \n
                lines.push_back(text.substr(startPos, endPos - startPos));
                startPos = endPos + 1;
                endPos = text.find_first_of('\n', startPos);
            }
            lines.push_back(text.substr(startPos, text.size() - startPos));

            int yInnerOffset = 0;
            for (int lineNumber = 0; lineNumber < lines.size(); ++lineNumber)
            {
                string &line = lines[lineNumber];
                
                LineText *lineText = new LineText();
                lineText->text = line;
                
                lineText->xOffset = jsonLineText["xOffset"].asInt() * XSCALE;
                lineText->yOffset = (jsonLineText["yOffset"].asInt() + yInnerOffset) * YSCALE;

                lineText->anchorPoint = anchorPoint;

                // split text into words
                page->splitText(lineText);
                
                // fontType
                lineText->fontType = jsonLineText["fontType"].asCString();
                
                // fontColor(r,g,b)
                Json::Value fontColor = jsonLineText["fontColor"];
                int r = 0, g = 1, b = 2;
                lineText->fontColor.r = fontColor[r].asUInt();
                lineText->fontColor.g = fontColor[g].asUInt();
                lineText->fontColor.b = fontColor[b].asUInt();
                
                // font highlight color
                Json::Value fontHighlightColor = jsonLineText["fontHighlightColor"];
                lineText->fontHighlightColor.r = fontHighlightColor[r].asUInt();
                lineText->fontHighlightColor.g = fontHighlightColor[g].asUInt();
                lineText->fontHighlightColor.b = fontHighlightColor[b].asUInt();
                
                // font size
                lineText->fontSize = jsonLineText["fontSize"].asDouble();
                yInnerOffset -= lineText->fontSize;
                if (XSCALE != 1)
                {
                    if (! Configurations::retainTextScalingRatio)
                    {
                        // add font size because it will be scaled down
                        lineText->fontSize += 8;
                    }
                }
                
                paragraph->linesOfTest.push_back(lineText);
            }
		}
        
        // hotspots
        Json::Value hotspotsJson = jsonParagraph["hotspots"];
        Json::Value hotspotJson;
        for (unsigned int l = 0; l < hotspotsJson.size(); ++l)
        {
            HotspotInfo *h = new HotspotInfo();
            h->particle = NULL;
            hotspotJson = hotspotsJson[l];
            
            // glitterIndicator
            h->glitterIndicator = hotspotJson["glitterIndicator"].asBool();
            
            // stopSoundAndHighlightingWhenTouched
            h->stopSoundAndHighlightingWhenTouched = hotspotJson["stopSoundAndHighlightingWhenTouched"].asBool();
            
            // delayAfterVideoDuringAutoplay
            h->delayAfterVideoDuringAutoplay = hotspotJson["delayAfterVideoDuringAutoplay"].asInt();
            
            // touchFlag
            h->touchFlag = hotspotJson["touchFlag"].asInt();
            
            // position
            int x = 0, y = 1;
            h->position.x = (float)hotspotJson["position"][x].asDouble() * XSCALE;
            h->position.y = (float)hotspotJson["position"][y].asDouble() * YSCALE;
            
            // radius
            h->radius = hotspotJson["radius"].asInt();// * MIN_SCALE;
            
            // soundToPlay
            Json::Value soundToPlayJson = hotspotJson["soundToPlay"];
            if (! soundToPlayJson.isNull())
            {
                h->soundToPlay = soundToPlayJson.asCString();
            }
            
            // videoToPlay
            Json::Value videoToPlayJson = hotspotJson["videoToPlay"];
            if (! videoToPlayJson.isNull())
            {
                h->videoToPlay = videoToPlayJson.asCString();
            }
            
            // actions to run
            Json::Value actionsToRunJson = hotspotJson["actionsToRun"];
            if (! actionsToRunJson.isNull())
            {
                for (int j = 0; j < actionsToRunJson.size(); ++j)
                {
                    SpriteAction spriteAction;
                    spriteAction.spriteTag = actionsToRunJson[j]["spriteTag"].asInt();
                    spriteAction.actionTag = actionsToRunJson[j]["actionTag"].asInt();
                    h->actionsToRun.push_back(spriteAction);
                }
            }
            
            paragraph->hotspots.push_back(h);
        }
        
        // voiceAudioFile
        paragraph->voiceAudioFile = jsonParagraph["voiceAudioFile"].asString();

		// add a new paragraph
		paragraphs.push_back(paragraph);	
	}
}

void JsonParser::parseWithAPI(Page* page, Json::Value &jsonAPI)
{
    for (int i = 0; i < jsonAPI.size(); ++i)
    {
        Json::Value jsonActions = jsonAPI[i];
        std::string actionName = jsonActions.getMemberNames()[0];
        Json::Value jsonAction = jsonActions[actionName];
        
        // CCBezierBy
        if (actionName.compare("CCBezierBy") == 0)
        {
            parseWithBezierByOrBezierTo(page, jsonAction, true);
        }
        
        // CCBezierTo
        if (actionName.compare("CCBezierTo") == 0)
        {
            parseWithBezierByOrBezierTo(page, jsonAction, false);
        }
        
        // CCFadeIn
        if (actionName.compare("CCFadeIn") == 0)
        {
            parseWithFadeInOrFadeOut(page, jsonAction, true);
        }
        
        // CCFadeOut
        if (actionName.compare("CCFadeOut") == 0)
        {
            parseWithFadeInOrFadeOut(page, jsonAction, false);
        }
        
        // CCFadeTo
        if (actionName.compare("CCFadeTo") == 0)
        {
            parseWithFadeTo(page, jsonAction);
        }
        
        // CCFlipX
        if (actionName.compare("CCFlipX") == 0)
        {
            parseWithFlipxOrFlipy(page, jsonAction, true);
        }
        
        // CCFlipY
        if (actionName.compare("CCFlipY") == 0)
        {
            parseWithFlipxOrFlipy(page, jsonAction, false);
        }
        
        // CCFlipX3D
        if (actionName.compare("CCFlipX3D") == 0)
        {
            parseWithFlipx3dOrFlipy3d(page, jsonAction, true);
        }
        
        // CCFlipY3D
        if (actionName.compare("CCFlipY3D") == 0)
        {
            parseWithFlipx3dOrFlipy3d(page, jsonAction, false);
        }
        
        // CCJumpTo
        if (actionName.compare("CCJumpTo") == 0)
        {
            parseWithJumpToOrJumpBy(page, jsonAction, true);
        }
        
        // CCJumpBy
        if (actionName.compare("CCJumpBy") == 0)
        {
            parseWithJumpToOrJumpBy(page, jsonAction, false);
        }
        
        // CCRotateTo
        if (actionName.compare("CCRotateTo") == 0)
        {
            parseWithRotateToOrRotateBy(page, jsonAction, true);
        }
        
        // CCRotateBy
        if (actionName.compare("CCRotateBy") == 0)
        {
            parseWithRotateToOrRotateBy(page, jsonAction, false);
        }
        
        // CCSkewTo
        if (actionName.compare("CCSkewTo") == 0)
        {
            parseWithSkewToOrSkewBy(page, jsonAction, true);
        }
        
        // CCSkewBy
        if (actionName.compare("CCSkewBy") == 0)
        {
            parseWithSkewToOrSkewBy(page, jsonAction, false);
        }
        
        // CCMoveTo
        if (actionName.compare("CCMoveTo") == 0)
        {
            parseWithMoveToOrMoveBy(page, jsonAction, true);
        }
        
        // CCMoveBy
        if (actionName.compare("CCMoveBy") == 0)
        {
            parseWithMoveToOrMoveBy(page, jsonAction, false);
        }
        
        // CCScaleTo
        if (actionName.compare("CCScaleTo") == 0)
        {
            parseWithScaleToOrScaleBy(page, jsonAction, true);
        }
        
        // CCScaleBy
        if (actionName.compare("CCScaleBy") == 0)
        {
            parseWithScaleToOrScaleBy(page, jsonAction, false);
        }
        
        // CCDelayTime
        if (actionName.compare("CCDelayTime") == 0)
        {
            parseWithDelayTime(page, jsonAction);
        }
        
        // CCSpawn
        if (actionName.compare("CCSpawn") == 0)
        {
            parseWithSpawn(page, jsonAction);
        }
        
        // CCSequence
        if (actionName.compare("CCSequence") == 0)
        {
            parseWithSequence(page, jsonAction);
        }
        
        // CCSprites
        if (actionName.compare("CCSprites") == 0)
        {
            parseWithSprites(page, jsonAction);
        }
        
        // CCStoryTouchableNode
        if (actionName.compare("CCStoryTouchableNode") == 0)
        {
            parseWithStoryTouchableNode(page, jsonAction);
        }
        
        // CCStorySwipeEnded
        if (actionName.compare("CCStorySwipeEnded") == 0)
        {
            parseWithStorySwipeEnded(page, jsonAction);
        }
        
        // CCEaseBounceIn
        if (actionName.compare("CCEaseBounceIn") == 0)
        {
            parseWithEaseBounceInOrEaseBounceOut(page, jsonAction, true);
        }
        
        // CCEaseBounceOut
        if (actionName.compare("CCEaseBounceOut") == 0)
        {
            parseWithEaseBounceInOrEaseBounceOut(page, jsonAction, false);
        }
        
        // CCEaseIn
        if (actionName.compare("CCEaseIn") == 0)
        {
            parseWithEaseInOrEaseOut(page, jsonAction, true);
        }
        
        // CCEaseOut
        if (actionName.compare("CCEaseOut") == 0)
        {
            parseWithEaseInOrEaseOut(page, jsonAction, false);
        }
        
        // CCRepeatForever
        if (actionName.compare("CCRepeatForever") == 0)
        {
            parseWithRepeatForever(page, jsonAction);
        }
        
        // CCBlink
        if (actionName.compare("CCBlink") == 0)
        {
            parseWithBlink(page, jsonAction);
        }
    }
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
		    Json::Value jsonEndPosition = jsonBezierByOrBezierTo["endPosition"];
		    int x = 0, y = 1;
		    config.endPosition.x = (float)jsonEndPosition[x].asInt() * XSCALE;
		    config.endPosition.y = (float)jsonEndPosition[y].asInt() * YSCALE;
       
		    // controlPosition1
		    Json::Value jsonControlPosition1 = jsonBezierByOrBezierTo["controlPosition1"];
		    config.controlPoint_1.x = (float)jsonControlPosition1[x].asInt() * XSCALE;
		    config.controlPoint_1.y = (float)jsonControlPosition1[y].asInt() * YSCALE;

		    // controlPosition2
		    Json::Value jsonControlPosition2 = jsonBezierByOrBezierTo["controlPosition2"];
		    config.controlPoint_2.x = (float)jsonControlPosition2[x].asInt() * XSCALE;
		    config.controlPoint_2.y = (float)jsonControlPosition2[y].asInt() * YSCALE;

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
		    // innerAction
		    int internalActionTag = easeInOrOut["innerAction"].asInt();
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
			position.x = (float)jumpToOrJumpBy["position"][x].asInt() * XSCALE;
			position.y = (float)jumpToOrJumpBy["position"][y].asInt() * YSCALE;

			// duration
			float duration = (float)jumpToOrJumpBy["duration"].asDouble();
			// actionTag
			int actionTag = jumpToOrJumpBy["actionTag"].asInt();
			// height
			float height = (float)jumpToOrJumpBy["height"].asDouble() * YSCALE;
			// jumps
			int jumps = jumpToOrJumpBy["numberOfJumps"].asInt();

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
			int deltaSkewX = skewToOrBy["deltaX"].asInt();
			// deltaSkewY
			int deltaSkewY = skewToOrBy["deltaY"].asInt();
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
			position.x = moveToOrBy["position"][x].asInt() * XSCALE;
			position.y = moveToOrBy["position"][y].asInt() * YSCALE;
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
				action = CCScaleTo::actionWithDuration(duration, intensity * XSCALE, intensity * YSCALE);
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
        for (unsigned int i = 0; i < value.size(); ++i)
        {
            Json::Value jsonSequence = value[i];
            
            // actionTag
            int actionTag = jsonSequence["actionTag"].asInt();
            // actions
            Json::Value actions = jsonSequence["actions"];
            CCArray *arr = CCArray::array();
            for (unsigned int j = 0; j < actions.size(); ++j)
            {
                int actionToAddTag = actions[j].asInt();
                CCAction *actionToAdd = page->getActionByTag(actionToAddTag);
                assert(actionToAdd != NULL);
                assert(dynamic_cast<CCFiniteTimeAction*>(actionToAdd) != NULL);
                arr->addObject((CCFiniteTimeAction*)actionToAdd);
            }
            
            CCAction *sequence = CCSequence::actionsWithArray(arr);
            page->addAction(actionTag, sequence);
        }		
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
			// zOrder
			spriteInfo->zOrder = sprite["zOrder"].asInt();
            // position
			int x = 0, y = 1;
			spriteInfo->position.x = sprite["position"][x].asInt() * XSCALE;
			spriteInfo->position.y = sprite["position"][y].asInt() * YSCALE;
            // visible
            spriteInfo->visible = sprite["visible"].asBool();
			// actions
			Json::Value actions = sprite["actions"];
			for (unsigned int j = 0; j < actions.size(); ++j)
			{
				spriteInfo->actions.push_back(actions[j].asInt());
			}
			
			page->sprites.push_back(spriteInfo);
            page->images.push_back(spriteInfo->image);
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
            storyTouchableNode->hotspotInfo.particle = NULL;

			// glitterIndicator
			storyTouchableNode->hotspotInfo.glitterIndicator = node["glitterIndicator"].asBool();
            // autoplayVideoFinishedDelay
            Json::Value value = node["delayAfterVideoDuringAutoplay"];
            if (! value.isNull())
            {
                storyTouchableNode->hotspotInfo.delayAfterVideoDuringAutoplay = value.asDouble();
            }
            else
            {
                storyTouchableNode->hotspotInfo.delayAfterVideoDuringAutoplay = 0;
            }
            // stopEffectIndicator
            Json::Value stopEffectIndicatorJson = node["stopSoundAndHighlightingWhenTouched"];
            if (! stopEffectIndicatorJson.isNull())
            {
                storyTouchableNode->hotspotInfo.stopSoundAndHighlightingWhenTouched = stopEffectIndicatorJson.asBool();
            }
            else 
            {
                storyTouchableNode->hotspotInfo.stopSoundAndHighlightingWhenTouched = true;
            }
            // delayForAnimation
            Json::Value delayForAnimationJson = node["delayForAnimation"];
            if (! delayForAnimationJson.isNull())
            {
                storyTouchableNode->delayForAnimation = delayForAnimationJson.asBool();
            }
            else 
            {
                storyTouchableNode->delayForAnimation = false;
            }
            // delayForText
            Json::Value delayTextJson = node["delayForText"];
            if (! delayTextJson.isNull())
            {
                storyTouchableNode->delayForText = delayTextJson.asBool();
            }
            else 
            {
                storyTouchableNode->delayForText = false;
            }
			// position
			int x = 0, y = 1;
			storyTouchableNode->hotspotInfo.position.x = node["position"][x].asInt() * XSCALE;
			storyTouchableNode->hotspotInfo.position.y = node["position"][y].asInt() * YSCALE;
			// radius
			storyTouchableNode->hotspotInfo.radius = node["radius"].asInt() * MIN_SCALE;
			// videoToPlay
            if (! node["videoToPlay"].isNull())
            {
                storyTouchableNode->hotspotInfo.videoToPlay = node["videoToPlay"].asCString();
            }
			// soundToPlay
            if (! node["soundToPlay"].isNull())
            {
                storyTouchableNode->hotspotInfo.soundToPlay = node["soundToPlay"].asCString();
            }			
			// touchFlag
			storyTouchableNode->hotspotInfo.touchFlag = node["touchFlag"].asInt();
			// runAction
			Json::Value runAction = node["runAction"];
			
			for (unsigned int j = 0; j < runAction.size(); ++j)
			{
				SpriteAction *storyTouchableNodeActionsToRun = new SpriteAction();
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
			// actionTags
            Json::Value actionTags = runAction[i]["actionTags"];
			for (unsigned int k =0; k < actionTags.size(); ++k)
            {
                action->actionTags.push_back(actionTags[k].asInt()); 
            }
            
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
		    float duration = (float)delayTime["duration"].asDouble();
		    // actionTag
		    int actionTag = delayTime["actionTag"].asInt();

		    CCAction *action = CCDelayTime::actionWithDuration(duration);
		    page->addAction(actionTag, action);
		}		
	}
}

void JsonParser::parseWithSpawn(Page *page, Json::Value &value)
{
    if (! value.isNull())
    {
        for (unsigned int i = 0; i < value.size(); ++i)
        {
            Json::Value jsonSpawn = value[i];
            
            // actionTag
            int actionTag = jsonSpawn["actionTag"].asInt();
            // actions
            Json::Value actions = jsonSpawn["actions"];
            CCArray *arr = CCArray::array();
            for (unsigned int j = 0; j < actions.size(); ++j)
            {
                int actionToAddTag = actions[j].asInt();
                CCAction *actionToAdd = page->getActionByTag(actionToAddTag);
                assert(actionToAdd != NULL);
                assert(dynamic_cast<CCFiniteTimeAction*>(actionToAdd) != NULL);
                arr->addObject((CCFiniteTimeAction*)actionToAdd);
            }
            CCFiniteTimeAction *action = CCSpawn::actionsWithArray(arr);
            page->addAction(actionTag, action);
        }
    }
}

void JsonParser::parseWithRepeatForever(Page *page, Json::Value &value)
{
    if (! value.isNull())
    {
        for (unsigned int i = 0; i < value.size(); ++i)
        {
            Json::Value jsonRepeatForever = value[i];
            
            // actionTag
            int actionTag = jsonRepeatForever["actionTag"].asInt();
            // innerAction
            int innerActionTag = jsonRepeatForever["innerAction"].asInt();
            CCActionInterval *actionToAdd = (CCActionInterval*)(page->getActionByTag(innerActionTag));
            assert(actionToAdd != NULL);
            
            CCFiniteTimeAction *action = CCRepeat::actionWithAction(actionToAdd, std::numeric_limits<int>::max());
            page->addAction(actionTag, action);
        }
    }
}

void JsonParser::parseWithBlink(Page *page, Json::Value &value)
{
    if (! value.isNull())
    {
        for (unsigned int i = 0; i < value.size(); ++i)
        {
            Json::Value jsonRepeatForever = value[i];
            
            // actionTag
            int actionTag = jsonRepeatForever["actionTag"].asInt();
            // duration
            float duration = (float)jsonRepeatForever["duration"].asDouble();
            
            // Blink for ever
            CCFiniteTimeAction *action = CCRepeat::actionWithAction(CCBlink::actionWithDuration(duration, 1), std::numeric_limits<int>::max());
            page->addAction(actionTag, action);
        }
    }
}

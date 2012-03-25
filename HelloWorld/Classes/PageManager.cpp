#include "PageManager.h"
#include "cocos2d.h"
#include "platform/CCFileUtils.h"

#include <json/value.h>
#include <json/reader.h>
#include <string>

using namespace std;
using namespace cocos2d;

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
	settings.numberTimesToLoop = backgroundMusicInfo["numberTimesToLoop"].asInt();
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

		// highlightingTimes
		Json::Value highlightingTimes = jsonParagraphs["highlightingTimes"];
		for (unsigned int j = 0; j < highlightingTimes.size(); ++j)
		{
			paragraph->highlightingTimes.push_back((float)highlightingTimes[j].asDouble());
		}
		// linesOfText
		Json::Value linesOfText = jsonParagraphs["linesOfText"];
		for (unsigned int k = 0; k < linesOfText.size(); ++k)
		{
			LineText *lineText = new LineText();
			lineText->text = linesOfText["text"].asCString();
			lineText->xOffset = linesOfText["xOffset"].asInt();
			lineText->yOffset = linesOfText["yOffset"].asInt();

			paragraph->linesOfTest.push_back(lineText);
		}

		// add a new paragraph
		paragraphs.push_back(paragraph);	
	}

	
}

void PageManager::parseWithAPI(Page* page, Json::Value &jsonAPI)
{
}

void PageManager::insertPageWithPageNumber(int pageNumber, Page *page)
{
}

Page* PageManager::getPage(int pageNumber)
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

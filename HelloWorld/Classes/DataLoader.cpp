#include "DataLoader.h"
#include "PageManager.h"
#include "Page.h"

using namespace std;
using namespace cocos2d;


void DataLoader::loadAssetsAsync(int pageNumber)
{
    Page* page = PageManager::pages[pageNumber];
    std::vector<string> &images = page->images;
    std::vector<string>::iterator iter = images.begin();
    for (; iter != images.end(); ++iter)
    {
        CCTextureCache::sharedTextureCache()->addImageAsync((*iter).c_str(), NULL, NULL);
    }
}
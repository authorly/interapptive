#ifndef __DATA_LOADER_H__
#define __DATA_LOADER_H__

#include "cocos2d.h"

#include <string>
#include <vector>

class DataLoader : public cocos2d::CCObject
{
public:
    static void loadAssetsAsync(int pageNumber);
};

#endif // __DATA_LOADER_H__

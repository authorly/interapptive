#ifndef __DATA_LOADER_H__
#define __DATA_LOADER_H__

#include "cocos2d.h"

#include <string>
#include <vector>

class DataLoader : public cocos2d::CCObject
{
public:
	DataLoader* instance();
	void loadAssetsAsync(std::vector<string> *filePathArray, cocos2d::CCObject *target, cocos2d::SEL_CallFunc callBack);
	void didLoadImage(cocos2d::CCObject *sender);
private:
	int totalToLoad;
	int loadPointer;
	cocos2d::CCObject *target;
	cocos2d::SEL_CallFunc callBack;
	std::vector<string> *filePathArray;
};

#endif // __DATA_LOADER_H__

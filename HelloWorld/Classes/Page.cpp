#include "Page.h"

using namespace cocos2d;
using namespace std;

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

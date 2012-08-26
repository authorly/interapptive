#include "SnowmanGameScene.h"
#include "SharedGlobalData.h"
#include "PageManager.h"

#include <string>

// macros of coordinate

#define ARROW_MENU_POSITION             ccp(893*XSCALE, 245*YSCALE)
#define SPRITE_BETWEEN_ARROW_POSITION   ccp(805*XSCALE, 245*YSCALE)
#define SCARF_BETWEEN_ARROW_POSITION    ccp(1080*XSCALE, 245*YSCALE)
#define LEFT_ARROW_POSITION             ccp(-250*XSCALE, 0)
#define SHARE_MENU_POSITION             ccp(832*XSCALE, 75*YSCALE)
#define LOGO_POSITION                   ccp(700*XSCALE, 690*YSCALE)
#define HOME_MENU_POSITION              ccp(35*XSCALE, 35*YSCALE)

// position to draw on snow man
#define EYE_POSITION                    ccp(373*XSCALE, 484*YSCALE)
#define MOUTH_POSITION                  ccp(373*XSCALE, 401*YSCALE)
#define MITTEN_POSITION                 ccp(392*XSCALE, 388*YSCALE)
#define HAT_POSITION                    ccp(373*XSCALE, 546*YSCALE)
#define NOSE_POSITION                   ccp(403*XSCALE, 449*YSCALE)
#define SCARF_POSITION                  ccp(350*XSCALE, 240*YSCALE)

// position fo menu
#define CATEGORY_POSITION       ccp(650*XSCALE, 380*YSCALE)
#define CATEGORY_LOWER_POSITION ccp(650*XSCALE, 285*YSCALE)

#define HAT_TAG     1
#define EYE_TAG     2
#define MITTEN_TAG  3
#define MOUTH_TAG   4
#define SCARF_TAG   5
#define NOSE_TAG    6

#define SPRITE_BETWEEN_ARROW_TAG       7

#define LEFT_ARROW_SPRITE_TAG   8
#define RIGHT_ARROW_SPRITE_TAG  9

#define SPRITE_TO_DRAG_TAG      10

#define SNOWMAN_HAT_TAG         11
#define SNOWMAN_EYE_TAG         12
#define SNOWMAN_MITTEN_TAG      13
#define SNOWMAN_MOUTH_TAG       14
#define SNOWMAN_SCARF_TAG       15
#define SNOWMAN_NOSE_TAG        16

#define FACEBOOK_TAG            17
#define TWITTER_TAG             18

#define HAT_SCALE   0.7
#define SCARF_SCALE 0.6

#define NUMBER_OF_SELECTION      4


struct SpriteBetweenArrowData
{
    int tag;
    int index;
    float scale;
};


using namespace cocos2d;
using namespace std;

SnowmanGameScene::~SnowmanGameScene()
{
    CC_SAFE_RELEASE(noseMenuItem);
    CC_SAFE_RELEASE(mouthMenuItem);
    CC_SAFE_RELEASE(hatMenuItem);
    CC_SAFE_RELEASE(mittenMenuItem);
    CC_SAFE_RELEASE(scarfMenuItem);
    CC_SAFE_RELEASE(eyeMenuItem);
}

bool SnowmanGameScene::init()
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // load plist
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("eyes.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("hats.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("mittens.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("mouths.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("scarves.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("noses.plist");
    
    // backgournd
    CCSprite *background = CCSprite::spriteWithFile("snowman-game-bg.jpg");
    background->setPosition(ccp(winSize.width/2, winSize.height/2));
    addChild(background);
    
    // arrow menu & add sprite between arrows
    addArrowMenuAndAddSprite();
    
    // snow man parts
    addSnowmanPartsMenu();
    
    // facebook twitter
    addShareMenu();
    
    // logo
    CCSprite *logo = CCSprite::spriteWithFile("game-logo.png");
    logo->setPosition(LOGO_POSITION);
    addChild(logo);
    
    // home menu
    CCMenuItem *mainMenuItem = CCMenuItemImage::itemFromNormalImage("home-button.png", 
                                                                    "home-button-over.png",
                                                                    this,
                                                                    menu_selector(SnowmanGameScene::homeMenuTouched));
    mainMenuItem->setPosition(HOME_MENU_POSITION);
    
    CCMenu *mainMenu = CCMenu::menuWithItems(mainMenuItem, NULL);
    mainMenu->setPosition(CCPointZero);
    addChild(mainMenu);
    
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 0, true);
    
    return true;
}

void SnowmanGameScene::addShareMenu()
{
    CCMenuItemSprite *facebookItem = CCMenuItemSprite::itemFromNormalSprite(CCSprite::spriteWithFile("facebook.png"), 
                                                                            CCSprite::spriteWithFile("facebook-hover.png"),
                                                                            this,
                                                                            menu_selector(SnowmanGameScene::shareMenuTouched));
    facebookItem->setTag(FACEBOOK_TAG);
    
    CCMenuItemSprite *twitterItem = CCMenuItemSprite::itemFromNormalSprite(CCSprite::spriteWithFile("twitter.png"),
                                                                           CCSprite::spriteWithFile("twitter-hover.png"),
                                                                           this,
                                                                           menu_selector(SnowmanGameScene::shareMenuTouched));
    twitterItem->setTag(TWITTER_TAG);
    
    CCMenu *shareMenu = CCMenu::menuWithItems(facebookItem, twitterItem, NULL);
    shareMenu->alignItemsHorizontallyWithPadding(10.0f);
    shareMenu->setPosition(SHARE_MENU_POSITION);
    twitterItem->setPosition(ccp(15, 0));
    
    addChild(shareMenu);
}

void SnowmanGameScene::shareMenuTouched(cocos2d::CCObject *sender)
{
    
}

void SnowmanGameScene::addArrowMenuAndAddSprite()
{
    CCMenuItemImage *leftArrow = CCMenuItemImage::itemFromNormalImage("arrow.png",
                                                                      "arrow-hover.png",
                                                                      this,
                                                                      menu_selector(SnowmanGameScene::arrowTouched));
    leftArrow->setRotation(180);
    leftArrow->setTag(LEFT_ARROW_SPRITE_TAG);
    
    CCMenuItemImage *rightArrow = CCMenuItemImage::itemFromNormalImage("arrow.png", 
                                                                       "arrow-hover.png",
                                                                       this,
                                                                       menu_selector(SnowmanGameScene::arrowTouched));
    rightArrow->setTag(RIGHT_ARROW_SPRITE_TAG);
    
    CCMenu *arrowMenu = CCMenu::menuWithItems(leftArrow, rightArrow, NULL);
    arrowMenu->alignItemsHorizontally();
    arrowMenu->setPosition(ARROW_MENU_POSITION);
    leftArrow->setPosition(LEFT_ARROW_POSITION);
    addChild(arrowMenu);
    
    // add default sprite between arrows
    CCSprite *hat = CCSprite::spriteWithSpriteFrameName("hats00.png");
    
    SpriteBetweenArrowData *data = new SpriteBetweenArrowData();
    data->index = 0;
    data->tag = HAT_TAG;
    data->scale = HAT_SCALE;
    hat->setUserData(data);
    
    hat->setPosition(SPRITE_BETWEEN_ARROW_POSITION);
    hat->setScale(HAT_SCALE);
    hat->setTag(SPRITE_BETWEEN_ARROW_TAG);
    
    addChild(hat);
}

void SnowmanGameScene::addSnowmanPartsMenu()
{
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("menu.plist");
    CCSprite *hatButtonSprite = CCSprite::spriteWithSpriteFrameName("smg-menu-hat.png");
    CCSprite *hatButtonSpriteOver = CCSprite::spriteWithSpriteFrameName("smg-menu-hats-over.png");
    
    CCSprite *scarfButtonSprite = CCSprite::spriteWithSpriteFrameName("smg-menu-scarf.png");
    CCSprite *scarfButtonSpriteOver = CCSprite::spriteWithSpriteFrameName("smg-menu-scarf-over.png");
    
    CCSprite *mittenButtonSprite = CCSprite::spriteWithSpriteFrameName("smg-menu-mittens.png");
    CCSprite *mittenButtonSpriteOver = CCSprite::spriteWithSpriteFrameName("smg-menu-mittens-over.png");
    
    CCSprite *eyeButtonSprite = CCSprite::spriteWithSpriteFrameName("smg-menu-eyes.png");
    CCSprite *eyeButtonSpriteOver = CCSprite::spriteWithSpriteFrameName("smg-menu-eyes-over.png");
    
    CCSprite *noseButtonSprite = CCSprite::spriteWithSpriteFrameName("smg-menu-nose.png");
    CCSprite *noseButtonSpriteOver = CCSprite::spriteWithSpriteFrameName("smg-menu-nose-over.png");
    
    CCSprite *mouthButtonSprite = CCSprite::spriteWithSpriteFrameName("smg-menu-mouth.png");
    CCSprite *mouthButtonSpriteOver = CCSprite::spriteWithSpriteFrameName("smg-menu-mouth-over.png");
    
    hatMenuItem = CCMenuItemSprite::itemFromNormalSprite(hatButtonSprite,
                                                         hatButtonSpriteOver,
                                                         this,
                                                         menu_selector(SnowmanGameScene::snowmanPartsMenuTouched));
    hatMenuItem->setTag(HAT_TAG);
    hatMenuItem->retain();
    
    scarfMenuItem = CCMenuItemSprite::itemFromNormalSprite(scarfButtonSprite,
                                                            scarfButtonSpriteOver,
                                                            this,
                                                            menu_selector(SnowmanGameScene::snowmanPartsMenuTouched));
    scarfMenuItem->setTag(SCARF_TAG);
    scarfMenuItem->retain();
    
    mittenMenuItem = CCMenuItemSprite::itemFromNormalSprite(mittenButtonSprite,
                                                            mittenButtonSpriteOver,
                                                            this,
                                                            menu_selector(SnowmanGameScene::snowmanPartsMenuTouched));
    mittenMenuItem->setTag(MITTEN_TAG);
    mittenMenuItem->retain();
    
    eyeMenuItem = CCMenuItemSprite::itemFromNormalSprite(eyeButtonSprite,
                                                         eyeButtonSpriteOver,
                                                         this,
                                                         menu_selector(SnowmanGameScene::snowmanPartsMenuTouched));
    eyeMenuItem->setTag(EYE_TAG);
    eyeMenuItem->retain();
    
    noseMenuItem = CCMenuItemSprite::itemFromNormalSprite(noseButtonSprite,
                                                          noseButtonSpriteOver,
                                                          this,
                                                          menu_selector(SnowmanGameScene::snowmanPartsMenuTouched));
    noseMenuItem->setTag(NOSE_TAG);
    noseMenuItem->retain();
    
    mouthMenuItem = CCMenuItemSprite::itemFromNormalSprite(mouthButtonSprite,
                                                           mouthButtonSpriteOver,
                                                           this,
                                                           menu_selector(SnowmanGameScene::snowmanPartsMenuTouched));
    mouthMenuItem->setTag(MOUTH_TAG);
    mouthMenuItem->retain();
    
    CCMenu *category = CCMenu::menuWithItems(hatMenuItem, scarfMenuItem, mittenMenuItem, NULL);
    CCMenu *categoryLower = CCMenu::menuWithItems(eyeMenuItem, noseMenuItem, mouthMenuItem, NULL);
    
    category->alignItemsHorizontally();
    categoryLower->alignItemsHorizontally();
    category->setScale(0.68);
    categoryLower->setScale(0.68);
    
    category->setPosition(CATEGORY_POSITION);
    categoryLower->setPosition(CATEGORY_LOWER_POSITION);
    
    addChild(category);
    addChild(categoryLower);
}

bool SnowmanGameScene::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCPoint location = pTouch->locationInView(pTouch->view());
    location = CCDirector::sharedDirector()->convertToGL(location);
    
    CCSprite *spriteBetweenArrow = (CCSprite*)getChildByTag(SPRITE_BETWEEN_ARROW_TAG);
    SpriteBetweenArrowData *data = (SpriteBetweenArrowData*)spriteBetweenArrow->getUserData();
    
    if (CCRect::CCRectContainsPoint(spriteBetweenArrow->boundingBox(), location))
    {
        CCSprite *spriteToDrag = NULL;
        int curIndex = data->index;
        char spriteToDragFrameName[20];
        switch (data->tag) {
            case HAT_TAG:
                sprintf(spriteToDragFrameName, "hats0%d.png", curIndex);
                break;
            case EYE_TAG:
                sprintf(spriteToDragFrameName, "eyes0%d.png", curIndex);
                break;
            case NOSE_TAG:
                sprintf(spriteToDragFrameName, "nose0%d.png", curIndex);
                break;
            case SCARF_TAG:
                sprintf(spriteToDragFrameName, "scarf0%d.png", curIndex);
                break;
            case MITTEN_TAG:
                sprintf(spriteToDragFrameName, "mittens0%d.png", curIndex);
                break;
            case MOUTH_TAG:
                sprintf(spriteToDragFrameName, "mouth0%d.png", curIndex);
                break;
                
            default:
                CCAssert(false, "error tag");
                break;
        }
        
        spriteToDrag = CCSprite::spriteWithSpriteFrameName(spriteToDragFrameName);
        spriteToDrag->setScale(0.5);
        spriteToDrag->setPosition(SPRITE_BETWEEN_ARROW_POSITION);
        spriteToDrag->setTag(SPRITE_TO_DRAG_TAG);
        
        addChild(spriteToDrag);
        
        return true;
    }
    
    return false;
}

void SnowmanGameScene::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCSprite *spriteToDrag = (CCSprite*)getChildByTag(SPRITE_TO_DRAG_TAG);
    if (spriteToDrag)
    {
        CCPoint location = pTouch->locationInView(pTouch->view());
        location = CCDirector::sharedDirector()->convertToGL(location);
        spriteToDrag->setPosition(location);
    }
}

void SnowmanGameScene::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    removeChildByTag(SPRITE_TO_DRAG_TAG, true);
    
    // draw snow man part
    CCSprite *spriteBetweenArrow = (CCSprite*)getChildByTag(SPRITE_BETWEEN_ARROW_TAG);
    SpriteBetweenArrowData *data = (SpriteBetweenArrowData*)spriteBetweenArrow->getUserData();
    CCSprite *spriteToDraw = NULL;
    int curIndex = data->index;
    CCPoint pointToDraw;
    char spriteToDrawFrameName[20];
    int spriteToDrawTag;
    switch (data->tag) {
        case HAT_TAG:
            sprintf(spriteToDrawFrameName, "hats0%d.png", curIndex);
            pointToDraw = HAT_POSITION;
            spriteToDrawTag = SNOWMAN_HAT_TAG;
            break;
        case EYE_TAG:
            sprintf(spriteToDrawFrameName, "eyes0%d.png", curIndex);
            pointToDraw = EYE_POSITION;
            spriteToDrawTag = SNOWMAN_EYE_TAG;
            break;
        case NOSE_TAG:
            sprintf(spriteToDrawFrameName, "nose0%d.png", curIndex);
            pointToDraw = NOSE_POSITION;
            spriteToDrawTag = SNOWMAN_NOSE_TAG;
            break;
        case SCARF_TAG:
            sprintf(spriteToDrawFrameName, "scarf0%d.png", curIndex);
            pointToDraw = SCARF_POSITION;
            spriteToDrawTag = SNOWMAN_SCARF_TAG;
            break;
        case MITTEN_TAG:
            sprintf(spriteToDrawFrameName, "mittens0%d.png", curIndex);
            pointToDraw = MITTEN_POSITION;
            spriteToDrawTag = SNOWMAN_MITTEN_TAG;
            break;
        case MOUTH_TAG:
            sprintf(spriteToDrawFrameName, "mouth0%d.png", curIndex);
            pointToDraw = MOUTH_POSITION;
            spriteToDrawTag = SNOWMAN_MOUTH_TAG;
            break;
            
        default:
            CCAssert(false, "error tag");
            break;
    }
    
    spriteToDraw = CCSprite::spriteWithSpriteFrameName(spriteToDrawFrameName);
    spriteToDraw->setPosition(pointToDraw);
    spriteToDraw->setTag(SPRITE_TO_DRAG_TAG);
    spriteToDraw->setTag(spriteToDrawTag);
    
    // remove previous snow man parg
    removeChildByTag(spriteToDrawTag, true);
    
    addChild(spriteToDraw);
    
}

void SnowmanGameScene::arrowTouched(cocos2d::CCObject *sender)
{
    CCSprite *currentSpriteBetweenArrow = (CCSprite*)getChildByTag(SPRITE_BETWEEN_ARROW_TAG);
    SpriteBetweenArrowData *data = (SpriteBetweenArrowData*)currentSpriteBetweenArrow->getUserData();
    int index = 0;
    int tag = data->tag;
    
    CCMenuItemImage *menuItemImage = (CCMenuItemImage*)sender;
    int menuItemImageTag = menuItemImage->getTag();
    if (menuItemImageTag == LEFT_ARROW_SPRITE_TAG)
    {
        index = previousIndex(data->index);
    }
    else if (menuItemImageTag == RIGHT_ARROW_SPRITE_TAG)
    {
        index = nextIndex(data->index);
    }
    else 
    {
        CCAssert(false, "error menu item image tag");
    }
    
    char nextSpriteString[20];
    
    switch (tag) {
        case HAT_TAG:
            sprintf(nextSpriteString, "hats0%d.png", index);
            break;
        case NOSE_TAG:
            sprintf(nextSpriteString, "nose0%d.png", index);
            break;
        case EYE_TAG:
            sprintf(nextSpriteString, "eyes0%d.png", index);
            break;
        case SCARF_TAG:
            sprintf(nextSpriteString, "scarf0%d.png", index);
            break;
        case MITTEN_TAG:
            sprintf(nextSpriteString, "mittens0%d.png", index);
            break;
        case MOUTH_TAG:
            sprintf(nextSpriteString, "mouth0%d.png", index);
            break;
            
        default:
            CCAssert(false, "error tag");
            break;
    }
    
    // add new sprite
    CCSprite *nextSprite = CCSprite::spriteWithSpriteFrameName(nextSpriteString);
    SpriteBetweenArrowData *tmpData = new SpriteBetweenArrowData();
    tmpData->index = index;
    tmpData->tag = tag;
    tmpData->scale = data->scale;
    nextSprite->setUserData(tmpData);
    nextSprite->setTag(SPRITE_BETWEEN_ARROW_TAG);
    nextSprite->setScale(tmpData->scale);
    if (tag == MITTEN_TAG)
    {
        nextSprite->setPosition(SCARF_BETWEEN_ARROW_POSITION);
    }
    else 
    {
        nextSprite->setPosition(SPRITE_BETWEEN_ARROW_POSITION);
    }
    
    // remove previous sprite
    delete((SpriteBetweenArrowData*)data);
    removeChildByTag(SPRITE_BETWEEN_ARROW_TAG, true);
    
    addChild(nextSprite);
}

int SnowmanGameScene::nextIndex(int curIndex)
{
     return  (curIndex+1) % NUMBER_OF_SELECTION;
}

int SnowmanGameScene::previousIndex(int curIndex)
{
    int ret = 0;
    if (curIndex == 0)
    {
        ret = NUMBER_OF_SELECTION - 1;
    }
    else 
    {
        ret = (curIndex - 1) % NUMBER_OF_SELECTION;
    }
    
    return ret;
}

void SnowmanGameScene::snowmanPartsMenuTouched(cocos2d::CCObject *sender)
{
    // set all menu items to default normal sprite
    noseMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-nose.png"));
    eyeMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-eyes.png"));
    hatMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-hat.png"));
    mittenMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-mittens.png"));
    mouthMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-mouth.png"));
    scarfMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-scarf.png"));
    
    CCSprite *spriteBetweenArrow = NULL;
    CCMenuItemSprite *selectedMenuItem = (CCMenuItemSprite*)sender;
    struct SpriteBetweenArrowData *data = new SpriteBetweenArrowData();
    int tag = selectedMenuItem->getTag();
    float scale = 1.0f;
    switch (tag) {
        case NOSE_TAG:
            noseMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-nose-over.png"));
            spriteBetweenArrow = CCSprite::spriteWithSpriteFrameName("nose00.png");
            break;
        case EYE_TAG:
            eyeMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-eyes-over.png"));
            spriteBetweenArrow = CCSprite::spriteWithSpriteFrameName("eyes00.png");
            break;
        case HAT_TAG:
            hatMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-hats-over.png"));
            spriteBetweenArrow = CCSprite::spriteWithSpriteFrameName("hats00.png");
            scale = HAT_SCALE;
            break;
        case MITTEN_TAG:
            mittenMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-mittens-over.png"));
            spriteBetweenArrow = CCSprite::spriteWithSpriteFrameName("mittens00.png");
            break;
        case MOUTH_TAG:
            mouthMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-mouth-over.png"));
            spriteBetweenArrow = CCSprite::spriteWithSpriteFrameName("mouth00.png");
            break;
        case SCARF_TAG:
            scarfMenuItem->setNormalImage(CCSprite::spriteWithSpriteFrameName("smg-menu-scarf-over.png"));
            spriteBetweenArrow = CCSprite::spriteWithSpriteFrameName("scarf00.png");
            scale = SCARF_SCALE;
            break;
            
        default:
            CCAssert(false, "error tag");
            break;
    }
    if (tag == MITTEN_TAG)
    {
        spriteBetweenArrow->setPosition(SCARF_BETWEEN_ARROW_POSITION);
    }
    else 
    {
        spriteBetweenArrow->setPosition(SPRITE_BETWEEN_ARROW_POSITION);
    }    
    
    // remove previous sprite first
    CCSprite *sprite = (CCSprite*)getChildByTag(SPRITE_BETWEEN_ARROW_TAG);
    delete((SpriteBetweenArrowData*)sprite->getUserData());
    removeChildByTag(SPRITE_BETWEEN_ARROW_TAG, true);
    
    // set data
    
    data->tag = tag;
    data->index = 0;
    data->scale = scale;
    spriteBetweenArrow->setUserData(data);
    spriteBetweenArrow->setTag(SPRITE_BETWEEN_ARROW_TAG);
    spriteBetweenArrow->setScale(scale);
    
    addChild(spriteBetweenArrow);
}

void SnowmanGameScene::homeMenuTouched(cocos2d::CCObject *sender)
{
    // popup a dialog to select
    // should release previous dialog if it is exist
    CC_SAFE_RELEASE_NULL(mydialog);
    
    // pop a dialog to select 
    vector<string> items;
    items.push_back("Yes");
    items.push_back("No");
    
    mydialog = new MyDialog();
    mydialog->initWithItems("Go to main menu?", items, this);
    mydialog->popUp();  
}

void SnowmanGameScene::buttonClicked(int index)
{
    if (index == 0)
    {
        PageManager::gotoMainMenu();
    }
}

#include "SnowmanGameScene.h"
#include "SharedGlobalData.h"
#include "PageManager.h"
#include "Share.h"

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

// position for menu
#define CATEGORY_POSITION       ccp(650*XSCALE, 430*YSCALE)
#define CATEGORY_LOWER_POSITION ccp(650*XSCALE, 340*YSCALE)

// position for info icon
#define INFO_ICON_POSITION ccp(50*XSCALE, 715*YSCALE)

#define HAT_TAG     1
#define EYE_TAG     2
#define MITTEN_TAG  3
#define MOUTH_TAG   4
#define SCARF_TAG   5
#define NOSE_TAG    6

#define SPRITE_BETWEEN_ARROW_TAG       7

#define LEFT_ARROW_SPRITE_TAG   8
#define RIGHT_ARROW_SPRITE_TAG  9
#define ARROW_MENU_TAG          10

#define SPRITE_TO_DRAG_TAG      11

#define SNOWMAN_HAT_TAG         12
#define SNOWMAN_EYE_TAG         13
#define SNOWMAN_MITTEN_TAG      14
#define SNOWMAN_MOUTH_TAG       15
#define SNOWMAN_SCARF_TAG       16
#define SNOWMAN_NOSE_TAG        17
#define CATEGORY_TAG            18
#define LOWER_CATEGORY_TAG      19

#define FACEBOOK_TAG            20
#define TWITTER_TAG             21
#define SHARE_MENU_TAG          22

#define HOME_MENU_TAG           23
#define HOME_MENU_ITEM_TAG      24

#define INFO_MENU_TAG           25
#define INFO_MENU_ITEM_TAG      26

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
  
  
  
    // info menu
    CCMenuItem *infoMenuItem = CCMenuItemImage::itemFromNormalImage("info-icon.png",
                                                                    "info-icon.png",
                                                                    this,
                                                                    menu_selector(SnowmanGameScene::infoMenuTouched));
    infoMenuItem->setPosition(INFO_ICON_POSITION);
    infoMenuItem->setTag(INFO_MENU_ITEM_TAG);
    CCMenu *infoMenu = CCMenu::menuWithItems(infoMenuItem, NULL);
    infoMenu->setTag(INFO_MENU_TAG);
    infoMenu->setPosition(CCPointZero);
    addChild(infoMenu);
  
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
    CCMenuItem *homeMenuItem = CCMenuItemImage::itemFromNormalImage("home-button.png",
                                                                    "home-button-over.png",
                                                                    this,
                                                                    menu_selector(SnowmanGameScene::homeMenuTouched));
    homeMenuItem->setPosition(HOME_MENU_POSITION);
    homeMenuItem->setTag(HOME_MENU_ITEM_TAG);
    
    CCMenu *homeMenu = CCMenu::menuWithItems(homeMenuItem, NULL);
    homeMenu->setPosition(CCPointZero);
    homeMenu->setTag(HOME_MENU_TAG);
    addChild(homeMenu);
    
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
    shareMenu->setTag(SHARE_MENU_TAG);
    twitterItem->setPosition(ccp(15, 0));
    
    addChild(shareMenu);
}

void SnowmanGameScene::shareMenuTouched(cocos2d::CCObject *sender)
{
    // hide categories, items, arrows, and sharing icons before capturing image
    showSprites(false);
    
    CCDirector::sharedDirector()->myDrawScene();
    
    CCMenuItemSprite *menuItemSprite = (CCMenuItemSprite*)sender;
    int tag = menuItemSprite->getTag();
    
    if (FACEBOOK_TAG == tag)
    {
        shareOnFaceBook();
    }
    
    if (TWITTER_TAG == tag)
    {
        shareOnTwitter();
    }
    
    showSprites(true);
}

void SnowmanGameScene::showSprites(bool isShow)
{
    // hide or show arrows
    CCMenu *arrowMenu = (CCMenu*)getChildByTag(ARROW_MENU_TAG);
    CCSprite *leftArrow = (CCSprite*)arrowMenu->getChildByTag(LEFT_ARROW_SPRITE_TAG);
    leftArrow->setIsVisible(isShow);
    
    CCSprite *rightAroow = (CCSprite*)arrowMenu->getChildByTag(RIGHT_ARROW_SPRITE_TAG);
    rightAroow->setIsVisible(isShow);
    
    // hide or show categories
    CCMenu *categoryMenu = (CCMenu*)getChildByTag(CATEGORY_TAG);
    CCSprite *hatCategory = (CCSprite*)categoryMenu->getChildByTag(HAT_TAG);
    hatCategory->setIsVisible(isShow);
    
    CCSprite *scarfCategory = (CCSprite*)categoryMenu->getChildByTag(SCARF_TAG);
    scarfCategory->setIsVisible(isShow);
    
    CCSprite *mittenCategory = (CCSprite*)categoryMenu->getChildByTag(MITTEN_TAG);
    mittenCategory->setIsVisible(isShow);
    
    CCMenu *lowerCategoryMenu = (CCMenu*)getChildByTag(LOWER_CATEGORY_TAG);
    CCSprite *eyeCategory = (CCSprite*)lowerCategoryMenu->getChildByTag(EYE_TAG);
    eyeCategory->setIsVisible(isShow);
    
    CCSprite *noseCategory = (CCSprite*)lowerCategoryMenu->getChildByTag(NOSE_TAG);
    noseCategory->setIsVisible(isShow);
    
    CCSprite *mouthCategory = (CCSprite*)lowerCategoryMenu->getChildByTag(MOUTH_TAG);
    mouthCategory->setIsVisible(isShow);
    
    // hide or show sharing icons
    CCMenu *shareMenu = (CCMenu*)getChildByTag(SHARE_MENU_TAG);
    CCSprite *facebookIcon = (CCSprite*)shareMenu->getChildByTag(FACEBOOK_TAG);
    facebookIcon->setIsVisible(isShow);
    
    CCSprite *twitterIcon = (CCSprite*)shareMenu->getChildByTag(TWITTER_TAG);
    twitterIcon->setIsVisible(isShow);
    
    // hide or show item
    CCSprite *item = (CCSprite*)getChildByTag(SPRITE_BETWEEN_ARROW_TAG);
    item->setIsVisible(isShow);
    
    // hide or show home menu
    CCMenu *homeMenu = (CCMenu*)getChildByTag(HOME_MENU_TAG);
    CCMenuItem *homeMenuItem = (CCMenuItem*)homeMenu->getChildByTag(HOME_MENU_ITEM_TAG);
    homeMenuItem->setIsVisible(isShow);
    
    // hide or show info menu
    CCMenu *infoMenu = (CCMenu*)getChildByTag(INFO_MENU_TAG);
    CCMenuItem *infoMenuItem = (CCMenuItem*)infoMenu->getChildByTag(INFO_MENU_ITEM_TAG);
    infoMenuItem->setIsVisible(isShow);
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
    arrowMenu->setTag(ARROW_MENU_TAG);
    leftArrow->setPosition(LEFT_ARROW_POSITION);

    addChild(arrowMenu);
    
    // add default sprite between arrows
    CCSprite *hat = CCSprite::spriteWithSpriteFrameName("hats00.png");
    
    SpriteBetweenArrowData *data = new SpriteBetweenArrowData();
    data->index = 0;
    data->tag = HAT_TAG;
    data->scale = HAT_SCALE * MIN_SCALE;
    hat->setUserData(data);
    
    hat->setPosition(SPRITE_BETWEEN_ARROW_POSITION);
    hat->setScale(HAT_SCALE * MIN_SCALE);
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
    
    category->setTag(CATEGORY_TAG);
    categoryLower->setTag(LOWER_CATEGORY_TAG);
    
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
        spriteToDrag->setScale(0.5 * MIN_SCALE);
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
    data->scale = scale * MIN_SCALE;
    spriteBetweenArrow->setUserData(data);
    spriteBetweenArrow->setTag(SPRITE_BETWEEN_ARROW_TAG);
    spriteBetweenArrow->setScale(data->scale);
    
    addChild(spriteBetweenArrow);
}

void SnowmanGameScene::homeMenuTouched(cocos2d::CCObject *sender)
{
    isHomeMenuTouched = true;
    
    // pop a dialog to select 
    vector<string> items;
    items.push_back("Yes");
    items.push_back("No");
    
    homeMenuDialog = new MyDialog();
    homeMenuDialog->initWithItems("Go to main menu?", items, this);
    homeMenuDialog->popUp();  
}

void SnowmanGameScene::infoMenuTouched(cocos2d::CCObject *sender)
{
    isInfoMenuTouched = true;
    
    // pop a dialog to select
    vector<string> items;
    items.push_back("OK");
    
    infoMenuDialog = new MyDialog();
    infoMenuDialog->initWithItems("stranger in the wood", items, this);
    infoMenuDialog->popUp();
}

void SnowmanGameScene::buttonClicked(int index)
{
    if (isHomeMenuTouched)
    {
        if (index == 0)
        {
            PageManager::gotoMainMenu();
        }
        
        // release the dialog
        homeMenuDialog->release();
        
        isHomeMenuTouched = false;
    }
    
    if (isInfoMenuTouched)
    {
        infoMenuDialog->release();
        isInfoMenuTouched = false;
    }
}

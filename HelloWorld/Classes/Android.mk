LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_logic_static

LOCAL_MODULE_FILENAME := libgame_logic

LOCAL_SRC_FILES := AppDelegate.cpp \
                   ChipmunkLayer.cpp \
                   Configurations.cpp \
                   cpSprite.cpp \
                   CreditsLayer.cpp \
                   DataLoader.cpp \
                   GCpShapeCache.cpp \
                   JsonParser.cpp \
                   MainMenu.cpp \
                   MainMenuLayer.cpp \
                   MyDialog.cpp \
                   MyPageTurn.cpp \
                   MyScene.cpp \
                   OpenUrlHelper.cpp \
                   Page.cpp \
                   PageLayer.cpp \
                   PageManager.cpp \
                   Share.cpp \
                   SharedGlobalData.cpp \
                   SnowmanGameScene.cpp \
                   TouchDetection.cpp \
                   TouchObject.cpp  \
                   VideoPlayer.cpp  \
                   Utils.cpp      
                   
                   
                   
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

LOCAL_C_INCLUDES := ../.. \
../../cocos2dx/effects

LOCAL_STATIC_LIBRARIES := png_static_prebuilt
LOCAL_STATIC_LIBRARIES += xml2_static_prebuilt
LOCAL_STATIC_LIBRARIES += jpeg_static_prebuilt
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
                
LOCAL_SHARED_LIBRARIES := cocosdenshion_shared chipmunk_shared
            
include $(BUILD_STATIC_LIBRARY)

$(call import-module,cocos2dx/platform/third_party/android/modules/libpng)
$(call import-module,cocos2dx/platform/third_party/android/modules/libxml2)
$(call import-module,cocos2dx/platform/third_party/android/modules/libjpeg)

APP_STL := gnustl_static
APP_CPPFLAGS += -frtti
# json use exception
APP_CPPFLAGS += -fexceptions

APP_MODULES := cocos2dx_static chipmunk_shared cocosdenshion_shared game_logic_static helloworld_shared
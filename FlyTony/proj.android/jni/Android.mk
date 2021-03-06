LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/FakeSplash.cpp \
                   ../../Classes/GameScene.cpp \
                   ../../Classes/Obstacle.cpp \
                   ../../Classes/VisibleRect.cpp \
                   ../../Classes/AdMobHelper/AdmobHelper.cpp \
                   ../../Classes/AnalyticX/AnalyticX.cpp \
                   ../../Classes/AnalyticX/AnalyticXStringUtilAndroid.cpp \
                   ../../Classes/ChartboostX/ChartboostX.cpp \
                   ../../Classes/ChartboostX/ChartboostXJni.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../Classes/AdMobHelper \
					$(LOCAL_PATH)/../../Classes/AnalyticX \
					$(LOCAL_PATH)/../../Classes/ChartboostX \
					$(LOCAL_PATH)/../..//cocos2d/cocos/base

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
# $(call import-module,editor-support/cocostudio)
# $(call import-module,network)
# $(call import-module,extensions)

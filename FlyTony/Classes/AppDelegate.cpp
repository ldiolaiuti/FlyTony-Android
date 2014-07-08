#include "AppDelegate.h"
#include "FakeSplash.h"
#include "SimpleAudioEngine.h"
#include "AdMobHelper/AdmobHelper.h"

USING_NS_CC;

// The app key of flurry
#define FLURRY_KEY_IOS          "36DNQ9MNW36CYV7ZKSY8"
#define FLURRY_KEY_ANDROID      "4T3BMRNBYQDZNZF88NST"



AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    
    auto pDirector = Director::getInstance();
    auto pEGLView = pDirector->getOpenGLView();
    if(!pEGLView) {
        pEGLView = GLView::create("Fly Tony");
        pDirector->setOpenGLView(pEGLView);
    }
	
    pEGLView->setDesignResolutionSize(640, 960, ResolutionPolicy::NO_BORDER);

    // turn on display FPS
    pDirector->setDisplayStats(false);

    Size screenSize = pEGLView->getFrameSize();

       std::vector<std::string> searchPath;

       if (screenSize.width > 640)
       {
           searchPath.push_back("hd");
           pDirector->setContentScaleFactor(2);
       }
       else
       {
           searchPath.push_back("sd");
           pDirector->setContentScaleFactor(1);
       }

    FileUtils::getInstance()->setSearchPaths(searchPath);
    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    Scene *pScene = FakeSplash::scene();


    //AdmobHelper::showAd();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    //AdmobHelper::hideAd();

    // if you use SimpleAudioEngine, it must be pause
     CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::getInstance()->startAnimation();

    //AdmobHelper::showAd();
    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}


void AppDelegate::loadPlugins(){
}

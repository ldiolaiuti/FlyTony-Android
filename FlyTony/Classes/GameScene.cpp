//
//  GameScene.cpp
//  TonyFlyingDream
//
//  Created by Luca Diolaiuti on 08/07/14.
//
//

#include "GameScene.h"
#include "VisibleRect.h"
#include "Obstacle.h"
#include "SimpleAudioEngine.h"
#include "AdmobHelper.h"
#include "AnalyticX.h"
#include <string>
#include <sstream>

#define PTM_RATIO 32
#define GRAVITY 2500
#define DEFAULT_ROTATION_UP 20
#define DEFAULT_ROTATION_DOWN 90
#define FLOATING_GRAVITY_OPPOSITE GRAVITY - 5
#define FORCE_Y 650
#define FIXED_TIME_STEP 1/60

using namespace CocosDenshion;

enum {
	kTagNewRecordLabel,
};

static const float firstObstaclePosition = 400;
static const float distanceBetweenObstacles = 350.f;

static const char s_FontText[] = "fonts/text.fnt";
static const char s_FontScore[] = "fonts/score.fnt";
static const char s_FontScoreBig[] = "fonts/ScoreBig.fnt";

typedef enum {
	LayerDrawingBackground, LayerDrawingMidground, LayerDrawingForeground,

} LayerDrawing;

typedef enum {
	DrawingOrderGround, DrawingOrderClouds, DrawingOrderPipes, DrawingOrderHero,

} DrawingOrder;

GameScene::GameScene() {

}

GameScene::~GameScene() {
	delete _grounds;
	delete _clouds;
	delete _obstacles;
	delete _wheels;
	CC_SAFE_RELEASE(_tonyAnimation);
	CC_SAFE_RELEASE(_cloudBtnAnimation);

}

Scene* GameScene::scene() {
	Scene* scene = Scene::create();

	GameScene* layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init() {
	if (!CCLayer::init()) {
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
			listener, this);

	auto keyListener = EventListenerKeyboard::create();
	keyListener->setEnabled(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
			keyListener, this);
	this->setKeypadEnabled(true);

	int indexOfSpriteSheet = (CC_CONTENT_SCALE_FACTOR() == 2) ? 2 : 1;

	for (int i = 0; i < indexOfSpriteSheet; i++) {
		ostringstream s1;
		s1 << "GameAtlas" << i << ".plist";
		ostringstream s2;
		s2 << "GameAtlas" << i << ".png";
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(s1.str(),
				s2.str());
	}
	_screenSize = Director::getInstance()->getWinSize();
	_scrollSpeedBackground = 200.f;
	_scrollSpeedMidGround = 40;
	_scrollSpeedForeground = _scrollSpeedBackground;
	_yVel = 0;
	_isTap = false;
	_isGameOver = false;
	_isNewRecord = false;
	_timeWaitLabel = 0;
	_maxTimeWaitLabel = 1;
	_idSoundNewGroupScore = 10;

	//Aggiungo gli ostacoli
	_obstacles = new Vector<Node*>;
//    _obstacles->retain();
//    
	_clouds = new Vector<Ref*>;
//    _clouds->retain();
//    
	_grounds = new Vector<Ref*>();
//   _grounds->retain();
//    
	_wheels = new Vector<Ref*>;
//    _wheels->retain();

	Sprite *fxBackground = Sprite::create("background.png");
	fxBackground->setAnchorPoint(Point(0, 0));
	fxBackground->setPosition(Point(0, 0));
	this->addChild(fxBackground);

	_background = CCNode::create();
	_background->setContentSize(CCDirector::getInstance()->getVisibleSize());
	_background->setAnchorPoint(Point(0, 0));
	this->addChild(_background, LayerDrawingBackground);

	_midground = CCNode::create();
	_midground->setContentSize(CCDirector::getInstance()->getVisibleSize());
	_midground->setAnchorPoint(Point(0, 0));
	this->addChild(_midground, LayerDrawingMidground);

	_foreground = CCNode::create();
	_foreground->setContentSize(CCDirector::getInstance()->getVisibleSize());
	_foreground->setAnchorPoint(Point(0, 0));
	this->addChild(_foreground, LayerDrawingForeground);

	//Animazione dell'uccellino
	_tonyAnimation = CCAnimation::create();
	_tonyAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"tony_0001"));
	_tonyAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"tony_0002"));
	_tonyAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"tony_0003"));
	_tonyAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"tony_0004"));
	_tonyAnimation->setDelayPerUnit(0.08f);

	_tonyAnimation->retain();

	_cloudBtnAnimation = CCAnimation::create();
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0002"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0003"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0004"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0005"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0006"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0007"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0008"));
	_cloudBtnAnimation->addSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName(
					"cloudTap_0009"));

	_cloudBtnAnimation->setDelayPerUnit(0.08f);

	_cloudBtnAnimation->setRestoreOriginalFrame(true);

	_cloudBtnAnimation->retain();

	_layerColorWait = LayerColor::create(Color4B(9, 13, 73, 102),
			_screenSize.width, _screenSize.height);
	_layerColorWait->setAnchorPoint(Point(0, 0));
	this->addChild(_layerColorWait, LayerDrawingForeground + 1);

	this->showMainMenu();

	_recordRays = CCSprite::createWithSpriteFrameName("recordRays");
	_recordRays->setPosition(
			Point(_screenSize.width / 2, _screenSize.height - 50));
	this->addChild(_recordRays, LayerDrawingForeground);
	_recordRays->setOpacity(0);

	_newRecordLabel = Label::createWithBMFont(s_FontText, "New Record!");
	_newRecordLabel->setPosition(
			Point(_screenSize.width / 2,
					_screenSize.height
							+ _newRecordLabel->getContentSize().height * 2));
	this->addChild(_newRecordLabel, LayerDrawingForeground, kTagNewRecordLabel);

	this->drawLayout();

	return true;
}

void GameScene::showMainMenu() {
	_layerColorWait->removeAllChildrenWithCleanup(true);
	MenuItemImage* btn = MenuItemImage::create();
	btn->setNormalSpriteFrame(
			SpriteFrameCache::getInstance()->getSpriteFrameByName("cloud"));
	btn->setSelectedSpriteFrame(
			SpriteFrameCache::getInstance()->getSpriteFrameByName("cloud"));
	btn->setCallback(CC_CALLBACK_1(GameScene::flyTony, this)); //->setTarget(this, menu_selector(GameScene::flyTony));

	Label* flyLabel = Label::createWithBMFont(s_FontText, "Fly");
	flyLabel->setPosition(
			Point(btn->getContentSize().width / 2,
					btn->getContentSize().height / 2 - 6));
	flyLabel->setScale(1.5);
	btn->addChild(flyLabel);

	Menu* menu = CCMenu::create(btn, NULL);
	menu->setPosition(
			Point(_layerColorWait->getContentSize().width / 2,
					_layerColorWait->getContentSize().height / 2));

	_layerColorWait->addChild(menu);

	//--Set game state--
	_state = GameState::MAINMENU;
}

void GameScene::onEnter() {
	Layer::onEnter();

	this->spawnNewObstacle();
	this->spawnNewObstacle();
	this->spawnNewObstacle();

	SimpleAudioEngine::getInstance()->playBackgroundMusic(
			"Suoni/suono_menu.mp3", true);
}

void GameScene::flyTony(Ref *obj) {
	SimpleAudioEngine::getInstance()->preloadEffect("Suoni/pop.mp3");
	SimpleAudioEngine::getInstance()->playEffect("Suoni/pop.mp3");
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->playBackgroundMusic("Suoni/tonyfly.mp3",
			true);
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5);

	MenuItemSprite* btn = (MenuItemSprite*) obj;

	btn->setPosition(Point(-130, -210));
	btn->getNormalImage()->runAction(
			CCSequence::create(Animate::create(_cloudBtnAnimation),
					CallFunc::create(
							CC_CALLBACK_0(GameScene::callbackFlyTony, this)),
					NULL));

	Vector<Node*> btnChildren = btn->getChildren();
	for (Vector<Node*>::iterator it = btnChildren.begin();
			it < btnChildren.end(); it++) {
		Label* node = dynamic_cast<Label *>(*it);

		if (node != NULL) {
			node->setVisible(false);
		}
	}
}

void GameScene::callbackFlyTony() {
	_player->setVisible(true);
	_label->setVisible(true);
	_layerColorWait->setVisible(false);

	//--Set game state
	_state = GameState::RUNNING;

	this->scheduleUpdate();
}

void GameScene::onExit() {
	CCLayer::onExit();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
}

void GameScene::drawLayout() {
	this->drawBackground();
	this->drawScoreLabel();
	this->drawPlayer();
}

void GameScene::drawBackground() {
	// bottom
	_background1 = CCSprite::create("clouds_part1.png");
	_background1->setPosition(Point(0, 0));
	_background1->setAnchorPoint(Point(0, 0));
	_foreground->addChild(_background1, DrawingOrderGround);

	_background2 = CCSprite::create("clouds_part2.png");
	_background2->setPosition(
			Point(
					_background1->getPositionX()
							+ _background1->getContentSize().width, 0));
	_background2->setAnchorPoint(Point(0, 0));
	_foreground->addChild(_background2, DrawingOrderGround);

	// top
	_backgroundTop1 = CCSprite::create("clouds_part1.png");
	_backgroundTop1->setPosition(Point(0, this->getContentSize().height));
	_backgroundTop1->setAnchorPoint(Point(0, 1));
	_backgroundTop1->setFlippedY(true);

	_foreground->addChild(_backgroundTop1, DrawingOrderClouds);

	_backgroundTop2 = CCSprite::create("clouds_part2.png");
	_backgroundTop2->setPosition(
			Point(
					_backgroundTop1->getPositionX()
							+ _backgroundTop1->getContentSize().width,
					this->getContentSize().height));
	_backgroundTop2->setFlippedY(true);
	_backgroundTop2->setAnchorPoint(Point(0, 1));

	_foreground->addChild(_backgroundTop2, DrawingOrderClouds);

	_grounds->pushBack(_background1);
	_grounds->pushBack(_background2);

	_clouds->pushBack(_backgroundTop1);
	_clouds->pushBack(_backgroundTop2);

	// Wheel
	_wheelBg1 = CCSprite::create("wheelBg_01.png");
	_wheelBg1->setPosition(Point(0, 84));
	_wheelBg1->setAnchorPoint(Point(0, 0));
	_midground->addChild(_wheelBg1);

	_wheel = CCSprite::create("wheel.png");
	_wheel->setPosition(Point(394, 242));
	_wheelBg1->addChild(_wheel, -1);
	_wheel->runAction(CCRepeatForever::create(CCRotateBy::create(8, 360)));

	_wheelBg2 = CCSprite::create("wheelBg_02.png");
	_wheelBg2->setPosition(
			Point(_wheelBg1->getPositionX() + _wheelBg1->getContentSize().width,
					84));
	_wheelBg2->setAnchorPoint(Point(0, 0));
	_midground->addChild(_wheelBg2);

	_wheels->pushBack(_wheelBg1);
	_wheels->pushBack(_wheelBg2);
}

void GameScene::drawScoreLabel() {
	_score = 0;

	ostringstream s1;
	s1 << _score;
	_label = Label::createWithBMFont(s_FontScore, s1.str());

	_label->setPosition(
			Point(_screenSize.width / 2, _screenSize.height / 2 + 200));
	_label->setScale(2);

	this->addChild(_label, LayerDrawingForeground);
	_label->setVisible(false);
}

void GameScene::drawPlayer() {

	_player = CCSprite::createWithSpriteFrameName("tony_0001");
	this->addChild(_player, DrawingOrderHero);
	_player->setPosition(Point(200, 400));
	_player->setVisible(false);
}

// Update

void GameScene::update(float dt) {
	_background->setPosition(
			Point(
					_background->getPositionX()
							- (_scrollSpeedBackground * FIXED_TIME_STEP),
					_background->getPositionY()));
	_midground->setPosition(
			Point(
					_midground->getPositionX()
							- (_scrollSpeedMidGround * FIXED_TIME_STEP),
					_midground->getPositionY()));
	_foreground->setPosition(
			Point(
					_background->getPositionX()
							- (_scrollSpeedForeground * FIXED_TIME_STEP),
					_background->getPositionY()));

	if (_isTap) {
		_yVel = (FORCE_Y);
		_isTap = false;
	}

	_yVel -= (0.016 * GRAVITY);

	if (_yVel >= FORCE_Y) {
		_yVel = FORCE_Y;

	} else if (_yVel <= 2 * -GRAVITY) {
		_yVel = 2 * -GRAVITY;
	}

	_player->setPositionY(_player->getPositionY() + (_yVel * FIXED_TIME_STEP));

	if (_player->getPositionY()
			> VisibleRect::top().y + _player->getContentSize().height) {
		_yVel -= 1.5 * FORCE_Y;
		_player->setPositionY(
				_player->getPositionY() + (_yVel * FIXED_TIME_STEP));
		this->ccTouchEnded(NULL, NULL);
	} else if (_player->getPositionY()
			< VisibleRect::bottom().y - _player->getContentSize().height) {
		this->gameOver();
		this->unscheduleUpdate();
	}

	// Grounds Movement
	for (Vector<Ref*>::iterator it = _grounds->begin(); it < _grounds->end();
			it++) {
		Sprite* ground = (Sprite*) *it;

		// get the world position of the ground
		Point groundWorldPosition = _foreground->convertToWorldSpace(
				ground->getPosition());

		// get the screen position of the ground
		Point groundScreenPosition = this->convertToNodeSpace(
				groundWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (groundScreenPosition.x <= (-1 * ground->getContentSize().width)) {
			ground->setPosition(
					Point(
							ground->getPositionX()
									+ 2 * ground->getContentSize().width,
							ground->getPositionY()));
		}

	}

	// Clouds Movement

	for (Vector<Ref*>::iterator it = _clouds->begin(); it < _clouds->end();
			it++) {
		Sprite* cloud = (Sprite*) *it;

		// get the world position of the ground
		Point cloudWorldPosition = _foreground->convertToWorldSpace(
				cloud->getPosition());

		// get the screen position of the ground
		Point cloudScreenPosition = this->convertToNodeSpace(
				cloudWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (cloudScreenPosition.x <= (-1 * cloud->getContentSize().width)) {
			cloud->setPosition(
					Point(
							cloud->getPositionX()
									+ 2 * cloud->getContentSize().width,
							cloud->getPositionY()));
		}

	}

	// Wheel Movement

	for (Vector<Ref*>::iterator it = _wheels->begin(); it < _wheels->end();
			it++) {
		Sprite* wheelBg = (Sprite*) *it;

		Point wheelBgWorldPosition = _midground->convertToWorldSpace(
				wheelBg->getPosition());

		Point wheelBgScreenPosition = this->convertToNodeSpace(
				wheelBgWorldPosition);

		if (wheelBgScreenPosition.x <= (-1 * wheelBg->getContentSize().width)) {
			wheelBg->setPosition(
					Point(
							wheelBg->getPositionX()
									+ 2 * wheelBg->getContentSize().width,
							wheelBg->getPositionY()));
		}
	}

	Vector<Node*>* offScreenObstacles = new Vector<Node*>;

	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Node* obstacle = (Node*) *it;

		Point obstacleWorldPosition = _foreground->convertToWorldSpace(
				obstacle->getPosition());
		Point obstacleScreenPosition = this->convertToNodeSpace(
				obstacleWorldPosition);

		if (obstacleScreenPosition.x < -obstacle->getContentSize().width) {
			offScreenObstacles->pushBack(obstacle);
		}
	}

	for (Vector<Node*>::iterator it = offScreenObstacles->begin();
			it < offScreenObstacles->end(); it++) {
		Node* obstacleToRemove = (Node *) *it;
		obstacleToRemove->removeFromParentAndCleanup(true);
		_obstacles->eraseObject(obstacleToRemove);

		this->spawnNewObstacle();
	}

	checkForCollision(dt);
}

// Obstacle

void GameScene::spawnNewObstacle() {
	Node* previousObstacle = NULL;
	if (_obstacles->size() > 0) {
		previousObstacle = (Node*) _obstacles->back();
	}
	float previousObstacleXPosition;

	if (previousObstacle == NULL) {
		// this is the first obstacle
		previousObstacleXPosition = firstObstaclePosition;
	}

	else {
		previousObstacleXPosition = previousObstacle->getPositionX();
	}

	Point position = Point(previousObstacleXPosition + distanceBetweenObstacles,
			0);

	Obstacle* obstacle = Obstacle::create();
	obstacle->setPosition(position);
	obstacle->setupRandomPosition();

	_foreground->addChild(obstacle, DrawingOrderPipes);
	_obstacles->pushBack(obstacle);
}

// Collision

void GameScene::checkForCollision(float dt) {
	_timeWaitLabel += dt;
	CollisionType type;

	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Obstacle* obstacle = (Obstacle *) *it;

		Point obstacleWorldPosition = _foreground->convertToWorldSpace(
				obstacle->getPosition());
		Point obstacleScreenPosition = this->convertToNodeSpace(
				obstacleWorldPosition);

		type = obstacle->checkCollisionWithPlayer(_player,
				obstacleScreenPosition);

		if (type == CollisionTypePipe) {
			gameOver();
			this->unscheduleUpdate();
		} else if (type == CollisionTypeGoal) {
			if (_timeWaitLabel > _maxTimeWaitLabel) {
				int idSound = (rand() % 2) + 1;
				ostringstream idSoundString;
				idSoundString << "Suoni/pass_" << idSound << ".mp3";
				SimpleAudioEngine::getInstance()->preloadEffect(
						idSoundString.str().c_str());
				SimpleAudioEngine::getInstance()->playEffect(
						idSoundString.str().c_str());

				_timeWaitLabel = 0;
				updateScoreLabel();
			}
		}
	}
}

// Touch

bool GameScene::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) {
	if (!_isGameOver) {
		_isTap = true;
		_player->stopActionByTag(10);

		Action* action = Sequence::create(
				EaseExponentialOut::create(
						RotateTo::create(0.1, -DEFAULT_ROTATION_UP)),
				EaseExponentialOut::create(RotateTo::create(0.4, 0)),
				CallFunc::create(
						CC_CALLBACK_0(GameScene::finishRotateUp, this)), NULL);
		action->setTag(10);

		_player->runAction(action);

		Animate* animate = Animate::create(_tonyAnimation);
		_player->runAction(animate);

		int idSound = (rand() % 5) + 1;

		ostringstream idSoundString;
		idSoundString << "Suoni/flap_" << idSound << ".mp3";

		SimpleAudioEngine::getInstance()->preloadEffect(
				idSoundString.str().c_str());
		SimpleAudioEngine::getInstance()->playEffect(
				idSoundString.str().c_str());

	}
	return true;
}

void GameScene::onTouchEnded(Touch *pTouch, Event *pEvent) {
	if (!_isGameOver) {
		finishRotateUp();
	}
}

void GameScene::finishRotateUp() {
	_player->stopActionByTag(10);
	Action* action = EaseExponentialIn::create(
			RotateTo::create(0.6, DEFAULT_ROTATION_DOWN));
	action->setTag(10);
	_player->runAction(action);
}

void GameScene::gameOver() {
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->stopAllEffects();
	SimpleAudioEngine::getInstance()->preloadEffect("Suoni/splat.mp3");
	SimpleAudioEngine::getInstance()->playEffect("Suoni/splat.mp3");
	_isGameOver = true;

	_label->setVisible(false);

	this->hideNewRecordLabel();
	_player->stopAllActions();
	_player->runAction(
			MoveTo::create(_player->getPositionY() / _screenSize.height,
					Point(_player->getPositionX(),
							VisibleRect::bottom().y
									- _player->getContentSize().height)));
	_player->runAction(
			EaseExponentialIn::create(
					RotateTo::create(0.3, DEFAULT_ROTATION_DOWN)));

	LayerColor* layer = LayerColor::create(Color4B(255, 255, 255, 255),
			_screenSize.width, _screenSize.height);
	this->addChild(layer, LayerDrawingForeground);
	layer->setOpacity(0);

	layer->runAction(
			Sequence::create(FadeIn::create(0.1), FadeOut::create(0.1),
					RemoveSelf::create(), NULL));

	float offset = 3;

	Point initialPosition = this->getPosition();

	MoveTo* moveUpL = MoveTo::create(0.02,
			Point(this->getPositionX() - offset,
					this->getPositionY() + offset));
	MoveTo* moveUpR = MoveTo::create(0.02,
			Point(this->getPositionX() + offset,
					this->getPositionY() + offset));
	MoveTo* moveDownL = MoveTo::create(0.02,
			Point(this->getPositionX() - offset,
					this->getPositionY() - offset));
	MoveTo* moveDownR = MoveTo::create(0.02,
			Point(this->getPositionX() + offset,
					this->getPositionY() - offset));

	this->runAction(
			CCSequence::create(moveUpL, moveUpR, moveDownR, moveDownL,
					moveDownR, moveUpL, moveDownL, moveUpR, moveUpL, moveUpR,
					moveDownR, moveDownL, moveDownR, moveUpL, moveDownL,
					moveUpR, moveDownR, moveDownL, moveDownR, moveUpL,
					moveDownL, moveUpR, Place::create(initialPosition),
					CallFunc::create(
							CC_CALLBACK_0(GameScene::callbackGameOver, this)),
					NULL));

}

void GameScene::callbackGameOver() {

	SimpleAudioEngine::getInstance()->playBackgroundMusic(
			"Suoni/suono_menu.mp3", true);

	//--INTERSTITIAL CHARTBOOST--
	if (++_counter % 3 == 0)
		ChartboostX::sharedChartboostX()->showInterstitial();

	//--Set game state
	_state = GameState::ENDMENU;

	_layerColorWait->removeAllChildrenWithCleanup(true);
	_layerColorWait->setVisible(true);

	//Score Label
	LayerColor* scoreItem = LayerColor::create(Color4B(33, 43, 112, 255), 200,
			200);
	scoreItem->setAnchorPoint(Point::ZERO);
	scoreItem->setTag(401);
	scoreItem->setPosition(
			Point(_layerColorWait->getContentSize().width / 2 - 210,
					_layerColorWait->getContentSize().height / 2));
	Label* scoreLabel = Label::createWithBMFont(s_FontScore, "Score");
	scoreLabel->setPosition(
			Point(scoreItem->getContentSize().width / 2,
					scoreItem->getContentSize().height * 0.9 - 30));
	scoreLabel->setScale(1.3);
	scoreItem->addChild(scoreLabel);
	ostringstream score;
	score << _score;
	Label* scored = Label::createWithBMFont(s_FontScoreBig, score.str());
	scored->setPosition(
			Point(scoreItem->getContentSize().width / 2,
					scoreItem->getContentSize().height / 2 - 30));
	scored->setScale(1.5);
	scoreItem->addChild(scored);

	//Record Label
	LayerColor* recordItem = LayerColor::create(Color4B(41, 56, 155, 255), 200,
			200);
	recordItem->setAnchorPoint(Point::ZERO);
	recordItem->setTag(402);
	recordItem->setPosition(
			Point(_layerColorWait->getContentSize().width / 2 + 10,
					_layerColorWait->getContentSize().height / 2));

	if (_isNewRecord) {
		Sprite* nastro = Sprite::create("nastro.png");
		nastro->setPosition(Point(100, 130));
		recordItem->addChild(nastro);
		Sprite* badge = Sprite::create("medal0001.png");
		badge->setPosition(Point(100, 55));
		recordItem->addChild(badge);
		Animation* medalAnimation = Animation::create();
		medalAnimation->addSpriteFrameWithFile("medal0001.png");
		medalAnimation->addSpriteFrameWithFile("medal0002.png");
		medalAnimation->addSpriteFrameWithFile("medal0003.png");
		medalAnimation->addSpriteFrameWithFile("medal0004.png");
		for (int i = 0; i < 25; i++) {
			medalAnimation->addSpriteFrameWithFile("medal0001.png");
		}
		medalAnimation->setDelayPerUnit(0.08f);
		medalAnimation->retain();
		badge->runAction(
				CCRepeatForever::create(CCAnimate::create(medalAnimation)));
		Label* recordLabel = Label::createWithBMFont(s_FontScore, "Record!");
		recordLabel->setPosition(
				Point(recordItem->getContentSize().width / 2,
						recordItem->getContentSize().height * 0.9 - 30));
		recordLabel->setScale(1.3);
		recordItem->addChild(recordLabel);
	} else {
		Label* recordLabel = Label::createWithBMFont(s_FontScore, "Highscore");
		recordLabel->setPosition(
				Point(recordItem->getContentSize().width / 2,
						recordItem->getContentSize().height * 0.9 - 30));
		recordLabel->setScale(1.3);
		recordItem->addChild(recordLabel);
		Label* record =
				Label::createWithBMFont(s_FontScoreBig,
						UserDefault::getInstance()->getStringForKey(
								"currentScore", "0").c_str());
		record->setPosition(
				Point(recordItem->getContentSize().width / 2,
						recordItem->getContentSize().height / 2 - 30));
		record->setScale(1.5);
		recordItem->addChild(record);
	}

	_layerColorWait->addChild(scoreItem);
	_layerColorWait->addChild(recordItem);

	MenuItemImage* btn = MenuItemImage::create();
	btn->setNormalSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName("cloud"));
	btn->setSelectedSpriteFrame(
			CCSpriteFrameCache::getInstance()->getSpriteFrameByName("cloud"));
	btn->setCallback(CC_CALLBACK_1(GameScene::restart, this));
	btn->setAnchorPoint(Point(0.5, 0));
	CCLOG("%f", btn->getContentSize().height);
	btn->setPosition(
			Point(_layerColorWait->getContentSize().width / 2,
					_layerColorWait->getContentSize().height / 2 - 200));

	Label* restartLabel = Label::createWithBMFont(s_FontText, "restart");
	restartLabel->setPosition(
			Point(btn->getContentSize().width / 2,
					btn->getContentSize().height / 2 - 6));
	btn->addChild(restartLabel);

	Menu* menu = Menu::create(btn, NULL);
	menu->setPosition(Point::ZERO);

	_layerColorWait->addChild(menu);

}

void GameScene::restart(Ref *obj) {
	SimpleAudioEngine::getInstance()->playEffect("Suoni/pop.mp3");
	LayerColor* layer = (LayerColor*) _layerColorWait->getChildByTag(401);
	layer->removeFromParentAndCleanup(true);
	layer = (LayerColor*) _layerColorWait->getChildByTag(402);
	layer->removeFromParentAndCleanup(true);
	MenuItemSprite* btn = (MenuItemSprite*) obj;
	btn->setAnchorPoint(Point::ZERO);
	btn->setPosition(
			Point(30, _layerColorWait->getContentSize().height / 2 - 410));
	btn->getNormalImage()->runAction(
			Sequence::create(Animate::create(_cloudBtnAnimation),
					CallFunc::create(
							CC_CALLBACK_0(GameScene::callbackRestart, this)),
					NULL));

	Vector<Node*> btnChildren = btn->getChildren();
	for (Vector<Node*>::iterator it = btnChildren.begin();
			it < btnChildren.end(); it++) {
		Label* node = dynamic_cast<Label *>(*it);

		if (node != NULL) {
			node->setVisible(false);
		}
	}

	//--FLURRY EVENT--
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	AnalyticX::flurryLogEvent("RESTART CLICKED");
#endif
}

void GameScene::callbackRestart() {
	this->resetGameAttribute();

	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->playBackgroundMusic("Suoni/tonyfly.mp3",
			true);

	this->runAction(
			CCSequence::createWithTwoActions(DelayTime::create(0.8),
					CallFunc::create(
							CC_CALLBACK_0(GameScene::resetScene, this))));
}

void GameScene::resetGameAttribute() {
	_yVel = 0;
	_isTap = false;
	_isGameOver = false;
	_isNewRecord = false;
	_timeWaitLabel = 0;
	_score = -1;
	_idSoundNewGroupScore = 10;
	_label->setVisible(true);

	_layerColorWait->removeAllChildrenWithCleanup(true);
	_layerColorWait->setVisible(false);
	_background->setPosition(Point::ZERO);
	_background1->setPosition(Point::ZERO);
	_background2->setPosition(
			Point(
					_background1->getPositionX()
							+ _background1->getContentSize().width, 0));
	_backgroundTop1->setPosition(Point(0, this->getContentSize().height));
	_backgroundTop2->setPosition(
			Point(
					_backgroundTop1->getPositionX()
							+ _backgroundTop1->getContentSize().width,
					this->getContentSize().height));

	_midground->setPosition(Point::ZERO);
	_foreground->setPosition(Point::ZERO);

	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Node* obstacleToRemove = (Node *) *it;
		obstacleToRemove->removeFromParentAndCleanup(true);
	}

	_obstacles->clear();

	_player->setPosition(Point(200, 400));
	_player->setRotation(0);
	_player->setVisible(true);

	this->spawnNewObstacle();
	this->spawnNewObstacle();
	this->spawnNewObstacle();
}

void GameScene::resetScene() {
	this->updateScoreLabel();

	//--Set game state
	_state = GameState::RUNNING;

	this->scheduleUpdate();
}

void GameScene::updateScoreLabel() {
	_score++;

	if (_score > 0 && _score % 10 == 0) {

		ostringstream idSoundString;
		idSoundString << "Suoni/" << _idSoundNewGroupScore << ".mp3";

		SimpleAudioEngine::getInstance()->playEffect(
				idSoundString.str().c_str());

		_idSoundNewGroupScore++;

		if (_idSoundNewGroupScore > 34) {
			_idSoundNewGroupScore = 34;
		}
	}

	if (_score
			> atoi(
					CCUserDefault::getInstance()->getStringForKey(
							"currentScore", "").c_str())) {
		showNewRecordLabel();

		ostringstream score;
		score << _score;
		CCUserDefault::getInstance()->setStringForKey("currentScore",
				score.str().c_str());
		CCUserDefault::getInstance()->flush();
	}

	ostringstream score;
	score << _score;
	_label->setString(score.str());
}

void GameScene::showNewRecordLabel() {
	if (!_isNewRecord) {
		_isNewRecord = true;

		_newRecordLabel->runAction(
				EaseBackOut::create(
						MoveTo::create(0.5,
								Point(_screenSize.width / 2,
										_screenSize.height - 80))));
		_recordRays->runAction(FadeIn::create(0.5));
		_recordRays->runAction(
				RepeatForever::create(RotateBy::create(8, -360)));

		this->runAction(
				Sequence::create(DelayTime::create(3),
						CallFunc::create(
								CC_CALLBACK_0(GameScene::hideNewRecordLabel, this)),
						NULL));

		SimpleAudioEngine::getInstance()->preloadEffect("Suoni/new_record.mp3");
		SimpleAudioEngine::getInstance()->playEffect("Suoni/new_record.mp3");
	}
}

void GameScene::hideNewRecordLabel() {
	_newRecordLabel->runAction(
			EaseBackIn::create(
					MoveTo::create(0.5,
							Point(_screenSize.width / 2,
									_screenSize.height
											+ _newRecordLabel->getContentSize().height
													* 2))));

	_recordRays->stopAllActions();

	if (_recordRays->getOpacity() > 0) {
		_recordRays->runAction(CCFadeOut::create(0.5));
	}
}

void GameScene::bannerPressed(Ref *obj) {
	//NdkHelper
	//SendMessageWithParams(string("openApp"),NULL);
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		if (_state == GameState::MAINMENU) {
			if (_backKeyLayer) {
				_backKeyLayer->removeFromParentAndCleanup(true);
				_backKeyLayer = NULL;
				return;
			}

			_backKeyLayer = LayerColor::create(Color4B(9, 13, 73, 102),
					_screenSize.width, _screenSize.height);
			_backKeyLayer->setPosition(Point::ZERO);
			_backKeyLayer->setOpacity(0);

			//--CANCEL BUTTON--
			auto pCancelItem = MenuItemImage::create("violetButton.png",
					"violetButton_tap.png",
					CC_CALLBACK_1(GameScene::cancelLeaveGameCallback, this));

			pCancelItem->setPosition(
					Point(0, -2 * pCancelItem->getContentSize().height));

			//FLY AGAIN label
			auto label = Label::createWithBMFont(s_FontScore, "Fly Again!");
			label->setScale(1.4);
			label->setPosition(
					Point(pCancelItem->getContentSize().width / 2,
							pCancelItem->getContentSize().height / 2));
			pCancelItem->addChild(label);

			//--CONFIRM BUTTON--
			auto pConfirmItem = MenuItemImage::create("violetButton.png",
					"violetButton_tap.png",
					CC_CALLBACK_1(GameScene::confirmLeaveGameCallback,this));

			pConfirmItem->setPosition(
					Point(0, -3 * pCancelItem->getContentSize().height - 20));

			//YES, PLEASE BUTTON
			label = Label::createWithBMFont(s_FontScore, "Please Exit");
			label->setScale(1.2);
			label->setPosition(
					Point(pConfirmItem->getContentSize().width / 2,
							pConfirmItem->getContentSize().height / 2));
			pConfirmItem->addChild(label);
			pConfirmItem->setScale(0.8f);

			auto pMenu = Menu::create(pConfirmItem, pCancelItem, NULL);

			//Really quit Fly Tony?
			label = Label::createWithBMFont(s_FontScore,
					"Really quit Fly Tony?");
			//label->setScale(1.5);
			//label->setAnchorPoint(Point(0.5,0.5));

			auto menuLabel = MenuItemLabel::create(label, [&](Ref* sender) {});
			menuLabel->setScale(1.5);
			menuLabel->setPosition(Point(0, label->getContentSize().height / 2));

			pMenu->addChild(menuLabel);

			_backKeyLayer->addChild(pMenu);

			this->addChild(_backKeyLayer, 100);

			_backKeyLayer->runAction(FadeTo::create(0.2, 200));
		} else if (_state == GameState::ENDMENU) {
			this->resetGameAttribute();
			this->updateScoreLabel();
			_layerColorWait->setVisible(true);
			_player->setVisible(false);
			_label->setVisible(false);
			this->showMainMenu();
		}
	}
}

void GameScene::confirmLeaveGameCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //exit(0);
#endif

    CCLOG("exited 0");
}

void GameScene::cancelLeaveGameCallback(Ref* pSender)
{
	_backKeyLayer->removeFromParentAndCleanup(true);
	_backKeyLayer = NULL;
}

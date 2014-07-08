//
//  GameScene.h
//  TonyFlyingDream
//
//  Created by Luca Diolaiuti on 08/07/14.
//
//

#ifndef __TonyFlyingDream__GameScene__
#define __TonyFlyingDream__GameScene__

#include <iostream>
#include "cocos2d.h"
#include "ChartboostX.h"
#include "NDKHelper/NDKHelper.h"

USING_NS_CC;

class GameScene: public Layer, public ChartboostXDelegate{

	enum class GameState{
		MAINMENU,
		RUNNING,
		ENDMENU
	};

	GameState _state;

	Size _screenSize;
	Node * _background;
	Node * _midground;
	Node * _foreground;
	Sprite * _player;
	Sprite * _background1;
	Sprite * _background2;
	Sprite * _backgroundTop1;
	Sprite * _backgroundTop2;
	Sprite * _wheelBg1;
	Sprite * _wheelBg2;
	Sprite * _wheel;
	Sprite * _recordRays;
	Label* _label;
	Label* _newRecordLabel;
	LayerColor* _layerColorWait;
	LayerColor* _backKeyLayer=NULL;

	int _score;
	int _counter=0;
	int _idSoundNewGroupScore;
	float _yVel;
	float _scrollSpeedBackground;
	float _scrollSpeedMidGround;
	float _scrollSpeedForeground;
	bool _isTap;
	bool _isNewRecord;
	bool _isGameOver;
	float _timeWaitLabel;
	float _maxTimeWaitLabel;

	Vector<Ref*>* _grounds;
	Vector<Ref*>* _clouds;
	Vector<Node*>* _obstacles;
	Vector<Ref*>* _wheels;
	Animation* _tonyAnimation;
	Animation* _cloudBtnAnimation;

	// Draw Layout
	void drawLayout();
	void drawBackground();
	void drawScoreLabel();
	void drawPlayer();

	// Init
	void flyTony(Ref * obj);
	void showMainMenu();
	void callbackFlyTony();

	//New Obstacle
	void spawnNewObstacle();

	//GameOver
	void gameOver();
	void callbackGameOver();
	void restart(Ref *obj);
	void callbackRestart();

	//Reset
	void resetGameAttribute();
	void resetScene();

	// Collision
	void checkForCollision(float dt);
	void updateScoreLabel();

	//Record
	void showNewRecordLabel();
	void hideNewRecordLabel();

	//Banner
	void bannerPressed(Ref * obj);

	//Callback
	void confirmLeaveGameCallback(Ref *obj);
	void cancelLeaveGameCallback(Ref *obj);

public:

	GameScene();
	virtual ~GameScene();

	virtual void onEnter();
	virtual void onExit();
	virtual bool init();
	virtual void update(float dt);
	virtual bool onTouchBegan(Touch *pTouch, Event *pEvent);
	virtual void onTouchEnded(Touch *pTouch, Event *pEvent);

	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	static Scene* scene();

	void finishRotateUp();

	CREATE_FUNC(GameScene)
	;

};
#endif /* defined(__TonyFlyingDream__GameScene__) */

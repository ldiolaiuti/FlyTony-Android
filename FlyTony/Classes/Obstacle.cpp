//
//  Obstacle.cpp
//  TonyFlyingDream
//
//  Created by Luca Diolaiuti on 08/07/14.
//
//

#include "Obstacle.h"
#include "VisibleRect.h"

#define PTM_RATIO 32


// visibility on a 3,5-inch iPhone ends a 88 points and we want some meat
static const float minimumYPositionTopPipe = 80;

// visibility ends at 480 and we want some meat
static const float maximumYPositionBottomPipe = 700.f;

// distance between top and bottom pipe
static const float pipeDistance = 180.0f;

// calculate the end of the range of top pipe
static const float maximumYPositionTopPipe = maximumYPositionBottomPipe - pipeDistance;


Obstacle::Obstacle()
{
    
}

Obstacle::~Obstacle()
{
    
}




bool Obstacle::init()
{
    if (!CCNode::init())
    {
        return false;
    }
    _screenSize = Director::getInstance()->getWinSize();
    
    this->setContentSize(Size(80, 640));
    this->setAnchorPoint(Point(0, 0));
    
    _topPipe = Sprite::create("obstacleTopUp.png");
    _topPipe->setAnchorPoint(Point(0.5, 0));
    _topPipe->setPosition(Point(this->getContentSize().width/2,0));
    
    Sprite* basePipeUp = Sprite::create("obstacleBase.png");
    basePipeUp->setAnchorPoint(Point(0.5f, 0));
    basePipeUp->setPosition(Point(_topPipe->getContentSize().width/2, _topPipe->getContentSize().height));
    basePipeUp->setScaleY(20);
    
    _topPipe->addChild(basePipeUp);
    
    _bottomPipe = CCSprite::create("obstacleTopDown.png");
    _bottomPipe->setAnchorPoint(Point(0.5, 1.0));
    _bottomPipe->setPositionX(this->getContentSize().width/2);
    
    Sprite* basePipeDown = Sprite::create("obstacleBase.png");
    basePipeDown->setAnchorPoint(Point(0.5f, 1.0));
    basePipeDown->setPosition(Point(_bottomPipe->getContentSize().width/2,0));
    basePipeDown->setScaleY(200);

    _bottomPipe->addChild(basePipeDown);

    _goal = Node::create();
    _goal->setContentSize(Size(20, 640));
    _goal->setAnchorPoint(Point(0, 0));
    _goal->setPosition(Point(this->getContentSize().width/2 - _goal->getContentSize().width/2, 0));
    
    this->addChild(_topPipe);
    this->addChild(_bottomPipe);
    this->addChild(_goal);
    
    return true;
}


void Obstacle::setupRandomPosition()
{
    // value between 0.f and 1.f
    float random = (float)((rand()%10)*0.1);
    float range = maximumYPositionTopPipe - minimumYPositionTopPipe;
    
    _topPipe->setPosition(Point(_topPipe->getPositionX(), _screenSize.height - (minimumYPositionTopPipe + (random * range))));
    
    _bottomPipe->setPosition(Point(_bottomPipe->getPositionX(), _topPipe->getPositionY() - pipeDistance));

}

CollisionType Obstacle::checkCollisionWithPlayer(Sprite *sprite, const Point& point)
{
    CollisionType type = CollisionTypeNone;
    
    Rect bottomPipeRect = Rect(
                                       point.x,
                                       point.y,
                                       _bottomPipe->getContentSize().width/2,
                                       _bottomPipe->getPositionY());
    
    Rect topPipeRect = Rect(
                                       point.x,
                                       _topPipe->getPositionY(),
                                       _topPipe->getContentSize().width/2,
                                       _screenSize.height);
    
    Rect goalRect = Rect(
                                       point.x,
                                       point.y,
                                       _goal->getContentSize().width,
                                        VisibleRect::getVisibleRect().size.height);
    
    
    Rect spriteBoundingBox = Rect(sprite->getPositionX() - sprite->getContentSize().width/4,
                                          sprite->getPositionY() - sprite->getContentSize().height/4,
                                          sprite->getContentSize().width/2,
                                          sprite->getContentSize().height/2);
    
    if (bottomPipeRect.intersectsRect(spriteBoundingBox))
    {
        type = CollisionTypePipe;
    }
    else if (topPipeRect.intersectsRect(spriteBoundingBox))
    {
        type = CollisionTypePipe;
    }
    else if(goalRect.intersectsRect(spriteBoundingBox))
    {
        type = CollisionTypeGoal;
    }
    
    return type;
}



//
//  Obstacle.h
//  TonyFlyingDream
//
//  Created by Luca Diolaiuti on 08/07/14.
//
//

#ifndef __TonyFlyingDream__Obstacle__
#define __TonyFlyingDream__Obstacle__

#include <iostream>
#include "cocos2d.h"
#include "GameScene.h"


USING_NS_CC;

typedef enum
{
    CollisionTypeNone,
    CollisionTypeGoal,
    CollisionTypePipe,
    
}CollisionType;


class Obstacle : public Node
{
    
    Size _screenSize;
    
    Sprite* _topPipe;
    Sprite* _bottomPipe;
    Node*   _goal;
    
    GameScene* _game;
    
public:
    
    Obstacle();
    virtual ~Obstacle();
    
    bool init();
    
    void setupRandomPosition();
    
    CREATE_FUNC(Obstacle);
    
    CollisionType checkCollisionWithPlayer(Sprite * sprite, const Point& point);
    
};

#endif /* defined(__TonyFlyingDream__Obstacle__) */

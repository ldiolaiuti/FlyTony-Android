//
//  FakeSplash.h
//  FlyTony
//
//  Created by Luca Diolaiuti on 08/07/14.
//
//

#ifndef __Kroms__FakeSplash__
#define __Kroms__FakeSplash__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class FakeSplash : public Layer
{
    
    void showInitialMenu(Node* node);
    
public:
    
    FakeSplash();
    virtual ~FakeSplash();
    
    static Scene* scene();
    CREATE_FUNC(FakeSplash);
    
    virtual bool init();
};

#endif /* defined(__Kroms__FakeSplash__) */
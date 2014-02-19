#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

	void gameLogic(float dt);
	void generateSpike();

	void resetGame();
	bool isColliding();
	bool isPixelPerfectCollide(CCSprite* sp1, CCSprite* sp2);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);


private:

	CCTexture2D* _texBackground;
	CCTexture2D* _texGround;
	CCTexture2D* _texPlane[4];
	CCTexture2D *_texSpikeUp, *_texSpikeDown;
	CCImage *_texMaskPlane, *_texMaskSpikeUp, *_texMaskSpikeDown;
	int _curPlane;

	CCLabelTTF* _lbl;
	CCSprite* _background;
	CCSprite* _ground;
	CCSprite* _plane, *_plane2;
	std::vector<CCSprite*> _spikes;

	float _pGravity;
	float _pTapVelocity;
	float _pVelocity;
	float _pMaxVelocity;
	float _pRotation;
	float _pMaxRotation;
	float _pMinRotation;
	float _pBgSpeed;
	float _pSpeed;
	int _kNextSpawn;

	bool _xCrash;
};

#endif // __HELLOWORLD_SCENE_H__

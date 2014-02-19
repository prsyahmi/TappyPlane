#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GLES-Render.h"
#include "B2DebugDrawLayer.h"
#include "GameOver.h"

USING_NS_CC;

class BoxContactListener :
	public b2ContactListener
{
public:
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);

	bool _collided;
	CCSprite* _plane;
};

class HelloWorld : public cocos2d::CCLayer
{
public:
	enum EVerticesType
	{
		eVerticesPlane,
		eVerticesGround,
		eVerticesSpikeUp,
		eVerticesSpikeDown
	};

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

	void gameLogic(float dt);
	void generateSpike();
	void removeSpikes();
	void resetGame();
	void addBoxBodyForSprite(CCSprite* sprite, EVerticesType type);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);


private:
	CCTexture2D* _texBackground;
	CCTexture2D* _texGround;
	CCTexture2D* _texPlane[4];
	CCTexture2D *_texSpikeUp, *_texSpikeDown;
	CCTexture2D *_texTap[2];
	int _curPlane;

	b2World *_world;
	BoxContactListener _contactListener;

	CCLabelTTF* _lblScore;
	CCSprite* _txtGetReady;
	CCSprite* _background;
	CCSprite* _ground;
	CCSprite* _plane;
	CCSprite* _instTap;
	CCAnimate* _planeAnim;
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
	float _travel;
	unsigned int _hiScore;

	bool _xCrash;
	bool _xGameOver;
	bool _xGameInstruct;

	GameOver *_layerGameOver;
};

#endif // __HELLOWORLD_SCENE_H__

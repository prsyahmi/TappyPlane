#include "HelloWorldScene.h"
#include <random>

#define PTM_RATIO 32.0

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::create();
	layer->_layerGameOver = GameOver::create();
	layer->_layerGameOver->setMainScene(layer);

	// add layer as a child to scene
	scene->addChild(layer);
	scene->addChild(layer->_layerGameOver);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	CCTextureCache* cache = CCTextureCache::sharedTextureCache();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "Images/CloseNormal.png",
                                        "Images/CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	_world = new b2World(gravity);
	_world->SetContactListener(&_contactListener);

	ccTexParams repeatParam = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE };

	_texBackground = cache->addImage("Images/background.png");
	_texBackground->setTexParameters(&repeatParam);
	_background = CCSprite::createWithTexture(_texBackground, CCRect(0, 0, visibleSize.width, visibleSize.height));

	_texGround = cache->addImage("Images/groundGrass2.png");
	_texGround->setTexParameters(&repeatParam);
	_ground = CCSprite::createWithTexture(_texGround, CCRect(0, 0, visibleSize.width, 71));

	_texSpikeUp = cache->addImage("Images/rock.png");
	_texSpikeDown = cache->addImage("Images/rockDown.png");
	_texSpikeUp->setAntiAliasTexParameters();

	_texPlane[0] = cache->addImage("Images/planeBlue1.png");
	_texPlane[1] = cache->addImage("Images/planeBlue2.png");
	_texPlane[2] = cache->addImage("Images/planeBlue3.png");
	_texPlane[3] = cache->addImage("Images/planeBlue2.png");
	_curPlane = 0;
	_plane = CCSprite::create();

	CCAnimation* anim = CCAnimation::create();
	CCRect rcAnim(0, 0, _texPlane[0]->getContentSize().width, _texPlane[0]->getContentSize().height);

	anim->addSpriteFrameWithTexture(_texPlane[0], rcAnim);
	anim->addSpriteFrameWithTexture(_texPlane[1], rcAnim);
	anim->addSpriteFrameWithTexture(_texPlane[2], rcAnim);
	anim->addSpriteFrameWithTexture(_texPlane[3], rcAnim);
	anim->setDelayPerUnit(0.1f);
	anim->setLoops(-1);
	_planeAnim = CCAnimate::create(anim);
	_planeAnim->retain();

	_texTap[0] = cache->addImage("UI/tap.png");
	_texTap[1] = cache->addImage("UI/tapTick.png");
	_instTap = CCSprite::create();

	CCAnimation* animTap = CCAnimation::create();
	CCRect rcAnimTap(0, 0, _texTap[0]->getContentSize().width, _texTap[0]->getContentSize().height);
	animTap->addSpriteFrameWithTexture(_texTap[0], rcAnimTap);
	animTap->addSpriteFrameWithTexture(_texTap[1], rcAnimTap);
	animTap->setDelayPerUnit(0.1f);
	animTap->setLoops(-1);
	_instTap->runAction(CCAnimate::create(animTap));

	_txtGetReady = CCSprite::create("UI/textGetReady.png");
	_txtGetReady->setPosition(ccp(visibleSize.width / 2, visibleSize.height - 200));

	_background->setPosition(CCPoint(visibleSize.width / 2, visibleSize.height / 2));
	_ground->setPosition(CCPoint(visibleSize.width / 2, _texGround->getContentSize().height / 2));
	
	//
	//  UI
	//

	_lblScore = CCLabelTTF::create("Score: 0", "Arial", 24);
	_lblScore->setPosition(CCPoint(
		origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - _lblScore->getContentSize().height)
		);
	_lblScore->setColor({ 80, 80, 80 });

	this->addChild(_background, 0);
	this->addChild(_ground, 2);
	this->addChild(_plane, 3);
	this->addChild(_instTap, 4);
	this->addChild(_txtGetReady, 4);
	this->addChild(_lblScore, 50);
	this->addChild(pMenu, 100);
	//this->addChild(B2DebugDrawLayer::create(_world, PTM_RATIO), 100);

	addBoxBodyForSprite(_plane, eVerticesPlane);
	_contactListener._collided = false;
	_contactListener._plane = _plane;
	resetGame();

	this->schedule(schedule_selector(HelloWorld::gameLogic));

	this->setTouchEnabled(true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);

	return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}


void HelloWorld::generateSpike()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	_texSpikeUp->setAntiAliasTexParameters();
	_texSpikeDown->setAntiAliasTexParameters();
	CCSprite* spikeUp = CCSprite::createWithTexture(_texSpikeUp);
	CCSprite* spikeDown = CCSprite::createWithTexture(_texSpikeDown);

	std::random_device generator;
	std::uniform_int_distribution<int> distribution(28, 40);
	std::uniform_int_distribution<int> distribution2(-100, 100);
	int gap = distribution(generator);
	int offset = distribution2(generator);

	spikeUp->setAnchorPoint({ 0.5, 0.0 });
	spikeUp->setPosition(CCPoint(visibleSize.width + 100, offset - gap));

	spikeDown->setAnchorPoint({ 0.5, 1.0 });
	spikeDown->setPosition(CCPoint(visibleSize.width + 100, visibleSize.height + offset + gap));

	addBoxBodyForSprite(spikeUp, eVerticesSpikeUp);
	addBoxBodyForSprite(spikeDown, eVerticesSpikeDown);

	this->addChild(spikeUp, 1);
	this->addChild(spikeDown, 1);

	_spikes.push_back(spikeUp);
	_spikes.push_back(spikeDown);

	if (offset > 60)
	{
		CCSprite* extraSpikeUp;

		extraSpikeUp = CCSprite::createWithTexture(_texSpikeUp);
		extraSpikeUp->setPosition(CCPoint(visibleSize.width + 50, offset - 10));
		extraSpikeUp->setScaleX(1.5f);
		this->addChild(extraSpikeUp, 1);
		_spikes.push_back(extraSpikeUp);
		addBoxBodyForSprite(extraSpikeUp, eVerticesSpikeUp);

		extraSpikeUp = CCSprite::createWithTexture(_texSpikeUp);
		extraSpikeUp->setPosition(CCPoint(visibleSize.width + 150, offset - 30));
		extraSpikeUp->setScaleX(1.7f);
		extraSpikeUp->setFlipX(true);
		this->addChild(extraSpikeUp, 1);
		_spikes.push_back(extraSpikeUp);
		addBoxBodyForSprite(extraSpikeUp, eVerticesSpikeUp);
	}
	else if (offset <= -32)
	{
		CCSprite* extraSpikeDown;

		extraSpikeDown = CCSprite::createWithTexture(_texSpikeDown);
		extraSpikeDown->setPosition(CCPoint(visibleSize.width + 50, visibleSize.height + offset + 10));
		extraSpikeDown->setScaleX(1.5f);
		this->addChild(extraSpikeDown, 1);
		_spikes.push_back(extraSpikeDown);
		addBoxBodyForSprite(extraSpikeDown, eVerticesSpikeDown);

		extraSpikeDown = CCSprite::createWithTexture(_texSpikeDown);
		extraSpikeDown->setPosition(CCPoint(visibleSize.width + 150, visibleSize.height + offset + 30));
		extraSpikeDown->setScaleX(1.7f);
		extraSpikeDown->setFlipX(true);
		this->addChild(extraSpikeDown, 1);
		_spikes.push_back(extraSpikeDown);
		addBoxBodyForSprite(extraSpikeDown, eVerticesSpikeDown);
	}
}

void HelloWorld::addBoxBodyForSprite(CCSprite* sprite, EVerticesType type)
{
	b2BodyDef spriteBodyDef;
	spriteBodyDef.type = b2_dynamicBody;
	spriteBodyDef.position.Set(sprite->getPositionX() / PTM_RATIO,
		sprite->getPositionY() / PTM_RATIO);
	spriteBodyDef.userData = sprite;
	b2Body *spriteBody = _world->CreateBody(&spriteBodyDef);

	b2PolygonShape spriteShape;

	b2Vec2 vertices[8];
	int numVertices;

	switch (type)
	{
	case eVerticesPlane:
		numVertices = 7;
		vertices[0] = { 22 / PTM_RATIO, 8 / PTM_RATIO };
		vertices[1] = { 58 / PTM_RATIO, 0 };
		vertices[2] = { 88 / PTM_RATIO, 8 / PTM_RATIO };
		vertices[3] = { 88 / PTM_RATIO, 54 / PTM_RATIO };
		vertices[4] = { 70 / PTM_RATIO, 73 / PTM_RATIO };
		vertices[5] = { 25 / PTM_RATIO, 73 / PTM_RATIO };
		vertices[6] = { 0, 57 / PTM_RATIO };
		break;
	case eVerticesSpikeUp:
		numVertices = 3;
		vertices[0] = { 0, 0  };
		vertices[1] = { 108 / PTM_RATIO , 0  };
		vertices[2] = { 64 / PTM_RATIO, 239 / PTM_RATIO };
		break;
	case eVerticesSpikeDown:
		numVertices = 3;
		vertices[0] = { 64 / PTM_RATIO, 0 };
		vertices[1] = { 108 / PTM_RATIO, 239 / PTM_RATIO };
		vertices[2] = { 0, 239 / PTM_RATIO };
		break;
	}

	spriteShape.Set(vertices, numVertices);

	b2FixtureDef spriteShapeDef;
	spriteShapeDef.shape = &spriteShape;
	spriteShapeDef.density = 10.0;
	spriteShapeDef.isSensor = true;
	spriteBody->CreateFixture(&spriteShapeDef);

	sprite->setUserData(spriteBody);
}

bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (_xGameOver) return false;
	if (_xGameInstruct)
	{
		_xGameInstruct = false;
		_instTap->setVisible(false);
		_txtGetReady->setVisible(false);
		_lblScore->setVisible(true);
	}

	_pVelocity = _pTapVelocity;
	return true;
}

void HelloWorld::removeSpikes()
{
	std::vector<CCSprite*>::iterator it;
	for (it = _spikes.begin(); it != _spikes.end();)
	{
		_world->DestroyBody((b2Body*)(*it)->getUserData());
		this->removeChild(*it, true);
		it = _spikes.erase(it);
	}
}

void HelloWorld::resetGame()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

	_plane->runAction(_planeAnim);

	_pTapVelocity = 8.0f;
	_pVelocity = 1.0f;
	_pGravity = -0.5f;
	_pMaxVelocity = -15.0f;
	_pRotation = 0;
	_kNextSpawn = 5;
	_pBgSpeed = 0.05f;
	_pSpeed = 2.7f;
	_travel = 0.0f;

	_plane->setPosition({ 150, visibleSize.height - 250 });
	_plane->setRotation(-_pVelocity);

	_lblScore->setVisible(false);

	_instTap->setPosition(ccp(230, _plane->getPositionY() - 30));
	_instTap->setVisible(true);

	_txtGetReady->setVisible(true);
	_txtGetReady->setOpacity(100);
	_txtGetReady->setPositionY(visibleSize.height - 220);
	_txtGetReady->runAction(
		CCEaseSineOut::create(
		CCMoveTo::create(0.5f, ccp(visibleSize.width / 2, visibleSize.height - 200)
		)));
	_txtGetReady->runAction(CCFadeIn::create(0.5f));

	removeSpikes();

	_xCrash = false;
	_xGameOver = false;
	_xGameInstruct = true;

	this->resumeSchedulerAndActions();
}

void HelloWorld::gameLogic(float dt)
{
	const CCSize& visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCRect rc;
	CCPoint pt;

	if (_xGameInstruct) return;

	rc = _background->getTextureRect();
	rc.origin.x += _pBgSpeed;
	_background->setTextureRect(rc);

	rc = _ground->getTextureRect();
	rc.origin.x += _pSpeed;
	_ground->setTextureRect(rc);

	//
	//  Move the world
	//

	std::vector<CCSprite*>::iterator it;
	for (it = _spikes.begin(); it != _spikes.end();)
	{
		(*it)->setPositionX((*it)->getPositionX() - _pSpeed);
		if ((*it)->getPositionX() < -(*it)->getContentSize().width)
		{
			if ((*it)->getUserData())
			{
				_world->DestroyBody((b2Body*)(*it)->getUserData());
			}

			this->removeChild(*it, true);
			it = _spikes.erase(it);
		}
		else
		{
			++it;
		}
	}

	//
	//  box2d update and sprite sync
	//

	_world->Step(dt, 10, 10);
	for (b2Body *b = _world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			CCSprite *sprite = (CCSprite *)b->GetUserData();

			b2Vec2 b2Position = b2Vec2((sprite->getPositionX() - sprite->getAnchorPointInPoints().x) / PTM_RATIO,
				(sprite->getPositionY() - sprite->getAnchorPointInPoints().y) / PTM_RATIO);
			float32 b2Angle = -1 * CC_DEGREES_TO_RADIANS(sprite->getRotation());
			b->SetTransform(b2Position, b2Angle);
		}
	}

	//
	//  Plane logic
	//

	pt = _plane->getPosition();
	if (_pVelocity >= _pMaxVelocity) {
		_pVelocity += _pGravity;
	}

	if (pt.y > 64)
	{
		pt.setPoint(150, pt.y + _pVelocity);
		_plane->setPosition(pt);
		_plane->setRotation(-_pVelocity);
	}

	if (!_xGameOver)
	{

		if (!_xCrash)
		{
			static char x[100] = { 0 };
			_travel += _pBgSpeed;
			unsigned int score = (int)ceilf(_travel);

			sprintf_s(x, "Score: %u", score);
			_lblScore->setString(x);

			if (pt.y + _pVelocity >= visibleSize.height)
			{
				_pVelocity = 0;
			}

			_kNextSpawn--;
			if (_kNextSpawn <= 0) {
				generateSpike();

				std::random_device generator;
				std::uniform_int_distribution<int> distribution(50, 150);
				_kNextSpawn = distribution(generator);
			}
		}
		else
		{
			_lblScore->setVisible(false);
			_layerGameOver->show((int)ceilf(_travel), _hiScore);
			_xGameOver = true;
		}
	}
	else
	{
		if (pt.y <= 64)
		{
			this->pauseSchedulerAndActions();
		}
	}

	if (_contactListener._collided || pt.y <= 64)
	{
		_xCrash = true;
		_pBgSpeed = 0;
		_pSpeed = 0;

		_plane->stopAction(_planeAnim);
	}
}

void BoxContactListener::BeginContact(b2Contact* contact)
{
	_collided =
		contact->GetFixtureA()->GetBody()->GetUserData() == _plane ||
		contact->GetFixtureB()->GetBody()->GetUserData() == _plane;
}

void BoxContactListener::EndContact(b2Contact* contact)
{
	_collided = false;
}
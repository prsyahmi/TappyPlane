#include "HelloWorldScene.h"
#include <random>

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

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
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

	_lbl = CCLabelTTF::create("Hello World", "kenvector_future", 24);

	// position the label on the center of the screen
	_lbl->setPosition(
		CCPoint(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - _lbl->getContentSize().height)
		);

	// add the label as a child to this layer
	this->addChild(_lbl, 1);

	ccTexParams repeatParam = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE };

	_texBackground = cache->addImage("background.png");
	_texBackground->setTexParameters(&repeatParam);
	_background = CCSprite::createWithTexture(_texBackground, CCRect(0, 0, visibleSize.width, visibleSize.height));

	_texGround = cache->addImage("groundGrass2.png");
	_texGround->setTexParameters(&repeatParam);
	_ground = CCSprite::createWithTexture(_texGround, CCRect(0, 0, visibleSize.width, 71));

	_texSpikeUp = cache->addImage("rock.png");
	_texSpikeDown = cache->addImage("rockDown.png");
	_texSpikeUp->setAntiAliasTexParameters();

	_texPlane[0] = cache->addImage("Planes/planeBlue1.png");
	_texPlane[1] = cache->addImage("Planes/planeBlue2.png");
	_texPlane[2] = cache->addImage("Planes/planeBlue3.png");
	_texPlane[3] = cache->addImage("Planes/planeBlue2.png");
	_curPlane = 0;
	_plane = CCSprite::create();
	_plane2 = CCSprite::create();
	_plane2->setTexture(_texPlane[0]);
	_plane2->retain();

	/*
	_texMaskPlane = new Image();
	_texMaskPlane->initWithImageFile("Planes/planeBlue1.png");
	_texMaskPlane->
	*/
	CCAnimation* anim = CCAnimation::create();
	CCRect rcAnim(0, 0, _texPlane[0]->getContentSize().width, _texPlane[0]->getContentSize().height);

	anim->addSpriteFrameWithTexture(_texPlane[0], rcAnim);
	anim->addSpriteFrameWithTexture(_texPlane[1], rcAnim);
	anim->addSpriteFrameWithTexture(_texPlane[2], rcAnim);
	anim->addSpriteFrameWithTexture(_texPlane[3], rcAnim);
	anim->setDelayPerUnit(0.1f);
	anim->setLoops(-1);
	_plane->runAction(CCAnimate::create(anim));

	_background->setPosition(CCPoint(visibleSize.width / 2, visibleSize.height / 2));
	_ground->setPosition(CCPoint(visibleSize.width / 2, _texGround->getContentSize().height / 2));
	_plane->setPosition(CCPoint(_texPlane[0]->getContentSize().width / 2, visibleSize.height / 2));

	this->addChild(_background, 0);
	this->addChild(_ground, 2);
	this->addChild(_plane, 3);

	resetGame();

	this->schedule(schedule_selector(HelloWorld::gameLogic));

	this->setTouchEnabled(true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	/*auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 1);*/

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


void HelloWorld::resetGame()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

	_pTapVelocity = 8.0f;
	_pVelocity = 1.0f;
	_pGravity = -0.5f;
	_pMaxVelocity = -10.0f;
	_pRotation = 0;
	_kNextSpawn = 5;
	_pBgSpeed = 0.05f;
	_pSpeed = 2.7f;

	_xCrash = false;
	_plane->setPosition({ 150, visibleSize.height / 2 });

	std::vector<CCSprite*>::iterator it;
	for (it = _spikes.begin(); it != _spikes.end();)
	{
		this->removeChild(*it, true);
		it = _spikes.erase(it);
	}
}

void HelloWorld::gameLogic(float dt)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

	CCRect rc;
	CCPoint pt;
	static float fGravity = 0;

	static char x[100] = { 0 };

	rc = _background->getTextureRect();
	rc.origin.x += _pBgSpeed;
	_background->setTextureRect(rc);

	rc = _ground->getTextureRect();
	rc.origin.x += _pSpeed;
	_ground->setTextureRect(rc);

	std::vector<CCSprite*>::iterator it;
	for (it = _spikes.begin(); it != _spikes.end();)
	{
		(*it)->setPositionX((*it)->getPositionX() - _pSpeed);
		if ((*it)->getPositionX() < -(*it)->getContentSize().width)
		{
			this->removeChild(*it, true);
			it = _spikes.erase(it);
		}
		else
		{
			++it;
		}
	}

	fGravity += dt;
	if (fGravity >= 0.015)
	{
		if (!_xCrash)
		{
			pt = _plane->getPosition();

			if (_pVelocity >= _pMaxVelocity) {
				_pVelocity += _pGravity;
			}

			sprintf(x, "%f", pt.y);
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

			pt.setPoint(150, pt.y + _pVelocity);
			_plane->setPosition(pt);
			_plane->setRotation(-_pVelocity);
			_plane2->setPosition(pt);
			_plane2->setRotation(-_pVelocity);
		}
		else
		{
		}

		fGravity = 0;

		//
		//  Logic goes here
		//
	}

	if (isColliding())
	{
		_xCrash = true;
	}

	_lbl->setString(x);
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

	//spikeUp->setAnchorPoint({ 0.5, 0.0 });
	spikeUp->setPosition(CCPoint(visibleSize.width + 100, offset - gap));

	//spikeDown->setAnchorPoint({ 0.5, 1.0 });
	spikeDown->setPosition(CCPoint(visibleSize.width + 100, visibleSize.height + offset + gap));

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

		extraSpikeUp = CCSprite::createWithTexture(_texSpikeUp);
		extraSpikeUp->setPosition(CCPoint(visibleSize.width + 150, offset - 30));
		extraSpikeUp->setScaleX(1.7f);
		extraSpikeUp->setFlipX(true);
		this->addChild(extraSpikeUp, 1);
		_spikes.push_back(extraSpikeUp);
	}
	else if (offset <= -32)
	{
		CCSprite* extraSpikeDown;

		extraSpikeDown = CCSprite::createWithTexture(_texSpikeDown);
		extraSpikeDown->setPosition(CCPoint(visibleSize.width + 50, visibleSize.height + offset + 10));
		extraSpikeDown->setScaleX(1.5f);
		this->addChild(extraSpikeDown, 1);
		_spikes.push_back(extraSpikeDown);

		extraSpikeDown = CCSprite::createWithTexture(_texSpikeDown);
		extraSpikeDown->setPosition(CCPoint(visibleSize.width + 150, visibleSize.height + offset + 30));
		extraSpikeDown->setScaleX(1.7f);
		extraSpikeDown->setFlipX(true);
		this->addChild(extraSpikeDown, 1);
		_spikes.push_back(extraSpikeDown);
	}
}

bool HelloWorld::isColliding()
{
	std::vector<CCSprite*>::iterator it;

	for (it = _spikes.begin(); it != _spikes.end(); ++it)
	{
		if (isPixelPerfectCollide(_plane, *it))
		{
			return true;
		}
	}

	return (isPixelPerfectCollide(_plane, _ground));
}

bool HelloWorld::isPixelPerfectCollide(CCSprite* sp1, CCSprite* sp2)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

	CCRect intersection;
	CCRect r1 = sp1->boundingBox();
	CCRect r2 = sp2->boundingBox();
	CCRect ru;
	CCRenderTexture* rt;
	bool isColliding = false;

	if (r1.intersectsRect(r2))
	{
		intersection = CCRect(fmax(r1.getMinX(), r2.getMinX()), fmax(r1.getMinY(), r2.getMinY()), 0, 0);
		intersection.size.width = ceilf(fmin(r1.getMaxX(), r2.getMaxX() - intersection.getMinX()));
		intersection.size.height = ceilf(fmin(r1.getMaxY(), r2.getMaxY() - intersection.getMinY()));

		unsigned int x = intersection.origin.x;
		unsigned int y = intersection.origin.y;
		unsigned int w = intersection.size.width;
		unsigned int h = intersection.size.height;
		unsigned int numPixels = w * h;

		rt = CCRenderTexture::create(w, h);
		//rt->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		//this->addChild(rt);

		// FIXME:
		// Plane can collide with ground but cannot collide with
		// rock. The rock offset is way too far off screen when
		// rendered.
		
		rt->beginWithClear(0, 0, 0, 1);

		glColorMask(1, 0, 0, 0);
		sp1->visit();

		glColorMask(0, 1, 0, 0);
		sp2->visit();

		glColorMask(1, 1, 1, 1);

		ccColor4B *buffer = new ccColor4B[numPixels];

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		rt->end();

		unsigned int step = 2;
		for (unsigned int i = 0; i < numPixels; i += step)
		{
			const ccColor4B& color = buffer[i];
			if (color.r > 0 && color.g > 0)
			{
				isColliding = true;
				break;
			}
		}

		delete[] buffer;
		rt->autorelease();
		//this->removeChild(rt);
	}

	return isColliding;
}

bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (_xCrash)
	{
		resetGame();
	}

	_pVelocity = _pTapVelocity;
	return true;
}

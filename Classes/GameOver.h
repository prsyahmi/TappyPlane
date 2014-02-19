#pragma once

#include "cocos2d.h"

class HelloWorld;

USING_NS_CC;

class GameOver : public cocos2d::CCLayer
{
public:
	GameOver();
	~GameOver();

	CREATE_FUNC(GameOver);
	virtual bool init();

	void setMainScene(HelloWorld* mainScene);
	void show(int score, int hiScore);
	void menuRestartCallback(CCObject* pSender);

private:
	CCSprite* _txtGameOver;
	CCSprite* _bgUi;
	CCMenuItemImage* _btnRestart;
	CCLabelTTF* _lblScore;
	CCLabelTTF* _lblScoreTitle;
	CCLabelTTF* _lblRestart;

	HelloWorld* _mainScene;
};


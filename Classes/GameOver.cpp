#include "GameOver.h"
#include "HelloWorldScene.h"

GameOver::GameOver()
{
}


GameOver::~GameOver()
{
}

bool GameOver::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

	this->setTouchEnabled(true);

	_txtGameOver = CCSprite::create("UI/textGameOver.png");
	_bgUi = CCSprite::create("UI/UIbg.png");
	_btnRestart = CCMenuItemImage::create(
		"UI/buttonLarge.png",
		"UI/buttonLarge.png",
		this,
		menu_selector(GameOver::menuRestartCallback)
		);
	_lblScoreTitle = CCLabelTTF::create("Your Score", "Arial", 28);
	_lblScore = CCLabelTTF::create("0000", "Arial", 24);
	_lblRestart = CCLabelTTF::create("Restart", "Arial", 24);

	CCMenu* pMenu = CCMenu::create(_btnRestart, NULL);
	pMenu->setPosition(CCPointZero);

	this->addChild(_txtGameOver);
	this->addChild(_bgUi);
	_bgUi->addChild(_lblScoreTitle);
	_bgUi->addChild(_lblScore);
	_bgUi->addChild(pMenu);

	_lblScoreTitle->setPosition(ccp(_bgUi->getContentSize().width / 2, _bgUi->getContentSize().height - 40));
	_lblScoreTitle->setColor(ccc3(40, 40, 40));
	_lblScore->setPosition(ccp(_bgUi->getContentSize().width / 2, _bgUi->getContentSize().height - 80));
	_lblScore->setColor(ccc3(0, 0, 0));

	_btnRestart->setPosition(ccp(_bgUi->getContentSize().width / 2, _btnRestart->getContentSize().height));
	_btnRestart->addChild(_lblRestart);
	_lblRestart->setPosition(ccp(_btnRestart->getContentSize().width / 2, _btnRestart->getContentSize().height / 2));
	_lblRestart->setColor(ccc3(0, 0, 0));

	this->setVisible(false);

	return true;
}

void GameOver::show(int score, int hiScore)
{
	const CCSize& visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	char txtScore[64];

	_itoa(score, txtScore, 10);
	_lblScore->setString(txtScore);

	_txtGameOver->setOpacity(0);
	_txtGameOver->setPosition(ccp(visibleSize.width / 2, visibleSize.height / 2));
	_txtGameOver->runAction(CCFadeIn::create(1.0f));
	_txtGameOver->runAction(
		CCEaseSineOut::create(
		CCMoveTo::create(1.0f, ccp(visibleSize.width / 2, visibleSize.height - 100))));

	_bgUi->setOpacity(0);
	_bgUi->setPosition(ccp(visibleSize.width / 2, 0));
	_bgUi->runAction(CCFadeIn::create(1.0f));
	_bgUi->runAction(
		CCEaseSineOut::create(
		CCMoveTo::create(1.0f, ccp(visibleSize.width / 2, visibleSize.height / 2))));

	_btnRestart->setOpacity(0);
	_btnRestart->runAction(CCFadeIn::create(1.0f));

	_lblScore->setOpacity(0);
	_lblScore->runAction(CCFadeIn::create(1.0f));

	_lblScoreTitle->setOpacity(0);
	_lblScoreTitle->runAction(CCFadeIn::create(1.0f));

	this->setVisible(true);
}

void GameOver::menuRestartCallback(CCObject* pSender)
{
	this->setVisible(false);
	_mainScene->resetGame();
}

void GameOver::setMainScene(HelloWorld* mainScene)
{
	_mainScene = mainScene;
}

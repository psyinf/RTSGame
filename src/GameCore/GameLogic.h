#pragma once

namespace nsGameCore{

class GameCore;

class GameLogic
{
public:
	GameLogic(GameCore& game_core);

	virtual void frame();

	void updateProduction();

	void updateCosts();

protected:
	GameCore& mrGameCore;
	unsigned int mFrameNumber;
};
}
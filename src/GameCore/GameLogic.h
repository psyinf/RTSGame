#pragma once

namespace nsGameCore{

class GameCore;

class GameLogic
{
public:
	GameLogic(GameCore& game_core);

	virtual void frame();
protected:
	GameCore& mrGameCore;
};
}
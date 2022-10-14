#pragma once

#include <string>

namespace nsGameCore{

class GameCore;

class GameLogic
{
public:
	explicit GameLogic(GameCore& game_core);

	~GameLogic() = default;
	void frame();

	void updateProduction();

	void updateCosts();

	void updateConstruction();

protected:
	void registerGUIValue(const std::string& category, const std::string& display_name, const std::string& property_name, int value);
	void updateGUIValue(const std::string& category, const std::string& property_name, const std::string& display_name);
private:
	GameCore& mrGameCore;
	unsigned int mFrameNumber  =0;
};
}
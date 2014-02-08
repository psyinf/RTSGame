#include "GameLogic.h"
#include "GameCore.h"
#include "Util/HUDManager.h"
using namespace nsGameCore;


nsGameCore::GameLogic::GameLogic( GameCore& game_core ) 
	:mrGameCore(game_core)
{
	mrGameCore.getHUDManager()->addNameValueLabel("Energy");
	mrGameCore.addNamedValue("Credits", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.addNamedValue("Energy", NamedValue::VT_INTEGRAL).setValue(1000);

}

void nsGameCore::GameLogic::frame()
{
	try 
	{
		nsGameCore::NamedValue& credits = mrGameCore.getNamedValue("Credits");
		nsGameCore::NamedValue& energy = mrGameCore.getNamedValue("Energy");
		credits.setValue(credits.getValue<int>() - 1);
		

		mrGameCore.getHUDManager()->getNameValueLabel("Funds")->setLabelValue(credits.getValueString());
		mrGameCore.getHUDManager()->getNameValueLabel("Energy")->setLabelValue(energy.getValueString());
	}
	catch (const std::exception& e)
	{
		std::cout << "Error in GameLogic::frame: " << e.what() << std::endl;
	}
}


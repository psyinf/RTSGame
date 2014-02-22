#include "GameLogic.h"
#include "GameCore.h"
#include "GameArea.h"
#include "Util/GameModels.h"
#include "Util/HUDManager.h"

using namespace nsGameCore;


nsGameCore::GameLogic::GameLogic( GameCore& game_core ) 
	:mrGameCore(game_core)
	,mFrameNumber(0)
{
	//TODO: some functionality to bind valueName to display name
	mrGameCore.addNamedValue("Credits", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("$");
	mrGameCore.addNamedValue("Energy", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("Energy");
	mrGameCore.addNamedValue("H3", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("H3");
	mrGameCore.addNamedValue("Ores", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("Ores");
	mrGameCore.addNamedValue("Water", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("Water");
	mrGameCore.addNamedValue("Air", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("Air");
	mrGameCore.addNamedValue("CM", NamedValue::VT_INTEGRAL).setValue(1000);
	mrGameCore.getHUDManager()->addNameValueLabel("CM");
}

void nsGameCore::GameLogic::frame()
{
	mFrameNumber++;
	if (0 == mFrameNumber % 60)
	{
		try 
		{
			nsGameCore::NamedValue& credits = mrGameCore.getNamedValue("Credits");
			nsGameCore::NamedValue& energy = mrGameCore.getNamedValue("Energy");
			nsGameCore::NamedValue& helium = mrGameCore.getNamedValue("H3");
			nsGameCore::NamedValue& ores = mrGameCore.getNamedValue("Ores");
			nsGameCore::NamedValue& air = mrGameCore.getNamedValue("Air");
			nsGameCore::NamedValue& water = mrGameCore.getNamedValue("Water");
			nsGameCore::NamedValue& cm = mrGameCore.getNamedValue("CM");
			
			//credits.setValue(credits.getValue<int>() - 1);
			updateConstruction();
			updateCosts();
			updateProduction();

			

			

			//update all labels
			mrGameCore.getHUDManager()->getNameValueLabel("$")->setLabelValue(credits.getValueString());
			mrGameCore.getHUDManager()->getNameValueLabel("Energy")->setLabelValue(energy.getValueString());
			mrGameCore.getHUDManager()->getNameValueLabel("H3")->setLabelValue(helium.getValueString());
			mrGameCore.getHUDManager()->getNameValueLabel("Ores")->setLabelValue(ores.getValueString());
			mrGameCore.getHUDManager()->getNameValueLabel("Air")->setLabelValue(air.getValueString());
			mrGameCore.getHUDManager()->getNameValueLabel("Water")->setLabelValue(water.getValueString());
			mrGameCore.getHUDManager()->getNameValueLabel("CM")->setLabelValue(cm.getValueString());

		}
		catch (const std::exception& e)
		{
			std::cout << "Error in GameLogic::frame: " << e.what() << std::endl;
		}
	}
	
}

void nsGameCore::GameLogic::updateProduction()
{
	std::vector<nsGameCore::CellDataPtr> cell_datas;
	cell_datas = mrGameCore.getGameArea()->getCellDatas();

	for (auto iter = cell_datas.begin(); iter != cell_datas.end(); ++iter)
	{
		nsGameCore::CellDataPtr& cell_data = (*iter);
		nsGameCore::GameBuilding* game_building = dynamic_cast<nsGameCore::GameBuilding*>(cell_data->model_instance.get());
		if (!game_building)
		{
			continue;
		}
		std::string model_type = cell_data->model_instance->getModelDisplayName();
		//if (boost::iequals(model_type, "H3-Mine"))
		{

			bool  can_produce = nsGameCore::ValueRef<bool>(game_building->getProperties(), "Status", "IsProducing").get();
			bool finished = nsGameCore::ValueRef<float>(game_building->getProperties(), "Status", "ConstructionFinished").get() >= 1.0;
			
			

			if (!(can_produce && finished))
			{
				
				//TODO: issue callback
				continue;
			}
			std::vector<std::string> produced_types = game_building->getProperties().getSubPropertyNames("Production");
			for (auto p_iter = produced_types.begin(); p_iter != produced_types.end(); ++p_iter)
			{
				
				nsGameCore::ScopedNamedValue<int>  value(mrGameCore.getNamedValue(*(p_iter)));
				int production = game_building->getProperties().getPropertySubValue<int>("Production", (*p_iter));
				value.getValueRef() += production; 
			}
		}

	}
}

void nsGameCore::GameLogic::updateCosts()
{
	std::vector<nsGameCore::CellDataPtr> cell_datas;
	cell_datas = mrGameCore.getGameArea()->getCellDatas();

	for (auto iter = cell_datas.begin(); iter != cell_datas.end(); ++iter)
	{
		nsGameCore::CellDataPtr& cell_data = (*iter);
		nsGameCore::GameBuilding* game_building = dynamic_cast<nsGameCore::GameBuilding*>(cell_data->model_instance.get());
		if (!game_building)
		{
			continue;
		}
		std::string model_type = cell_data->model_instance->getModelDisplayName();
		//if (boost::iequals(model_type, "H3-Mine"))
		{
			float  is_build = nsGameCore::ValueRef<float> (game_building->getProperties(), "Status", "ConstructionFinished").get();
			if (1.0 > is_build)
			{
				continue;
			}
			nsGameCore::ValueRef<bool> is_producing(game_building->getProperties(), "Status", "IsProducing");
			std::vector<std::string> costs = game_building->getProperties().getSubPropertyNames("RunningCost");
			for (auto p_iter = costs.begin(); p_iter != costs.end(); ++p_iter)
			{
				nsGameCore::ScopedNamedValue<int>  value(mrGameCore.getNamedValue(*(p_iter)));
				int cost = game_building->getProperties().getPropertySubValue<int>("RunningCost", (*p_iter));
				
				value.getValueRef() -= cost; 
				value.getValueRef() = std::max(0, value.getValueRef());

				nsGameCore::ValueRef<bool>  production_status(game_building->getProperties(), "Status", "IsProducing");
				production_status = (0==value.getValueRef());
			}
		}

	}
}

void nsGameCore::GameLogic::updateConstruction()
{
	std::vector<nsGameCore::CellDataPtr> cell_datas;
	cell_datas = mrGameCore.getGameArea()->getCellDatas();

	for (auto iter = cell_datas.begin(); iter != cell_datas.end(); ++iter)
	{
		nsGameCore::CellDataPtr& cell_data = (*iter);
		nsGameCore::GameBuilding* game_building = dynamic_cast<nsGameCore::GameBuilding*>(cell_data->model_instance.get());
		if (!game_building)
		{
			continue;
		}
		std::string model_type = cell_data->model_instance->getModelDisplayName();
		{
			std::vector<nsGameCore::CellDataPtr> cell_datas;
			nsGameCore::ValueRef<float> construction_progress(game_building->getProperties(), "Status", "ConstructionFinished");
			nsGameCore::ValueRef<float> construction_speed(game_building->getProperties(), "Status", "ConstructionSpeed");
			nsGameCore::ValueRef<bool> is_producing(game_building->getProperties(), "Status", "IsProducing");
			if (1.0 > construction_progress.get())
			{
				//check resources
				auto building_costs = game_building->getProperties().getSubPropertyNames("BuildingCost");
				for (auto build_iter = building_costs.begin(); build_iter != building_costs.end(); ++build_iter)
				{
					//TODO: check resources needed for construction		
				}
				construction_progress.getRef() += construction_speed.get();
				is_producing = false;
			}
			else
			{
				is_producing = true;
			}
		}
		
	}
		
}


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
	/*
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
	*/
	
	registerGUIValue("Resources", "$", "Credits", 1000.0);
	registerGUIValue("Resources", "E", "Energy", 1000.0);
	registerGUIValue("Resources", "H3", "H3", 1000.0);
	registerGUIValue("Resources", "Ores", "Ores", 1000.0);
	registerGUIValue("Resources", "Water", "Water", 1000.0);
	registerGUIValue("Resources", "Air", "Air", 1000.0);
	registerGUIValue("Resources", "CM", "CM", 1000.0);
}

void nsGameCore::GameLogic::registerGUIValue( const std::string& category, const std::string& display_name, const std::string& property_name, int value )
{
	mrGameCore.getProperties().addProperty<float>(category, property_name, value );
	mrGameCore.getHUDManager()->addNameValueLabel(display_name);
	ValueRef<int> value_ref(mrGameCore.getProperties(), category, property_name);
	value_ref = value;
}


void nsGameCore::GameLogic::updateGUIValue( const std::string& category, const std::string& property_name, const std::string& display_name )
{
	nsGameCore::ValueRef<int> credits_value(mrGameCore.getProperties(), category, property_name);
	mrGameCore.getHUDManager()->getNameValueLabel(display_name)->setLabelValue(credits_value.getString());
}


void nsGameCore::GameLogic::frame()
{
	mFrameNumber++;
	if (0 == mFrameNumber % 60)
	{
		try 
		{
			

			updateConstruction();
			updateCosts();
			updateProduction();
			
			
			updateGUIValue("Resources", "Credits", "$");
			updateGUIValue("Resources", "Energy", "E");
			updateGUIValue("Resources", "H3", "H3");
			updateGUIValue("Resources", "Ores", "Ores");
			updateGUIValue("Resources", "Air", "Air");
			updateGUIValue("Resources", "Water", "Water");
			updateGUIValue("Resources", "CM", "CM");

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
				
				nsGameCore::ValueRef<int> value_ref(mrGameCore.getProperties(), "Resources", (*p_iter));
				int production = game_building->getProperties().getPropertySubValue<int>("Production", (*p_iter));
				value_ref += production; 
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
		{
			float  is_build = nsGameCore::ValueRef<float> (game_building->getProperties(), "Status", "ConstructionFinished").get();
			if (1.0 > is_build)
			{
				continue;
			}
			//nsGameCore::ValueRef<bool> is_producing(game_building->getProperties(), "Status", "IsProducing");
			std::vector<std::string> costs = game_building->getProperties().getSubPropertyNames("RunningCost");
			for (auto p_iter = costs.begin(); p_iter != costs.end(); ++p_iter)
			{
				//nsGameCore::ScopedNamedValue<int>  value(mrGameCore.getNamedValue(*(p_iter)));
				int cost = game_building->getProperties().getPropertySubValue<int>("RunningCost", (*p_iter));
				nsGameCore::ValueRef<int> value_ref(mrGameCore.getProperties(), "Resources", (*p_iter));
				int production = game_building->getProperties().getPropertySubValue<int>("RunningCost", (*p_iter));
				value_ref += -cost;
				value_ref = std::max(0, value_ref.get());

				nsGameCore::ValueRef<bool>  production_status(game_building->getProperties(), "Status", "IsProducing");
				production_status = (0!=value_ref.get());
				if (!production_status.get())
				{
					std::cout << "damn" << std::endl;
				}
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
			if (1.0 >= construction_progress.get())
			{
				//check resources
				auto building_costs = game_building->getProperties().getSubPropertyNames("BuildingCost");
				bool building_paused = false;
				//check pre-conditions
				for (auto build_iter = building_costs.begin(); build_iter != building_costs.end(); ++build_iter)
				{
					nsGameCore::ValueRef<int> value_ref(game_building->getProperties(), "BuildingCost", (*build_iter));
					nsGameCore::ValueRef<int> res_value_ref(mrGameCore.getProperties(), "Resources", (*build_iter));
					if ((value_ref.get() * construction_speed.get()) > res_value_ref.get())
					{
						building_paused = true;
					}
				}
				if (!building_paused)
				{
					for (auto build_iter = building_costs.begin(); build_iter != building_costs.end(); ++build_iter)
					{
						nsGameCore::ValueRef<int> value_ref(game_building->getProperties(), "BuildingCost", (*build_iter));
						nsGameCore::ValueRef<int> res_value_ref(mrGameCore.getProperties(), "Resources", (*build_iter));
						res_value_ref -= value_ref.get() * construction_speed.get();
					}
					construction_progress.getRef() += construction_speed.get();
					//TODO: move to model
					game_building->getGraphicalModel()->getOrCreateStateSet()->getOrCreateUniform("ConstructionProgress", osg::Uniform::FLOAT)->set(construction_progress.get());
				}
			
				
				is_producing = false;
			}
			else
			{
				is_producing = true;
			}
			//TODO: move to model, this way we could create some icons 
			game_building->getGraphicalModel()->getOrCreateStateSet()->getOrCreateUniform("ProductionPaused", osg::Uniform::BOOL)->set(!is_producing.get());
		}
		
	}
		
}


#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

#include <osg/Vec3i>
#include <boost/functional/hash.hpp>

namespace nsGameCore{

class Terrain;
class GameCore;
class GameModel;




using CellAdress = osg::Vec3i;

struct CellData
{
	explicit CellData(const CellAdress& cell_address, std::shared_ptr<GameModel> model_instance)
		:address(cell_address)
		,model_instance(model_instance)
	{

	}
	CellAdress address;
	std::shared_ptr<GameModel> model_instance;
};

struct CellAdressHasher
{
    std::size_t operator()(const CellAdress& cellAdress) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, cellAdress[0]);
        boost::hash_combine(seed, cellAdress[1]);
        boost::hash_combine(seed, cellAdress[2]);
        return seed; 
    }
};

using CellDataPtr = std::shared_ptr<CellData>;

class GameArea
{
	
public:
	explicit GameArea(nsGameCore::GameCore& ref_game_core);

	bool isFree(const CellAdress& cell_address) const;

	void setUsed(const CellAdress& cell_address, CellDataPtr cell_data);

	CellDataPtr getCellData(const CellAdress& cell_address) const;
	
	void setCellData(const CellAdress& cell_address, CellDataPtr cell_data);

	std::vector<CellDataPtr> getCellDatas() const;
private:

	std::unordered_map<CellAdress, CellDataPtr, CellAdressHasher> mCellData;
	nsGameCore::GameCore& mrGameCore;
};



} //namespace
#pragma once
#include <iostream>

#include <osg/Vec3i>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
namespace nsGameCore{

class Terrain;
class GameCore;
class GameModel;


struct CellAdress
{

	CellAdress()
		:coords(-1,-1,-1)
	{

	}

	CellAdress(const osg::Vec3i& pos )
	{
		coords = pos;
	}

	bool operator ==(const CellAdress& rhs) const
	{
		return coords == rhs.coords;
	}

	friend std::size_t hash_value(const CellAdress& input)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, input.coords[0]);
		boost::hash_combine(seed, input.coords[1]);
		boost::hash_combine(seed, input.coords[2]);
		return seed;

	}

	void print()
	{
		std::cout << coords[0] << " " << coords[1] << std::endl;
	}

	osg::Vec3i coords;
	
};



struct CellData
{
	CellData(const CellAdress& cell_address, boost::shared_ptr<GameModel>& model_instance)
		:address(cell_address)
		,model_instance(model_instance)
	{

	}
	CellAdress address;
	boost::shared_ptr<GameModel> model_instance;
};

typedef boost::shared_ptr<CellData> CellDataPtr;

class GameArea
{
	
public:
	GameArea(nsGameCore::GameCore& ref_game_core);

	bool isFree(const CellAdress& cell_address) const;

	void setUsed(const CellAdress& cell_address, CellDataPtr cell_data);

	CellDataPtr getCellData(const CellAdress& cell_address) const;
	
	void setCellData(const CellAdress& cell_address, CellDataPtr cell_data);

	
protected:

	boost::unordered_map<CellAdress, CellDataPtr> mCellData;
	nsGameCore::GameCore& mrGameCore;
};



} //namespace
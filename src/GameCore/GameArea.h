#pragma once
#include <iostream>

#include <osg/Vec3i>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
namespace nsGameCore{

class Terrain;
class GameCore;


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
	CellAdress adress;

};

typedef boost::shared_ptr<CellData> CellDataPtr;

class GameArea
{
	
public:
	GameArea(nsGameCore::GameCore& ref_game_core)
		:mrGameCore(ref_game_core)
	{
	}

	bool isFree(const CellAdress& cell_address) const
	{
		return (0 == mCellData.count(cell_address));
	}

	void setUsed(const CellAdress& cell_address, CellDataPtr cell_data)
	{
		mCellData[cell_address] = cell_data;
	}

	CellDataPtr getCellData(const CellAdress& cell_address) const
	{
		if (isFree(cell_address))
		{
			return CellDataPtr();
		}
		return mCellData.at(cell_address);
	}
	void setCellData(const CellAdress& cell_address, CellDataPtr cell_data)
	{
		if (!isFree(cell_address))
		{
			return;
		}
		mCellData[cell_address] = cell_data;
	}

	
protected:

	boost::unordered_map<CellAdress, CellDataPtr> mCellData;
	nsGameCore::GameCore& mrGameCore;
};



} //namespace
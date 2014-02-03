#include "GameArea.h"

using namespace nsGameCore;


GameArea::GameArea( nsGameCore::GameCore& ref_game_core ) :mrGameCore(ref_game_core)
{

}

void GameArea::setCellData( const CellAdress& cell_address, CellDataPtr cell_data )
{
	if (!isFree(cell_address))
	{
		return;
	}
	mCellData[cell_address] = cell_data;
}

CellDataPtr GameArea::getCellData( const CellAdress& cell_address ) const
{
	if (isFree(cell_address))
	{
		return CellDataPtr();
	}
	return mCellData.at(cell_address);
}

void GameArea::setUsed( const CellAdress& cell_address, CellDataPtr cell_data )
{
	mCellData[cell_address] = cell_data;
}

bool nsGameCore::GameArea::isFree( const CellAdress& cell_address ) const
{
	return (0 == mCellData.count(cell_address));
}


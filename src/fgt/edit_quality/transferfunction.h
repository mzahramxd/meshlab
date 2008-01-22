/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
#ifndef _TRANSFER_FUNCTION_H_
#define _TRANSFER_FUNCTION_H_

//eliminare questo define in fase di release
#define NOW_TESTING


#include <vcg/math/base.h>
#include <vcg/space/color4.h>
//#include <vector>
#include <map>
//#include <algorithm>
#include <cassert>

#include <QString>
//#include <assert.h>
#include "const_types.h"

#include "util.h"

using namespace std;
using namespace vcg;


//struct used to represent each point in the transfer function.
//It's composed of a position on x axis and two values on the y axis (potentially the same)
struct TF_KEY
{
	enum
	{
		LOWER_Y = 0,
		UPPER_Y
	};

	float	y_upper;
	float	y_lower;
	bool	left_junction_point_code;
	bool	right_junction_point_code;

	float getLeftJunctionPoint() { return left_junction_point_code == LOWER_Y ? y_lower : y_upper; }
	float getRightJunctionPoint() { return right_junction_point_code == LOWER_Y ? y_lower : y_upper; }
	void  setLeftJunctionPoint( bool j_p )	{ left_junction_point_code = j_p; right_junction_point_code = 1-left_junction_point_code; }
	void  setRightJunctionPoint( bool j_p )	{ right_junction_point_code = j_p; left_junction_point_code = 1-right_junction_point_code; }
	
	TF_KEY( float y_up=0.0f, float y_low=0.0f )
	{
		y_upper=y_up;
		y_lower=y_low;
		if ( y_upper < y_lower)
			this->swapY();
		this->setLeftJunctionPoint(LOWER_Y);
	}

	void swapY() { float tmp=y_lower; y_lower=y_upper; y_upper=tmp; }
	bool operator == (TF_KEY k)	{ return ( (y_lower == k.y_lower) && (y_upper == k.y_upper) ); }
};

#define TF_KEYsize	sizeof(TF_KEY)


//list of channels
enum TF_CHANNELS
{
	RED_CHANNEL = 0,
	GREEN_CHANNEL,
	BLUE_CHANNEL,
	NUMBER_OF_CHANNELS
};

//defines a to class to menage the keys for a single channel
class TfChannel
{
public:
	//container and iterator for TF KEYs
	typedef	map<float, TF_KEY> KEY_LIST;
	typedef	map<float, TF_KEY>::iterator KEY_LISTiterator;

	TfChannel(void);
	TfChannel(TF_CHANNELS type);
	~TfChannel(void);

	void	setType(TF_CHANNELS);
	TF_CHANNELS getType();
	TF_KEY	*addKey(float x, float y_up, float y_bot);
	TF_KEY	*addKey(float x, TF_KEY& new_key);
	float	removeKey(float x);
	float	removeKey(TF_KEY& to_remove_key);
	TF_KEY	*mergeKeys(float x_pos1, TF_KEY& x_pos2);

	float	getChannelValuef(float x_position);
	UINT8	getChannelValueb(float x_position);

	TF_KEY* operator [](float i)	{ KEY_LISTiterator it= KEYS.find(i); if (it!=KEYS.end()) return &it->second; else return 0;}
	TF_KEY* operator [](int i);
	inline int size()	{	return KEYS.size();	}

#ifdef NOW_TESTING
	void testInitChannel();
#endif

private:
	TF_CHANNELS	_type;
	int old_iterator_idx;


	//list of keys
	KEY_LIST	KEYS;
	KEY_LISTiterator idx_it;
};




//Representation of a transfer function as a triple of vectors of Keys, 
//one for each color (RGB)
class TransferFunction
{
private:
	TfChannel	_channels[NUMBER_OF_CHANNELS];			//set of channels
	int			_channels_order[NUMBER_OF_CHANNELS];	//array used to carry out virtual pivoting indexing
	Color4f		_color_band[COLOR_BAND_SIZE];		/*rendere color band una classe a se stante??*/

	void initTF(void);

public:
	TransferFunction(void);
	TransferFunction(QString colorBandFile);
	~TransferFunction(void);

	TfChannel& operator [](int i)	{ return _channels[_channels_order[i]];	}
	int size();
	void buildColorBand();
	void saveColorBand();
};

#endif

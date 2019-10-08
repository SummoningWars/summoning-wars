/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "minimap.h"
#include <algorithm>
// debugging
#include <iostream>

Minimap::Minimap(short dimx, short dimy)
{
	m_dimx = dimx;
	m_dimy = dimy;
	m_data.resize((dimx*dimy +7)/8);
	std::fill(m_data.begin(), m_data.end(),0);
}

void Minimap::update(Vector playerpos)
{
	// Position des Spielers
	short sx = (short) (playerpos.m_x/4);
	short sy = (short) (playerpos.m_y/4);
	
	short xmin = MathHelper::Max(sx-4,0);
	short ymin = MathHelper::Max(sy-4,0);	
	short xmax = MathHelper::Min(sx+4,m_dimx-1);
	short ymax = MathHelper::Min(sy+4,m_dimy-1);
	
	for (short x = xmin; x <= xmax; ++x)
	{
		for (short y = ymin; y <= ymax; ++y)
		{	
			setData(x, y);
		}
	}
}

void Minimap::merge(Minimap& other)
{
	if (other.m_dimx != m_dimx || other.m_dimy != m_dimy)
	{
		ERRORMSG("Dimension mismatch");
		return;
	}
	
	for (unsigned int i=0; i<m_data.size(); ++i)
	{
		m_data[i] |= other.m_data[i];
	}
}


void Minimap::print()
{
	for (short y = 0; y < m_dimy; ++y)
	{
		for (short x =0; x <= m_dimx; ++x)
		{
			std::cout << (int) getData(x,y);
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void Minimap::toString(CharConv* cv)
{
	cv->toBuffer(m_dimx);
	cv->toBuffer(m_dimy);
	cv->toBuffer(static_cast<int>(m_data.size()));
	for (unsigned int i=0; i< m_data.size(); ++i)
	{
		cv->toBuffer(m_data[i]);
	}
}

void Minimap::fromString(CharConv* cv)
{
	cv->fromBuffer(m_dimx);
	cv->fromBuffer(m_dimy);
	int size;
	cv->fromBuffer(size);
	m_data.resize(size);
	char tmp;
	for (unsigned int i=0; i < (unsigned int) size; ++ i)
	{
		cv->fromBuffer(tmp);
		m_data[i] = tmp;
	}
}


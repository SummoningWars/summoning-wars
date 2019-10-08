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

#include "random.h"

int Random::randDiscrete(float* distribution,int nr, float sum)
{
	float x = randf(sum);
	for (int i=0;i<nr;i++)
	{
		x-= distribution[i];
		if (x<=0)
		{
			return i;
		}
	}
	return 0;
}


int Random::randDiscrete(std::vector<float> distribution)
{
	std::vector<float>::iterator i;
	float sum =0;
	for (i= distribution.begin(); i!=distribution.end(); ++i)
	{
		sum += *i;
	}
	float x = randf(sum);
	int res =0;
	for (i=distribution.begin();i!=distribution.end();++i)
	{
		x-= *i;
		if (x<=0)
		{
			return res;
		}
		res++;
	}
	return 0;
}




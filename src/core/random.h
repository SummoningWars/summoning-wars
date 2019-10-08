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

#ifndef RANDOM_H
#define RANDOM_H

#include <math.h>
#include <stdlib.h>
#include "debug.h"
#include <vector>
#include <list>

/**
 * \class Random
 * \brief Klasse fuer die Erzeugung von Zufallszahlen
 */
class Random
{
#define rez_rand_max  1.0 / (1.0 + RAND_MAX)
	
	public:
		
	/**
	 * \fn static float random()
	 * \return Zufallszahl zwischen 0 und 1
	 */
	static float random()
	{
		return rand() *rez_rand_max;
	}
	
	/**
	 * \fn static float randf(float f)
	 * \param f Obergrenze
	 * \return Zufallszahl zwischen 0 und f
	 */
	static float randf(float f)
	{
		return random()*f;
	}
	
	/**
	 * \fn static int randi(int i)
	 * \param i Obergrenze
	 * \return Zufallszahl zwischen 0 und i-1
	 */
	static int randi(int i)
	{
		return int(random()*i);
	}
	
	/**
	 * \fn static float randrangef(float f1, float f2)
	 * \param f1 Untergrenze
	 * \param f2 Obergrenze
	 * \return Zufallszahl zwischen f1 und f2
	 */
	static float randrangef(float f1, float f2)
	{
		return f1 + randf(f2-f1);
	}
	
	/**
	 * \fn static int randrangei(int i1, int i2)
	 * \param i1 Untergrenze
	 * \param i2 Obergrenze
	 * \return Zufallszahl zwischen i2 und i2
	 */
	static int randrangei(int i1, int i2)
	{
		return i1 + randi(i2-i1+1);
	}
	
	/**
	 * \fn 	int randDiscrete(float* distribution,int nr, float sum)
	 * \brief Sucht unter eine Menge von Ereignissen mit einer gewissen Wahrscheinlichkeit eines aus
	 * \param distribution Liste der Wahrscheinlichkeiten der Ereignisse
	 * \param nr Anzahl der Ereignisse
	 * \param sum Summe der Einzelwahrscheinlichkeiten (erspart normieren)
	 * \return Nummer des eingetreteten Ereignisses
	 */
	static int randDiscrete(float* distribution,int nr, float sum=1);
	
	/**
	 * \fn static int randDiscrete(std::vector<float> distribution)
	 * \brief Sucht unter eine Menge von Ereignissen mit einer gewissen Wahrscheinlichkeit eines aus
	 * \param distribution Liste der Wahrscheinlichkeiten der Ereignisse
	 * \return Nummer des eingetreteten Ereignisses
	 */
	static int randDiscrete(std::vector<float> distribution);
	

};

//float Random::rez_rand_max = 1.0 / RAND_MAX;

#endif


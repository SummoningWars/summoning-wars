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

#ifndef DROPSLOT_H
#define DROPSLOT_H

#include "item.h"

/**
 * \struct DropSlot
 * \brief charakterisiert ein Item, das ein Monster droppen kann
 */
struct DropSlot
{
	
	
	/**
	 * \var float m_size_probability[5]
	 * \brief Wahrscheinlichkeit mit der ein Item einer bestimmten Groesse gedroppt wird
	 */
	float m_size_probability[5];
	
	/**
	 * \brief minimales Level, das das gedroppte Item haben muss
	 */
	int m_min_level;
	
	/**
	 * \var int m_max_level
	 * \brief maximales Level das das gedroppte Item haben darf
	 */
	int m_max_level;
	
	/**
	 * \var int m_min_gold
	 * \brief Anzahl Gold die mindestens droppt, wenn Gold ausgewuerfelt wurde
	 */
	int m_min_gold;
	
	/**
	 * \var int m_max_gold
	 * \brief Anzahl Gold die maximal droppt, wenn Gold ausgewuerfelt wurde
	 */
	int m_max_gold;

	
	/**
	 * \var float m_magic_probability
	 * \brief Chance, das der Gegenstand magisch wird
	 */
	float m_magic_probability;
	
	/**
	 * \var float m_magic_power
	 * \brief Staerke der Verzauberung, falls der Gegenstand magisch wird. 
	 */
	float m_magic_power;
	
	/**
	 * \fn DropSlot()
	 * \brief Konstruktor. Belegt die Daten so, dass nie ein Gegenstand erzeugt wird
	 */
	DropSlot()
	{
		for (int i=0;i<5;i++)
		{
			m_size_probability[i]=0;
		}
		
		m_min_level =-1;
		m_max_level =-1;
		m_magic_probability =0;
		m_magic_power =1;
	};
	
	/**
	 * \fn void operator=(DropSlot& other)
	 * \brief Kopieroperator
	 */
	void operator=(DropSlot& other)
	{
		for (int i=0; i<5; i++)
			m_size_probability[i]= other.m_size_probability[i];
		m_magic_power = other.m_magic_power;
		m_magic_probability = other.m_magic_probability;
		m_max_gold = other.m_max_gold,
		m_min_gold = other.m_min_gold;
		m_min_level = other.m_min_level;
		m_max_level = other.m_max_level;
	}
	
	/**
	 * \fn void init(float size_prob[4], int min_level, int max_level, float magic_prob, float magic_power)
	 * \brief Belegt die Daten der Struktur
	 * \param size_prob Wahrscheinlichkeit mit der ein Item einer bestimmten Groesse gedroppt wird
	 * \param min_level minimales Level, das das gedroppte Item haben muss
	 * \param max_level  maximales Level das das gedroppte Item haben darf
	 * \param magic_prob Chance, das der Gegenstand magisch wird
	 * \param magic_power Staerke der Verzauberung, falls der Gegenstand magisch wird.
	 */
	void init(float size_prob[4], int min_level, int max_level, float magic_prob, float magic_power)
	{
		float sum =0;
		for (int i=0;i<4;i++)
		{
			m_size_probability[i]=size_prob[i];
			sum += size_prob[i];
		}
		
		m_size_probability[4] = 1-sum;
		
		m_min_level = min_level;
		m_max_level =max_level;
		m_magic_probability =magic_prob;
		m_magic_power =magic_power;		
	}
};

/**
 * \struct DropChance
 * \brief Chance, dass ein Items dropt
 */
struct DropChance
{
	/**
	 * \var int m_level
	 * \brief Level ab dem das Item droppen kann
	 */
	int m_level;
	
	/**
	 *  \var float m_probability
	 * \brief relative Wahrscheinlichkeit, dass das Item droppt
	 */
	float m_probability;
	
	/**
	 * \var Item::Size m_size
	 * \brief Groesse des Items
	 */
	Item::Size m_size;
	
	/**
	 * \fn DropChance(int level, float prob, Item::Size size)
	 * \brief Erzeugt neue DropChance Struktur
	 * \param level Level ab dem das Item droppen kann
	 * \param prob relative Wahrscheinlichkeit
	 * \param size Groesse des Items
	 */
	DropChance(int level, float prob, Item::Size size)
	{
		m_level = level;
		m_probability = prob;
		m_size = size;
	}
};

#endif



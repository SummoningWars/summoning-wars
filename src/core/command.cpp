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

#include "command.h"

void Command::toString(CharConv* cv)
{
	cv->toBuffer(m_type);
	cv->toBuffer(m_goal.m_x);
	cv->toBuffer(m_goal.m_y);
	cv->toBuffer(m_goal_object_id);
	cv->toBuffer(m_range);
	cv->toBuffer(m_flags);		
}

void Command::fromString(CharConv* cv)
{
	cv->fromBuffer(m_type);
	cv->fromBuffer(m_goal.m_x);
	cv->fromBuffer(m_goal.m_y);
	cv->fromBuffer(m_goal_object_id);
	cv->fromBuffer(m_range);
	cv->fromBuffer(m_flags);
	m_damage_mult =1;	
}
		
		
bool Command::operator!=(Command& other)
{
	return (m_type != other.m_type || m_goal_object_id != other.m_goal_object_id 
			|| m_goal.m_x != other.m_goal.m_x
			|| m_goal.m_y != other.m_goal.m_y);
			
	
}

void Command::operator=(Command& other)
{
	m_type = other.m_type;
	m_goal = other.m_goal;
	m_goal_object_id= other.m_goal_object_id;
	m_damage_mult=other.m_damage_mult;
	m_range= other.m_range;
	m_flags =other.m_flags;
}


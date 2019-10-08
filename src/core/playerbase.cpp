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

#include "playerbase.h"
#include "world.h"

void PlayerLook::operator=(PlayerLook other)
{
	m_gender = other.m_gender;
	m_render_info = other.m_render_info;
	m_emotion_set = other.m_emotion_set;
	m_name = other.m_name;
	m_hair = other.m_hair;
	m_item_suffix = other.m_item_suffix;
}

void PlayerLook::toString(CharConv* cv)
{
	cv->toBuffer(static_cast<char>(m_gender));
	cv->toBuffer(m_render_info);
	cv->toBuffer(m_emotion_set);
	if (World::getWorld()->getVersion() >= 13)
	{
		cv->toBuffer(m_hair);
		cv->toBuffer(m_item_suffix);
	}
}

void PlayerLook::fromString(CharConv* cv)
{
	char tmp;
	cv->fromBuffer(tmp);
	m_gender = (Gender) tmp;
	cv->fromBuffer(m_render_info);
	cv->fromBuffer(m_emotion_set);
	if (cv->getVersion() >= 13)
	{
		cv->fromBuffer(m_hair);
		cv->fromBuffer(m_item_suffix);
	}
}


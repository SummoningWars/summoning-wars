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

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

#include "gameinfotab.h"
#include <sstream>

#include "player.h"
#include "region.h"
#include "gameobject.h"


void GameInfoTab::init(CEGUI::Window* parent)
{
	ContentEditorTab::init(parent);
  m_rootWindow = parent;
	
  m_ShowBox = static_cast<CEGUI::MultiLineEditbox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/GameInfoTab/TextBox"));
}

void GameInfoTab::update()
{
	std::stringstream outs;
	
	if(World::getWorld() && World::getWorld()->getLocalPlayer())
	{
		Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());
		outs << "========= Player infos =========" << std::endl;
		outs << "Position: X " << player->getPosition().m_x << " Y " << player->getPosition().m_y << std::endl << std::endl;
	}

	outs << m_object_info;
	m_ShowBox->setText(outs.str());
}

void GameInfoTab::setClickedObjectID(int object)
{
	m_ClickedObjectID = object;
	getObjectInfos();
}

void GameInfoTab::getObjectInfos()
{
	std::stringstream outs;
	outs << "========= Clicked Object Infos =========" << std::endl;
	outs << "Object ID:       " << m_ClickedObjectID << std::endl;

	Player* player = static_cast<Player*>(World::getWorld()->getLocalPlayer());
	Region* reg = player->getRegion();
	GameObject *go = reg->getGameObject(m_ClickedObjectID);
	outs << "Object Name:     " << go->getName().getRawText() << std::endl;
	outs << "Object Name ID:  " << go->getNameId() << std::endl;
	outs << "Object Type:     " << go->getType() << std::endl;
	outs << "Object Subtype:  " << go->getSubtype() << std::endl;
	outs << "Object Layer:    " << gameObjectLayerToString(go) << std::endl;
	outs << "Object State:    " << gameObjectStateToString(go) << std::endl;
	outs << "Object BaseType: " << gameObjectBaseTypeToString(go) << std::endl;
	m_object_info = outs.str();
}

CEGUI::String GameInfoTab::gameObjectLayerToString(GameObject *go)
{
	short s = go->getLayer();

	switch(s)
	{
	case GameObject::LAYER_BASE:
		return CEGUI::String(std::string("LAYER_BASE"));
	case GameObject::LAYER_AIR:
		return CEGUI::String(std::string("LAYER_AIR"));
	case GameObject::LAYER_DEAD:
		return CEGUI::String(std::string("LAYER_DEAD"));
	case GameObject::LAYER_COLLISION:
		return CEGUI::String(std::string("LAYER_COLLISION"));
	case GameObject::LAYER_NOCOLLISION:
		return CEGUI::String(std::string("LAYER_NOCOLLISION"));
	case GameObject::LAYER_ALL:
		return CEGUI::String(std::string("LAYER_ALL"));
	case GameObject::LAYER_SPECIAL:
		return CEGUI::String(std::string("LAYER_SPECIAL"));
	default:
		return CEGUI::String(std::string("No Layer"));
	}
}

CEGUI::String GameInfoTab::gameObjectStateToString(GameObject *go)
{
	GameObject::State s = go->getState();

	switch(s)
	{
	case GameObject::STATE_INACTIVE:
		return CEGUI::String(std::string("STATE_INACTIVE"));
	case GameObject::STATE_ACTIVE:
		return CEGUI::String(std::string("STATE_ACTIVE"));
	case GameObject::STATE_DIEING:
		return CEGUI::String(std::string("STATE_DIEING"));
	case GameObject::STATE_DEAD:
		return CEGUI::String(std::string("STATE_DEAD"));
	case GameObject::STATE_REGION_DATA_REQUEST:
		return CEGUI::String(std::string("STATE_REGION_DATA_REQUEST"));
	case GameObject::STATE_REGION_DATA_WAITING:
		return CEGUI::String(std::string("STATE_REGION_DATA_WAITING"));
	case GameObject::STATE_ENTER_REGION:
		return CEGUI::String(std::string("STATE_ENTER_REGION"));
	case GameObject::STATE_QUIT:
		return CEGUI::String(std::string("STATE_QUIT"));
	case GameObject::STATE_STATIC:
		return CEGUI::String(std::string("STATE_STATIC"));
	case GameObject::STATE_SPEAK:
		return CEGUI::String(std::string("STATE_SPEAK"));
	case GameObject::STATE_NONE:
		return CEGUI::String(std::string("STATE_NONE"));
	case GameObject::STATE_AUTO:
		return CEGUI::String(std::string("STATE_AUTO"));
	case GameObject::STATE_FLYING:
		return CEGUI::String(std::string("STATE_FLYING"));
	case GameObject::STATE_EXPLODING:
		return CEGUI::String(std::string("STATE_EXPLODING"));
	case GameObject::STATE_GROWING:
		return CEGUI::String(std::string("STATE_GROWING"));
	case GameObject::STATE_STABLE:
		return CEGUI::String(std::string("STATE_STABLE"));
	case GameObject::STATE_VANISHING:
		return CEGUI::String(std::string("STATE_VANISHING"));
	case GameObject::STATE_DESTROYED:
		return CEGUI::String(std::string("STATE_DESTROYED"));
	default:
		return CEGUI::String(std::string("No State"));
	}
}

CEGUI::String GameInfoTab::gameObjectBaseTypeToString(GameObject *go)
{
	GameObject::BaseType s = go->getBaseType();

	switch(s)
	{
	case GameObject::WORLDOBJECT:
		return CEGUI::String(std::string("WORLDOBJECT"));
	case GameObject::PROJECTILE:
		return CEGUI::String(std::string("PROJECTILE"));
	case GameObject::DROPITEM:
		return CEGUI::String(std::string("DROPITEM"));
	default:
		return CEGUI::String(std::string("No BaseType"));
	}
}

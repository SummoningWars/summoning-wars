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

#ifndef __SUMWARS_GUI_GRAPHICOBJECT_H__
#define __SUMWARS_GUI_GRAPHICOBJECT_H__

#include "OgreMovableObject.h"
#include "OgreSceneNode.h"
#include "OgreTagPoint.h"
#include "OgreBone.h"
#include "OgreSceneManager.h"

#include <string>
#include "graphicobjectrenderinfo.h"
#include "soundobject.h"

class GraphicObject;

/**
 * \brief Structure to store information on a MovableObject attached to the SceneTree of the Graphicobject
 * Ogre Tagpoints do not allow Scenenodes to be attached to them. That is why MovableObjects that should be attached to a bone are attached to the parent node of the entity instead. There is an additional Scenenode inserted inbetween that follows the movement of the Bone and therefore imitates that the object is attached to the Bone.
 */
struct AttachedMovableObject
{
	/**
	 * \brief OGRE ressource name of the Object
	 */
	MovableObjectInfo m_object_info;
	
	/**
	 * \brief Pointer to the object
	 */
	Ogre::MovableObject* m_object;
	
	/**
	 * \brief Tagpoint that defines the position of the object (if attached to a bone).
	 * The Object is actually not attached to this tagpoint, but the parent scenenode of the object follows this tagpoint.
	 * Pointer is NULL if the Object is not attached to a bone.
	 */
	Ogre::TagPoint* m_tagpoint;
	
	/**
	 * \brief Scenenode that follows the position of the Tagpoint
	 */
	Ogre::SceneNode* m_tag_trackpoint;
	
	/**
	 * \brief If object is attached to a bone, this pointer points to the Entity the bone belongs to
	 */
	Ogre::Entity* m_entity;
	
	/**
	 * \brief For highlighting, entities are duplicated and rendered with another texture. This is the duplicate
	 */
	Ogre::Entity* m_highlight_entity;
};


/**
 * \brief Structure to store information on a Graphikobject attached to a Scenetree of a Graphicobject (socalled Subobject)
 * Ogre Tagpoints do not allow Scenenodes to be attached to them. That is why GraphicObjects that should be attached to a bone are attached to the parent node of the entity instead. There is an additional Scenenode inserted inbetween that follows the movement of the Bone and therefore imitates that the object is attached to the Bone.
 */
struct AttachedGraphicObject
{ 
	/**
	 * \brief pointer to the object
	 */
	GraphicObject* m_object;
	
	/**
	 * \brief Tagpoint that defines the position of the object (if attached to a bone).
	 * The Object is actually not attached to this tagpoint, but the parent scenenode of the object follows this tagpoint.
	 * Pointer is NULL if the Object is not attached to a bone.
	 */
	Ogre::TagPoint* m_tagpoint;
	
	/**
	 * \brief If object is attached to a bone, this pointer points to the Entity the bone belongs to
	 */
	Ogre::Entity* m_entity;
	
	/**
	 * \brief Scenenode that follows the position of the Tagpoint
	 */
	Ogre::SceneNode* m_tag_trackpoint;
};

/**
 * \brief Structure used to store the dependencies among the Objects in the Scene Tree
 * All objects are stored by name and retrieved from the data structures when required
 */
struct ObjectDependency
{
	/**
	 * \brief List of children
	 */
	std::set<std::string> m_children;
	
	/**
	 * \brief The parent. Empty string in case of the root
	 */
	std::string m_parent;
};

/**
 * \brief Structure that stores information on an action that is currently displayed be the Graphicobject
 */
struct AttachedAction
{
	/**
	 * \brief Constructor
	 */
	AttachedAction():
		m_arinfo(0),
		m_current_percent(0),
		m_time(0),
		m_inherited(false)
	{}
	
	/**
	 * \brief Pointer to data structure that contains information how to visualize this action
	 */
	ActionRenderInfo* m_arinfo;
	
	/**
	 * \brief List of all Renderparts that are active at the moment
	 */
	std::list<ActionRenderpart*> m_active_parts;
	
	/**
	 * \brief Name of the action
	 */
	std::string m_current_action;
		
	/**
	 * \brief share of the overall time of the action that is elapsed (number between 0 and 1)
	 */
	float m_current_percent;
	
	/**
	 * \brief overall duration of the action
	 */
	float m_time;
	
	/**
	 * \brief true, if the ActionRenderInfo was supplied by a parent RenderInfo
	 */
	bool m_inherited;
};

/**
 * \brief Stores Information on a State that is attached to a GraphicObject
 * States are mapped to actions internally. When a state is set, the action activate:statename is performed once, after that active:statename in a loop. On deactivation deactivate:statename is performed.
 */
struct AttachedState
{
	/**
	 * \brief enum for the phase of a state visualisation
	 */
	enum Type
	{
		ACTIVATE =1,
 		ACTIVE = 2,
 		DEACTIVATE = 3,
	};
	
	/**
	 * \brief current phase of the state visualisation
	 */
	Type m_type;
	
	/**
	 * \brief Action that is used to display the state
	 */
	AttachedAction m_attached_action;
};

/**
 * \class GraphicObject
 * \brief Class for the visual representation of a GameObject
 * GraphicObject renders a Gameobject based on a GraphicRenderInfo structure. On basis of this information  a Scene Subtree is built. This subtree may contain OGRE::MovableObjects and other GraphicObjects. 
 * GraphicObjects allow two inputs that influence the SceneTree: An Action with percent number of completion or a set of states. Renderinfo contains the information how to react on these inputs.
 */
class GraphicObject
{
	public:
		/**
		 * \brief Name of the Renderinfo
		 */
		typedef std::string Type;
		
		/**
		 * \brief Constructor
		 * \param type Name of the Renderinfo
		 * \param render_info pointer to the Renderinfo
		 * \param name name of the object
		 * \param ID usually the ID of the underlying GameObject
		 * A Special case is, that not Renderinfo is supplied and the type is the name of a Mesh. In this case the Object is represented by a single, static mesh.
		 */
		GraphicObject(Type type, GraphicRenderInfo* render_info, std::string name, int id);
		
		/**
		 * \brief Destructor
		 */
		~GraphicObject();
		
		/**
		 * \brief Creates the content objects from the RenderInfo
		 */
		void initContent();
		
		/**
		 * \brief Deletes all the content objects
		 */
		void clearContent();
		
		/**
		 * \brief Sets the action that is displayed
		 * \param action Name of the action
		 * \param percent elapsed time / overall time of the action
		 * \param random_action_nr multiple visualization exist the one with the supplied number is chosen. If parameter is 0 random visualization is chose. This is used to synchronize animations of different Graphicobjects in one Scene Tree
		 */
		void updateAction(std::string action, float percent, int random_action_nr=0);
		
		/**
		 * \brief Return the topmost node of the Scene tree of this Object
		 */
		Ogre::SceneNode* getTopNode()
		{
			return m_top_node;
		}
		
		/**
		 * \brief Sets a single state to active
		 * \param state name of the state
		 * \param active activation status
		 */
		void updateState(std::string state, bool active);
		
		/**
		 * \brief Sets exactly the list of given states as active
		 * \param states List of active states. All other states are implicitely set to inactive.
		 */
		void updateAllStates(std::set<std::string>& states);
		
		/**
		 * \brief Update the object after a time of \a time milliseconds has elapsed
		 * \param time elapsed time in ms
		 */
		void update(float time);
		
		/**
		 * \brief Updates the subobject to match the supplied state
		 * \param object Information on attached Subobject state
		 * This function is used update the player equipement to match the currently equiped items.
		 * \return true if a modification was neccesary, false otherwise
		 */
		bool updateSubobject(MovableObjectInfo& object); 
		
		/**
		 * \brief Returns the Renderinfo name
		 */
		Type getType()
		{
			return m_type;
		}
		
		/**
		 * \brief Applies the mask to all OGRE Objects in the Scenetree. This mask is checked by OGRE queries.
		 * \param mask mask
		 */
		void setQueryMask(unsigned int mask);
		
		/**
		 * \brief Returns the name of the Object
		 */
		std::string getName()
		{
			return m_name;
		}
		
		/**
		 * \brief Returns the ID
		 */
		int getId()
		{
			return m_id;
		}
		
		/**
		 * \brief Sets the highlighting state of the GraphicObject
		 * Highlighting is applied to all entities in the Scene tree. These entities are duplicated and rerendered with the supplied material
		 * \param highligh highlighting state
		 * \param material material used for highlighting
		 */
		void setHighlight(bool highlight, std::string material);
		
		/**
		 * \brief Sets visibility of the GameObject
		 * \param visible visibility state
		 */
		void setVisibility(bool visible);
		
		/**
		 * \brief Sets the exact animations flags
		 * \param exact_animations new flag value
		 */
		void setExactAnimations(bool exact_animations);
		
		/**
		 * \brief Replaces the RenderInfo used for this Object
		 * This functions clears all the objects contents and replaces it with the new RenderInfo contents
		 * \param renderinfo new RenderInfo
		 */
		void  replaceRenderInfo(GraphicRenderInfo* renderinfo);
		
		/**
		 * \brief Returns if the objects RenderInfo is valid
		 */
		bool getRenderInfoValid()
		{
			return (m_render_info_valid);
		}
		
		/**
		 * \brief Makes sure, that the renderinfo of the Object is valid.
		 * If the RenderInfo is invalid, it will try to reload it
		 */
		void ensureRenderInfoValid();
		
		/**
		 * \brief Invalidates the the RenderInfo of this object
		 * If an objects RenderInfo is set to invalid, the RenderInfo will be reloaded with \ref replaceRenderInfo on the next frame
		 */
		void invalidateRenderInfo();

		
	private:
		
		/**
		 * \brief Adds all initial objects specified in the Renderinfo
		 * \param info Renderinfo
		 */
		void addObjectsFromRenderInfo(GraphicRenderInfo* info);
		
		/**
		 * \brief Adds a new OGRE MovableObject or GraphicObject (function should be renamed to reflect that)
		 * \param object information for creation of the object
		 */
		void addMovableObject(MovableObjectInfo& object);
		
		/**
		 * \brief Removes the OGRE MovableObject or GraphicObject with the given name (function should be renamed to reflect that)
		 * \param name Name of an attached MovableObject or GraphicObject
		 */
		void removeMovableObject(std::string name);
		
		/**
		 * \brief Adds a component of rendering an action
		 * \param part pointer to the renderpart
		 */
		void addActiveRenderPart(ActionRenderpart* part);
		
		/**
		 * \brief removes a component of rendering an action
		 * \param part pointer to the renderpart
		 */
		void removeActiveRenderPart(ActionRenderpart* part);
		
		/**
		 * \brief Updates an action to the given percent of completion
		 * In contrast to the public updateAction function, this is also applied to the actions that are generated from states.
		 * \param attchaction data structure with information on the current state of the action
		 * \param action name of the action
		 * \param percent elapsed_time / overall_time of the action
		 */
		void updateAttachedAction(AttachedAction& attchaction, std::string action, float percent);
		
		/**
		 * \brief Updates the renderpart to the supplied local percent of completion.
		 * If the the Renderpart does not span the complete time of action local percent will differ from global percent of the action. Example: An animation takes part on during the first half (0-0.5) of an action. A global percent of 0.4 equals a local 0.8 for the animation.
		 * \param part pointer to the Renderpart
		 * \param relpercent local percent of the renterpart
		 */
		void updateRenderPart(ActionRenderpart* part, float relpercent);
		
		/**
		 * \brief Returns the MovableObject with the given \a name
		 * \param name name
		 *  \return pointer to the MovableObject, returns NULL is none was found
		 */
		Ogre::MovableObject* getMovableObject(std::string name);
		
		/**
		 * \brief Returns the duplicated MovableObject for highlighting
		 * \param name name of the MovableObject
		 * \return pointer to the MovableObject, returns NULL is none was found
		 */
		Ogre::MovableObject* getHighlightObject(std::string name);
		
		/**
		 * \brief Returns the attached entity with the name (essentially getMovableObject with dynamic cast)
		 * \param name name of the Entity
		 * \return pointer to the Entity, returns NULL is none was found
		 */
		Ogre::Entity* getEntity(std::string name);
		
		/**
		 * \brief Returns the Node that determines the position of a MovableObject of Graphicobject. If this Object is attached to a Bone it will return the Tagpoint, not the Scenenode that follows the Tagpoint.
		 * \param name Name of a Subobject
		 */
		Ogre::Node* getParentNode(std::string name);
		
		/**
		 * \brief initialisizes the attached Action data structure after an action has started
		 * \param attchaction Attached Action
		 * \param action name of the action
		 */
		void initAttachedAction(AttachedAction& attchaction, std::string action);
		
		
		/**
		 * \brief Name of the Renderinfo
		 */
		Type m_type;
		
		/**
		 * \brief List of Ogre Objects that are attached to the Graphicobject
		 */
		std::map<std::string, AttachedMovableObject> m_attached_objects;
		
		/**
		 * \brief List of Graphicsobjects (SubObjects) that are attached to the GraphicObject
		 */
		std::map<std::string, AttachedGraphicObject > m_subobjects;
		
		/**
		 * \brief List of attached Soundobjects
		 */
		//std::map <std::string, SoundObject*> m_soundobjects;
		std::map <std::string, SoundObject> m_soundobjects;
		//std::map<std::string, std::string> m_soundobjects;
		
		/**
		 * \brief Dependency for all Subobjects
		 */
		std::map<std::string, ObjectDependency > m_dependencies;
		
		/**
		 * \brief Information how to render this Object
		 */
		GraphicRenderInfo* m_render_info;
		
		
		/**
		 * \brief List of States and their current status data
		 */
		std::map<std::string,  AttachedState> m_attached_states;
		
		/**
		 * \brief top node of the Scene tree
		 */
		Ogre::SceneNode* m_top_node;
		
		/**
		 * \brief name of the object
		 */
		std::string m_name;
		
		/**
		 * \brief Action that is currently displayed
		 */
		AttachedAction m_action;
		
		/**
		 * \brief ID
		 */
		int m_id;
		
		/**
		 * \brief Highlighting state
		 */
		bool m_highlight;
		
		/**
		 * \brief If set to true, Scenenodes follow their Tagpoints exactly. This is costly and can be deactivated therefore. If set so false, information from previous frame is used
		 */
		bool m_exact_animations;
		
		/**
		 * \brief Material used for highlighting
		 */
		std::string m_hightlight_material;
		
		/**
		 * \brief Random number that determines which action is chosen more than one visualisation exists
		 * This is used to synchronise Animations of different Graphicobjects in the same SkillTree
		 */
		int m_random_action_number;
		
		/**
		 * \brief Is set true, if the random number for the action is generated by this Graphicobject
		 */
		bool m_own_random_number;
		
		/**
		 * \brief Signals if the objects Renderinfo is still valid
		 */
		bool m_render_info_valid;
};


#endif // __SUMWARS_GUI_GRAPHICOBJECT_H__

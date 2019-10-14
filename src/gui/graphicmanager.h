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

#ifndef __SUMWARS_GUI_GRAPHICMANAGER_H__
#define __SUMWARS_GUI_GRAPHICMANAGER_H__


#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "OgreParticleSystem.h"
#include "OgreEntity.h"
#include "OgreMovableObject.h"
#include "OgreStaticGeometry.h"
#include "graphicobject.h"
#include "graphicobjectrenderinfo.h"

#include <tinyxml.h>
#include <string>
#include <list>
#include <map>

#include <OgreRenderQueueListener.h> 
#include <OgreRenderSystem.h>
#include <OgreRenderQueue.h>
#include <OgreRenderQueueListener.h> 

#ifndef RENDER_QUEUE_MAIN
#define RENDER_QUEUE_MAIN 50
#endif

#define RENDER_QUEUE_HIGHLIGHT_MASK	RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_HIGHLIGHT_OBJECTS	RENDER_QUEUE_MAIN + 2
#define LAST_STENCIL_OP_RENDER_QUEUE	RENDER_QUEUE_HIGHLIGHT_OBJECTS

#define STENCIL_VALUE_FOR_OUTLINE_GLOW 1
#define STENCIL_VALUE_FOR_FULL_GLOW 2
#define STENCIL_FULL_MASK 0xFFFFFFFF

class StencilOpQueueListener : public Ogre::RenderQueueListener 
{ 
	public: 
		virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
		

		virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);

}; 

/**
 * \brief Graphic object creation and destruction class
 * Class that manages creation and destruction of Graphicobject and Ogre::Movableobject objects. 
 */
class GraphicManager
{
	public:
		/**
		 * \brief initialises the object
		 */
		static void init();
	
		/**
		 * \brief Frees memory of internal data structures
		 */
		static void cleanup();
	
		/**
		 * \brief Reads the given XML file and stores the contained Renderinfo data
		 * \param pFilename Renderinfo XML file name
		 */
		static void loadRenderInfoData(const char* pFilename);
		
		/**
		 * \brief Creates a new Graphicobject
		 * \param type Name of the Renderinfo used for this object
		 * \param name Name of the Object
		 * \param id ID of the created Object. Usually matches the ID of the underlying Gameobject
		 * \return  pointer to the new object. Returns NULL if creation was not succesful.
		 * \sa destroyGraphicObject
		 */
		static GraphicObject* createGraphicObject(GraphicObject::Type type, std::string name, int id);
		
		/**
		 * \brief Adds a static GraphicObject with the requested type to the scene
		 * \param type Name of the Renderinfo used for this object
		 * \param position position where the object should be placed
		 * \param angle Rotation about y-Achses (yaw)
		 * The Object is created if it does not exist in the static geometry yet. The object is automatically added to an appropriate static geometry. 
		 */
		static void insertStaticGraphicObject(std::string type, Ogre::Vector3 position, float angle);
		
		/**
		 * \brief Destroys a GraphicObject
		 * \param obj pointer to the object
		 * Currently this just calls delete on the pointer, put some caching might be done in future
		 * \sa createGraphic
		 */
		static void destroyGraphicObject(GraphicObject* obj);
		
		/**
		 * \brief Returns the Scenemanager that is used to create new Ogre Objects
		 * \sa setSceneManager
		 */
		static Ogre::SceneManager* getSceneManager()
		{
			return m_scene_manager;
		}
		
		/**
		 * \brief Returns the container for all static meshes
		 */
		static Ogre::StaticGeometry* getStaticGeometry();
		
		/**
		 * \brief builds the static geometry
		 */
		static void buildStaticGeometry();
		
		/**
		 * \brief Sets the Scenemanager that is used to create new Ogre Objects
		 * \param scene_manager OGRE SceneManager
		 * \sa getSceneManager()
		 */
		static void setSceneManager(Ogre::SceneManager* scene_manager)
		{
			m_scene_manager = scene_manager;
		}
		
		/**
		 * \brief Creates a new OGRE MovableObject based on the information supplied
		 * \param info Structure containing all required information on the new Object
		 * \param name Unique name of the new object
		 * It is not guaranteed that the object is really newly created. For performance reasons deleted objects may be cached and reused. It is guaranteed that the object return is can not be distinguished from a new object.
		 * \sa destroyMovableObject
		 */
		static Ogre::MovableObject* createMovableObject(MovableObjectInfo& info, std::string name);
		
		/**
		 * \brief Destroys a MovableObject
		 * \param obj pointer to the object
		 * For performance reasons the object might be cached instead of being destroyed and returned on the next create call.
		 * \sa createMovableObject
		 */
		static void destroyMovableObject(Ogre::MovableObject* obj);
		
		/**
		 * \brief Detaches a MovableObject from its parent node
		 * \param obj pointer to the object
		 */
		static void detachMovableObject(Ogre::MovableObject* obj);
		
		/**
		 * \brief Registers a mapping that describes which renderinfo is used to visualize an ingame object
		 * \param objecttype Ingame Object type
		 * \param renderinfo Renderinfo used to display objects of type \a objecttype
		 * \param silent_replace if set to true, the data is replaced without error message. 
		 * \sa getGraphicType
		 */
		static void registerGraphicMapping(std::string objecttype, GraphicObject::Type renderinfo, bool silent_replace = false);
		
		/**
		 * \brief Returns the Renderinfo that is used to display the given objecttype
		 * \param objecttype Ingame object type
		 * \return Renderinfo used to display objects of type \a objecttype
		 * Returns an empty string if no data was found
		 * \sa registerGraphicMapping
		 */
		static GraphicObject::Type getGraphicType(std::string objecttype);
		
		/**
		 * \brief Returns the RenderInfo data structure that corresponds to the supplied renderinfo name
		 * \param renderinfoname Name of a Renderinfo
		 * \param returns pointer to RenderInfo structure, NULL if no data was found
		 * \sa  registerRenderInfo
		 */
		static GraphicRenderInfo* getRenderInfo(std::string renderinfoname);
		
		/**
		 * \brief specialized functions for determining the dropsound of an item
		 * \param objecttype Ingame object type, must be an item type
		 */
		static std::string getDropSound(std::string objecttype);
		
		/**
		 * \brief clears the internal particle pool
		 */
		static void clearParticlePool();
		
		/**
		 * \brief clears the contents of static geometry and the set of static entities
		 */
		static void clearStaticGeometry();
		
		/**
		 * \brief Clears all RenderInfo Data
		 * This function invalidates the RenderInfo for the GraphicObject objects forcing them to reload their data.
		 * New RenderInfo data must be loaded in order to provide fresh, correct RenderInfo for them.
		 */
		static void clearRenderInfos();
		
		/**
		 * \brief Clears on specific RenderInfo
		 * \param name  Name of a Renderinfo
		 * Invalidates RenderInfo for all GraphicObjects using this RenderInfo, forcing them to reload their data. 
		 * A new RenderInfo data (with the same name) must be loaded in order to provide fresh, correct RenderInfo for them.
		 */
		static void clearRenderInfo(std::string name);
		
		/**
		 * \brief Marks the RenderInfo for all GraphicObjects as invalid
		 */
		static void invalidateGraphicObjects();
		
		/**
		 * \brief Registers a Renderinfo data structure with the given name
		 * \param name Name of a Renderinfo
		 * \param info corresponding data structure
		 * \param silent_replace if set to true, the data is replaced without error message. 
		 * The data can be retrieved by a getRenderInfo call
		 * \sa getRenderInfo
		 */
		static bool registerRenderInfo(std::string name, GraphicRenderInfo* info, bool silent_replace = false);
		
		/**
		 * \brief Reads the Renderinfos recursively from an XML node
		 * \param node XML node
		 * \param silent_replace if set to true, existing data is overwritten without error message
		 */
		static void loadRenderInfos(TiXmlNode* node, bool silent_replace = false);
		
		/**
		 * \brief Reads Renderinfo from an XML node
		 * \param node XML node
		 */
		static void loadRenderInfo(TiXmlNode* node, GraphicRenderInfo* info);
		
	private:
		
		
		/**
		 * \brief Returns an OGRE Particlesystem with the requested type
		 * \param type Type of a Particlesystem
		 * The particle system is taken from the particle pool. A new particle system is only created if there is none of the requested type in the pool
		 */
		static Ogre::ParticleSystem* getParticleSystem(std::string type);
	
		/**
		 * \brief Inserts the particle system into the particle pool
		 * \param part particle system
	 	*/
		static void putBackParticleSystem(Ogre::ParticleSystem* part);
		
		/**
		 * \brief Adds all entities in the subtree below the given scene node to the static geometry
		 * \param node Top node of the scene subtree
		 */
		static void addSceneNodeToStaticGeometry(Ogre::SceneNode* node);
		
		/**
		 * Data structure containg all Renderinfo data. Maps name of Renderinfos to the real data
		 */
		static std::map<std::string, GraphicRenderInfo*> m_render_infos;
		
		/**
		 * Data structure containing the mapping of ingame object types to the name of renderinfos used to display these objects
		 */
		static std::map<std::string, GraphicObject::Type> m_graphic_mapping;
		
		/**
		 * \brief all GraphicObject objects, ordered by ID
		 */
		static std::map<std::string, GraphicObject*> m_graphic_objects;
		
		/**
		 * \brief internal pool of objects in static geometry, ordered by renderinfo type
		 */
		static std::map<std::string, GraphicObject*> m_static_graphic_objects;
		
		
		
		/**
		 * \brief Reads MovableObjectInfo from an XML node
		 * \param node XML node
		 * \param info data structure where results are written to
		 */
		static void loadMovableObjectInfo(TiXmlNode* node, MovableObjectInfo* info);
		
		/**
		 * \brief Reads ActionRenderInfo from an XML node
		 * \param node XML node
		 * \param info data structure where results are written to
		 */
		static void loadActionRenderInfo(TiXmlNode* node, ActionRenderInfo* info);
		
		/**
		 * \brief OGRE SceneManager that is used to create all OGRE Objects
		 */
		static Ogre::SceneManager* m_scene_manager;
		
		/**
		 * \brief Helper Class that is used for highlighting (just copied from OGRE wiki)
		 */
		static StencilOpQueueListener* m_stencil_listener;
		
		/**
		 * \brief internal pool of Particle system
		 */
		static std::multimap<std::string, Ogre::ParticleSystem*> m_particle_system_pool;
		
		/**
		 * \brief Name of XML file being read at the moment
		 */
		static std::string m_filename;
		
	public:
		static double g_global_scale;
};

#endif // __SUMWARS_GUI_GRAPHICMANAGER_H__


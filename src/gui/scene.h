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

#ifndef __SUMWARS_GUI_SCENE_H__
#define __SUMWARS_GUI_SCENE_H__

#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreTextureManager.h"
#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreRenderTarget.h"
#include "OgreViewport.h"
#include "OgreStaticGeometry.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreRenderTexture.h"
#include <OgreSubMesh.h>

#include <map>
#include <string>

#include "itemloader.h"
#include "objectloader.h"
#include "document.h"
#include "worldobject.h"
//#include "sound.h"
#include "player.h"
#include "graphicobject.h"
#include "scriptobject.h"

/**
 * \class Scene
 * \brief Beschreibt die darzustellende Szene
 */
class Scene
{
	public:
		

	enum ObjectMask
	{
		WORLDOBJECT = 0x1,
		ITEM =  0x2,
	};

	/**
	 * \fn Scene(Document* doc,Ogre::RenderWindow* window)
	 * \brief Konstruktor
	 * \param doc Dokument dessen Daten dargestellt werden sollen
	 * \param window Fenster in das gerendert wird
	 */
	Scene(Document* doc,Ogre::RenderWindow* window);

	/**
	 * \fn ~Scene()
	 * \brief Destruktor
	 */
	~Scene();

	/**
	 * \fn void update(float ms)
	 * \brief aktualisiert die Szene
	 * \param ms Vergangene Zeit in ms seit dem letzten update
	 */
	void update(float ms=0);

	/**
	 * \fn Ogre::Camera* getCamera()
	 * \brief Gibt die Kamera aus, mit der die Szene aufgenommen wird
	 */
	Ogre::Camera* getCamera()
	{
		return m_camera;
	}

	/**
	 * \fn Ogre::Viewport* getViewport()
	 * \brief Gibt Viewport aus
	 */
	Ogre::Viewport* getViewport()
	{
		return m_viewport;
	}
	
	
	/**
	 * \fn std::pair<float,float> getProjection(Vector pos, float height=0)
	 * \brief Gibt aus, an welcher Stelle ein Punkt in Spielkoordinaten auf dem Bildschirm erscheint
	 * \param pos Ort in Spielkoordinaten
	 * \param height Hoehe in Spielkoordinaten
	 */
	std::pair<float,float> getProjection(Vector pos, float height=0);
	

	/**
	 * \fn void updateCharacterView()
	 * \brief aktualisiert die Ansicht des Spielers
	 */
	void updateCharacterView();

	
	/**
	 * \fn GraphicObject* getGraphicObject(int id)
	 * \brief Gibt GraphikObjekt mit der angegebenen ID aus
	 * \param id ID
	 */
	GraphicObject* getGraphicObject(int id);
	


	static void getMeshInformation(const Ogre::MeshPtr mesh, size_t &vertex_count, Ogre::Vector3* &vertices,  size_t &index_count,
						unsigned long* &indices,  const Ogre::Vector3 &position,  const Ogre::Quaternion &orient,  const Ogre::Vector3 &scale);
						
						
	/**
	 * \fn bool updatePlayerGraphicObject(GraphicObject* obj, Player* pl)
	 * \brief aktualisiert das GraphikObjekt eines Spielers
	 * \param obj GraphikObjekt
	 * \param pl Spielerobjekt
	 */
	static bool updatePlayerGraphicObject(GraphicObject* obj, Player* pl);
						
	
	private:

		
	/**
	 * \fn GraphicObject* createGraphicObject(GameObject* gobj, std::string name="")
	 * \brief erzeugt zu einem Spielobjekt das passende Graphicobjekt
	 * \param gobj Spielobjekt
	 * \param name Name (wenn leer, wird ein Name generiert)
	 */
	GraphicObject* createGraphicObject(GameObject* gobj, std::string name="");
	
	/**
	 * \fn void updateGraphicObject(GraphicObject* obj, GameObject* gobj)
	 * \brief aktualisiert das Graphicobjekt
	 * \param obj GraphicObjekt
	 * \param gobj zugehoeriges Spielobjekt
	 * \param time vergangene Zeit in ms
	 */
	void updateGraphicObject(GraphicObject* obj, GameObject* gobj,float time);
	
	
	
	/**
	 * \fn void updateGraphicObjects(float time)
	 * \brief aktualisiert alle GraphicObjekte
	 * \param time vergangene Zeit in ms
	 */
	void updateGraphicObjects(float time);
	
	/**
	 * \fn void  insertObject(GameObject* obj,std::string& name, bool is_static)
	 * \brief Erzeugt auf Basis eines Objektes eine Ogre Objekt
	 * \param obj Objekt fuer das eine Ogre Entity erzeugt werden soll
	 * \param is_static gibt an, ob das Objekt statisch ist
	 */
	void  insertObject(GameObject* obj, bool is_static = false);

	/**
	 * \fn void deleteGraphicObject(int id)
	 * \brief loescht ein Graphicobjekt
	 * \param id ID des Objekts
	 */
	void deleteGraphicObject(int id);
	
	/**
	 * \fn void clearObjects()
	 * \brief Entfernt alle Objekte aus der Szene
	 */
	void clearObjects();
	
	/**
	 * \var void createScene()
	 * \brief Erstellt die Szene neu
	 */
	void createScene();


	/**
	 * \fn void registerMeshes()
	 * \brief Registriert fuer alle Objekte, Projektile, Items und Tiles die zugehoerigen Meshes
	 */
	static void registerMeshes();
	
	/**
	 * \var std::map<int,GraphicObject*> m_graphic_objects
	 * \brief Graphikobjekte fuer alle nicht statischen Objekte
	 */
	std::map<int,GraphicObject*> m_graphic_objects;
	
	
	


	/**
	 * \var Document* m_document
	 * \brief Dokument, dessen Daten dargestellt werden sollen
	 */
	Document* m_document;
	
	/**
	 * \var short m_region_id
	 * \brief ID der Region, die gerade angezeigt wird
	 */
	short m_region_id;

	/**
	 * \var Ogre::RenderWindow* m_window
	 * \brief Fenster in dem die Szene gerendert wird
	 */
	Ogre::RenderWindow* m_window;

	/**
	 * \var Ogre::Camera* m_camera
	 * \brief Kamera mit der die Szene betrachtet wird
	 */
	Ogre::Camera* m_camera;
	

	/**
	 * \var Ogre::Viewport* m_viewport
	 * \brief Darstellungsflaeche in die gerendert wird
	 */
	Ogre::Viewport* m_viewport;

	/**
	 * \var Ogre::SceneManager* m_scene_manager
	 * \brief aktueller Scenemanager
	 */
	Ogre::SceneManager* m_scene_manager;
	
	
	/**
	 * \var std::string m_temp_player
	 * \brief Vorschau Objekt auf den aktuellen Spieler
	 */
	std::string m_temp_player;
	
	/**
	 * \var GraphicObject* m_temp_player_object
	 * \brief GraphikObjekt fuer den temporaeren Spieler
	 */
	GraphicObject* m_temp_player_object;
	
};

#endif // __SUMWARS_GUI_SCENE_H__


#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "graphicobject.h"
struct Document
{
	float x;
	float z;
	float m_x_r;
	float m_y_r;
	float m_z_r;
	float m_x_a;
	float m_y_a;
	float m_z_a;
	float max_z;
	float min_z;
	
	float ext_z;
	float ext_x;
	
	float m_time;
	float m_max_time;
	
	GraphicObject* m_object;
	
	std::string m_mesh;
	std::string m_animation;
	
	bool m_anim;
};


#endif

#ifndef DOCUMENT_H
#define DOCUMENT_H


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
	
	
	AnimationState *mAnimationState;
	std::string m_mesh;
	std::string m_animation;
	float m_scaling_factor;
	
	bool m_anim;
	
	double speed_factor;
};


#endif

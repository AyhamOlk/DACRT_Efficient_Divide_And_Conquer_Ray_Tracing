#pragma once

#include <vector>

#include "Camera.h"
#include "Mesh.h"
#include "LightSource.h"

class Scene {
public:
	inline Scene () {}
	virtual ~Scene() {}

	inline const Camera& camera() const { return m_camera; }

	inline Camera& camera() { return m_camera; }

    inline const LightSource& light_source() { return m_light_source; }

	inline const std::vector<Mesh> & meshes () const { return m_meshes;  }
	
	inline std::vector<Mesh> & meshes () { return m_meshes; }

//private:
	Camera m_camera;
	std::vector<Mesh> m_meshes;
	LightSource m_light_source;
};
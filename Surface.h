#pragma once
#include "PVRShell/PVRShell.h"
#include "PVRUtils/PVRUtilsGles.h"

class Surface
{
private:
	// VBO handle
	uint32_t m_vbo;

	// Texture handle
	uint32_t m_texture;

	// Stride size of a vertex (position + texture coordinate)
	unsigned int m_vertexStride;

	// Position
	float m_x, m_y, m_z;

public:
	Surface(void);
	~Surface(void);

	bool Init(pvr::Shell *shell, pvr::EglContext &context);
	void Render(glm::mat4 mVP);
    void Compute();
	void SetPosition(float x, float y, float z);
};

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
    unsigned char* m_texMem;
    unsigned char* m_texResult;

	// Stride size of a vertex (position + texture coordinate)
	unsigned int m_vertexStride;

    static constexpr auto SIZE_X = 64;
    static constexpr auto SIZE_Y = 64;

public:
	Surface(void);
	~Surface(void);

	bool Init(pvr::Shell *shell, pvr::EglContext &context);
	void Render(glm::mat4 mVP);
    void Compute();
};

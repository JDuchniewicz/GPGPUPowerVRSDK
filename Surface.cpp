#include "Surface.h"

/******************************************************************************
 shader attributes and uniforms
******************************************************************************/
// Vertex attributes
// We define an enum for the attribute position and an array of strings that
// correspond to the attribute names in the shader. These can be used by the PVR Utils.

enum EVertexAttrib {
	VERTEX_ARRAY,
	eNumAttribs
};
const char* SurfaceAttribNames[] = {
	"inVertex", "inTexCoord"
};

// Shader uniforms
enum EUniform {
	eMVPMatrix,
	eNumUniforms
};
const char* SurfaceUniformNames[] = {
	"MVPMatrix"
};


// Group shader programs and their uniform locations together
struct
{
	GLuint uiId;
	GLuint auiLoc[eNumUniforms];
}
_triangleShaderProgram;


/******************************************************************************
 Content file names
******************************************************************************/

// Source shaders
const char FragmentShaderFile[]	= "FragShader.fsh";
const char VertexShaderFile[]	= "VertShader.vsh";

/******************************************************************************/


Surface::Surface(void) : m_vbo(0), m_vertexStride(0), m_texture(0), m_texMem(0), m_texResult(0)
{
}

Surface::~Surface(void)
{
	// Release Vertex buffer object.
	if(m_vbo) gl::DeleteBuffers(1, &m_vbo);
	// Frees the texture
	if(m_texture) gl::DeleteTextures(1, &m_texture);
    if(m_texMem) delete[] m_texMem;
    if(m_texResult) delete[] m_texResult;

	// Frees the OpenGL handles for the shader program
	gl::DeleteProgram(_triangleShaderProgram.uiId);
}

bool Surface::Init(pvr::Shell* shell, pvr::EglContext &context)
{
	// Interleaved vertex data
	GLfloat afVertices[] = {
						    -1.0f, -1.0f, 0.0f,
                             1.0f,-1.0f, 0.0f,
                             1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f,
                             1.0f, 1.0f, 0.0f,
                            -1.0f,-1.0f, 0.0f
							 };

	// Create VBO for the triangle from our data
	// Gen VBO
	gl::GenBuffers(1, &m_vbo);

	// Bind the VBO
	gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Calculate size of stride (position + texture coordinate = 3)
	m_vertexStride = 3 * sizeof(GLfloat); // 3 floats for the pos

	// Set the buffer's data
	gl::BufferData(GL_ARRAY_BUFFER, 6 * m_vertexStride, afVertices, GL_STATIC_DRAW);

	// Unbind the VBO
	gl::BindBuffer(GL_ARRAY_BUFFER, 0);

    // Create textures
    // allocate memory for the in texture and for result texture (after computation)
    m_texMem = new unsigned char[4 * Surface::SIZE_X * Surface::SIZE_Y];
    m_texResult = new unsigned char[4 * Surface::SIZE_X * Surface::SIZE_Y];
    //TODO: mark it for now
	for (int x = 0; x < 4*Surface::SIZE_X*Surface::SIZE_Y; x+=4) {
		m_texMem[x+0] = 0xde;
		m_texMem[x+1] = 0xad;
		m_texMem[x+2] = 0xbe;
		m_texMem[x+3] = 0xef;
	}
    gl::GenTextures(1, &m_texture);

	gl::BindTexture(GL_TEXTURE_2D, m_texture);
	gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Surface::SIZE_X, Surface::SIZE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) m_texMem);

	// We use PVR Utils to create the shader program for us
	static const char* attribs[] = { "inVertex" };
	static const uint16_t attribIndices[] = { 0 };
	_triangleShaderProgram.uiId = pvr::utils::createShaderProgram(*shell, VertexShaderFile, FragmentShaderFile, attribs, attribIndices, 1, 0, 0);

	// Store the location of uniforms for later use
	for (int i = 0; i < eNumUniforms; ++i)
	{
		_triangleShaderProgram.auiLoc[i] = gl::GetUniformLocation(_triangleShaderProgram.uiId, SurfaceUniformNames[i]);
	}

    // set up the framebuffer to which we will be drawing instead of the window
    GLuint framebuffer;
    GLuint textureColorBuffer;
    gl::GenFramebuffers(1, &framebuffer);

    // generate the texture to which we will be drawing
    gl::GenTextures(1, &textureColorBuffer);
    gl::BindTexture(GL_TEXTURE_2D, textureColorBuffer);
    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Surface::SIZE_X, Surface::SIZE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // bind the texture to this framebuffer
    gl::BindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    gl::Viewport(0, 0, Surface::SIZE_X, Surface::SIZE_Y);

    std::cout << "glCheckFramebufferStatus = " << gl::CheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	return true;
}

void Surface::Render(glm::mat4 mVP)
{
	// Use the loaded shader program
	gl::UseProgram(_triangleShaderProgram.uiId);

	// Pass the View Matrix to the shader.
	// Since we are not translating the triangle we do not need a Model Matrix.
	gl::UniformMatrix4fv(_triangleShaderProgram.auiLoc[eMVPMatrix], 1, GL_FALSE, glm::value_ptr(mVP));

    // Activate the texture unit 1 - one for calculations (all the setup can be run probably just once)
    gl::ActiveTexture(GL_TEXTURE1);

	// Bind the texture used for calculations
	gl::BindTexture(GL_TEXTURE_2D, m_texture);

	// Bind the VBO
	gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

	/*
		Enable the custom vertex attribute at index VERTEX_ARRAY.
		We previously binded that index to the variable in our shader "vec4 myVertex;"
	*/
	gl::EnableVertexAttribArray(VERTEX_ARRAY);
	// Points to the position data
	gl::VertexAttribPointer(VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, m_vertexStride, 0);

	gl::DrawArrays(GL_TRIANGLES, 0, 6);

	// Unbind the VBO
	gl::BindBuffer(GL_ARRAY_BUFFER, 0);

    // read back the pixels after rendering TODO: should happen before swapping?
    gl::ReadPixels(0, 0, Surface::SIZE_X, Surface::SIZE_Y, GL_RGBA, GL_UNSIGNED_BYTE, m_texResult);
	for (int i = 0; i < 4 * Surface::SIZE_X * Surface::SIZE_Y; i++) {
		if ((i % 16) == 0) putchar('\n');
		printf("%02x ", m_texResult[i]);
	}

}

void Surface::Compute()
{
    // this function will compute array addition (for now of two arrays in the shader)

}

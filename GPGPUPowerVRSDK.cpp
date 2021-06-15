#include "PVRShell/PVRShell.h"
#include "Surface.h"

class GPGPUPowerVRSDK : public pvr::Shell
{
	pvr::EglContext m_context;

    Surface m_surface;

public:
	virtual pvr::Result initApplication();
	virtual pvr::Result initView();
	virtual pvr::Result releaseView();
	virtual pvr::Result quitApplication();
	virtual pvr::Result renderFrame();
};

pvr::Result GPGPUPowerVRSDK::initApplication()
{
	m_surface.SetPosition(0, 0, -1);

	return pvr::Result::Success;
}

pvr::Result GPGPUPowerVRSDK::quitApplication()
{
	return pvr::Result::Success;
}

pvr::Result GPGPUPowerVRSDK::initView()
{
	// Create a new OpenGL ES graphics m_context
	m_context = pvr::createEglContext();
	m_context->init(getWindow(), getDisplay(), getDisplayAttributes());

	// Sets the clear color
	gl::ClearColor(0.6f, 0.8f, 1.0f, 1.0f);

	if (!m_surface.Init(this, m_context))
	{
		throw pvr::InvalidDataError(" ERROR: Triangle failed in Init()");
		return pvr::Result::UnknownError;
	}


	// Enable culling
	gl::Enable(GL_CULL_FACE);
	return pvr::Result::Success;
}

pvr::Result GPGPUPowerVRSDK::releaseView()
{
	return pvr::Result::Success;
}

pvr::Result GPGPUPowerVRSDK::renderFrame()
{
	// Clears the color and depth buffer
	gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
		Creates the Projection matrix using the PVR math tool.
		The tool returns the matrix using glm which is a cross-platform library
		used for handling Matrices and Vector mathematics
	*/
	glm::mat4 projection = pvr::math::perspective(pvr::Api::OpenGLES2, 45, static_cast<float>(this->getWidth()) / static_cast<float>(this->getHeight()), 0.1, 100, 0);

	/*
		Draw a triangle - we pass the Projection matrix to the triangle
	*/
	m_surface.Render(projection);

	m_context->swapBuffers();

	return pvr::Result::Success;
}

/// <summary>This function must be implemented by the user of the shell. The user should return its pvr::Shell object defining the behaviour of the application.</summary>
/// <returns>Return a unique ptr to the demo supplied by the user.</returns>
std::unique_ptr<pvr::Shell> pvr::newDemo()
{
    return std::unique_ptr<pvr::Shell>(new GPGPUPowerVRSDK());
}

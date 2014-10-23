#include <stdexcept>
#include "palleon/GlEs/GlEsRenderTarget.h"
#include "palleon/Log.h"

using namespace Palleon;

CGlEsRenderTarget::CGlEsRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
: m_width(width)
, m_height(height)
{
	glGenTextures(1, &m_texture);
	
	//TODO: Move sampling stuff out of here
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0 ,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECKGLERROR();
	
	glGenRenderbuffers(1, &m_depthRenderBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	CHECKGLERROR();

	glGenFramebuffers(1, &m_frameBuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);
	CHECKGLERROR();
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch(status)
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				CLog::GetInstance().Print("Failed to build framebuffer: Incomplete Attachment.\r\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				CLog::GetInstance().Print("Failed to build framebuffer: Incomplete Dimensions.\r\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				CLog::GetInstance().Print("Failed to build framebuffer: Missing Attachment.\r\n");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				CLog::GetInstance().Print("Failed to build framebuffer: Unsupported.\r\n");
				break;
			default:
				CLog::GetInstance().Print("Failed to build framebuffer: Unknown Status(0x%x).\r\n", status);
				break;
		}
		assert(0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CGlEsRenderTarget::~CGlEsRenderTarget()
{

}

void CGlEsRenderTarget::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGlEsRenderTarget::Draw(const ViewportPtr& viewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	auto& graphicDevice = static_cast<CGlEsGraphicDevice&>(CGraphicDevice::GetInstance());
	graphicDevice.DrawViewportMainMap(viewport.get(), m_width, m_height);
}

void CGlEsRenderTarget::Update(uint32, const void*)
{
	throw std::runtime_error("Not supported.");
}

void CGlEsRenderTarget::UpdateCubeFace(TEXTURE_CUBE_FACE, const void*)
{
	throw std::runtime_error("Not supported.");
}

void* CGlEsRenderTarget::GetHandle() const
{
	return reinterpret_cast<void*>(m_texture);
}

#include "athena/Emitter.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

static const float s_texCoords[4 * 2] =
{
	0, 0,
	1, 0,
	1, 1,
	0, 1
};

#define MAX_PARTICLE_COUNT 1000

CEmitter::CEmitter()
{
	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(MAX_PARTICLE_COUNT * 4, MAX_PARTICLE_COUNT * 6, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0 | VERTEX_BUFFER_HAS_COLOR);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = 0;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	m_particles.resize(MAX_PARTICLE_COUNT);
	memset(&m_particles[0], 0, sizeof(PARTICLE) * MAX_PARTICLE_COUNT);

	m_material->SetCullingMode(Athena::CULLING_NONE);
	m_material->SetAlphaBlendingMode(Athena::ALPHA_BLENDING_ADD);
}

CEmitter::~CEmitter()
{

}

void CEmitter::Emit(unsigned int emitCount)
{
	for(unsigned int i = 0; i < MAX_PARTICLE_COUNT; i++)
	{
		if(emitCount == 0) break;
		auto& particle(m_particles[i]);
		if(particle.life != 0) continue;
//		float angle = ((M_PI / 6) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - (M_PI / 12);
		float angle = 2 * M_PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		particle.position = CVector2(0, 0);
		particle.velocity = CVector2(sin(angle), cos(angle)) * 300.f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		particle.size = CVector2(30, 30);
		particle.maxLife = 0.01f + 0.25f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		particle.life = particle.maxLife;
//		particle.color.r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//		particle.color.g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//		particle.color.b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		particle.color.r = 1;
		particle.color.g = 1;
		particle.color.b = 1;
		particle.color.a = 1;
		emitCount--;
	}
}

void CEmitter::Update(float dt)
{
	UpdateParticles(dt);
	UpdateVertexBuffer();
}

void CEmitter::UpdateParticles(float dt)
{
	//Update all particles
	for(unsigned int i = 0; i < MAX_PARTICLE_COUNT; i++)
	{
		auto& particle(m_particles[i]);
		if(particle.life != 0)
		{
			particle.life = std::max<float>(particle.life - dt, 0);
			particle.color.a = particle.life / particle.maxLife;
			particle.position += particle.velocity * dt;
		}
	}
}

void CEmitter::UpdateVertexBuffer()
{
	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc(m_vertexBuffer->GetDescriptor());

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	uint16* indices = m_vertexBuffer->LockIndices();

	unsigned int particleCount = 0;
	for(unsigned int i = 0; i < MAX_PARTICLE_COUNT; i++)
	{
		const auto& particle(m_particles[i]);
		if(particle.life == 0)
		{
			continue;
		}

		CVector3 positions[4] =
		{
			CVector3(particle.position.x - particle.size.x / 2,	particle.position.y - particle.size.y / 2, 0),
			CVector3(particle.position.x + particle.size.x / 2, particle.position.y - particle.size.y / 2, 0),
			CVector3(particle.position.x + particle.size.x / 2, particle.position.y + particle.size.y / 2, 0),
			CVector3(particle.position.x - particle.size.x / 2,	particle.position.y + particle.size.y / 2, 0),
		};

		unsigned int indexBase = (particleCount * 4);

		uint32 color = CGraphicDevice::ConvertColorToUInt32(particle.color);

		for(unsigned int i = 0; i < 4; i++)
		{
			*reinterpret_cast<CVector3*>(vertices + bufferDesc.posOffset) = positions[i];
			*reinterpret_cast<CVector2*>(vertices + bufferDesc.uv0Offset) = CVector2(&s_texCoords[i * 2]);
			*reinterpret_cast<uint32*>(vertices + bufferDesc.colorOffset) = color;
			vertices += bufferDesc.GetVertexSize();
		}

		{
			indices[0] = indexBase + 0;
			indices[1] = indexBase + 2;
			indices[2] = indexBase + 1;
			indices[3] = indexBase + 0;
			indices[4] = indexBase + 3;
			indices[5] = indexBase + 2;
		}
		indices += 6;

		particleCount++;
	}

	m_vertexBuffer->UnlockVertices(particleCount * 4);
	m_vertexBuffer->UnlockIndices();

	m_primitiveCount = particleCount * 2;
}

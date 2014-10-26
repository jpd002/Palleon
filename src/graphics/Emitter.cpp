#include "palleon/graphics/Emitter.h"
#include "palleon/graphics/EmitterModifier.h"
#include "palleon/graphics/GraphicDevice.h"

using namespace Palleon;

static const float s_texCoords[4 * 2] =
{
	0, 0,
	1, 0,
	1, 1,
	0, 1
};

#define MAX_PARTICLE_COUNT 4000

template <typename ValueType>
ValueType RangeRandom(const ValueType& minVal, const ValueType& maxVal)
{
	float alpha = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return minVal + (maxVal - minVal) * alpha;
}

template <typename ValueType>
ValueType RangeRandom(const std::pair<ValueType, ValueType>& range)
{
	return RangeRandom(range.first, range.second);
}

CEmitter::CEmitter()
: m_descriptor(nullptr)
{
	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(MAX_PARTICLE_COUNT * 4, MAX_PARTICLE_COUNT * 6, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0 | VERTEX_BUFFER_HAS_COLOR);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = 0;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	m_particles.resize(MAX_PARTICLE_COUNT);
	memset(&m_particles[0], 0, sizeof(PARTICLE) * MAX_PARTICLE_COUNT);

	m_material->SetCullingMode(Palleon::CULLING_NONE);
	m_material->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_LERP);
}

CEmitter::~CEmitter()
{

}

void CEmitter::SetDescriptor(const CEmitterDescriptor* descriptor)
{
	m_descriptor = descriptor;
}

void CEmitter::AddModifier(const EmitterModifierPtr& emitterModifier)
{
	m_modifiers.push_back(emitterModifier);
}

void CEmitter::Emit(unsigned int emitCount)
{
	assert(m_descriptor != nullptr);
	for(unsigned int i = 0; i < MAX_PARTICLE_COUNT; i++)
	{
		if(emitCount == 0) break;
		auto& particle(m_particles[i]);
		if(particle.life != 0) continue;
		float speed = RangeRandom(m_descriptor->GetInitialVelocityRange());
		switch(m_descriptor->GetSourceType())
		{
		case EMITTER_SOURCE_POINT:
			{
				float angle = RangeRandom(0.0f, 2.0f * static_cast<float>(M_PI));
				particle.position = CVector2(0, 0);
				particle.velocity = CVector2(sin(angle), cos(angle)) * speed;
			}
			break;
		case EMITTER_SOURCE_LINE:
			{
				particle.position = RangeRandom(m_descriptor->GetLineSourceStart(), m_descriptor->GetLineSourceEnd());
				particle.velocity = CVector2(0, 1) * speed;
			}
			break;
		}
		particle.size = RangeRandom(m_descriptor->GetInitialSizeRange());
		particle.maxLife = RangeRandom(m_descriptor->GetInitialLifeRange());
		particle.life = particle.maxLife;
		particle.color = CColor(1, 1, 1, 1);
		emitCount--;
	}
}

void CEmitter::Update(float dt)
{
	for(auto& modifier : m_modifiers)
	{
		modifier->Update(dt);
	}
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
			for(const auto& modifier : m_modifiers)
			{
				modifier->Modify(dt, particle);
			}
		}
	}
}

void CEmitter::UpdateVertexBuffer()
{
	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc(m_vertexBuffer->GetDescriptor());
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_UV0);
	const auto& colorVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_COLOR);

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
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = positions[i];
			*reinterpret_cast<CVector2*>(vertices + uv0VertexItem->offset) = CVector2(&s_texCoords[i * 2]);
			*reinterpret_cast<uint32*>(vertices + colorVertexItem->offset) = color;
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

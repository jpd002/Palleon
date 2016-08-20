#pragma once

#include "vulkan/ShaderModule.h"
#include "palleon/graphics/Effect.h"
#include "palleon/graphics/Mesh.h"

namespace Palleon
{
	struct VULKAN_PIPELINE_KEY
	{
		bool operator ==(const VULKAN_PIPELINE_KEY& rhs) const
		{
			return
				(primitiveType == rhs.primitiveType) &&
				(vertexItems == rhs.vertexItems);
		}
		
		VkRenderPass     renderPass = VK_NULL_HANDLE;
		PRIMITIVE_TYPE   primitiveType = PRIMITIVE_INVALID;
		VERTEX_ITEMS_KEY vertexItems = VertexItemArray();
	};
}

namespace std
{
	template <>
	struct hash<Palleon::VULKAN_PIPELINE_KEY>
	{
		size_t operator ()(const Palleon::VULKAN_PIPELINE_KEY& key) const
		{
			using namespace Palleon;
			
			uLong crc = crc32(0L, Z_NULL, 0);
			crc = crc32(crc, reinterpret_cast<const Bytef*>(&key.renderPass), sizeof(VkRenderPass));
			crc = crc32(crc, reinterpret_cast<const Bytef*>(&key.primitiveType), sizeof(PRIMITIVE_TYPE));
			crc = crc32(
				crc, 
				reinterpret_cast<const Bytef*>(key.vertexItems.vertexItems.data()), 
				sizeof(Palleon::VERTEX_ITEM) * key.vertexItems.vertexItems.size());
			return crc;
		}
	};
}

namespace Palleon
{
	class CMesh;
	
	class CVulkanEffect : public CEffect
	{
	public:
		CVulkanEffect(Framework::Vulkan::CDevice&);
		~CVulkanEffect();
		
		void UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		
		void          PrepareDraw(VkCommandBuffer);
		VkPipeline    GetPipelineForMesh(CMesh*, VkRenderPass);
		
	protected:
		struct DefaultPushConstants
		{
			CMatrix4 viewProjMatrix;
			CMatrix4 worldMatrix;
		};
		
		typedef std::unordered_map<VULKAN_PIPELINE_KEY, VkPipeline> PipelineMap;
	
		void    CreateDefaultPipelineLayout();
		
		Framework::Vulkan::CDevice*         m_device = nullptr;
		
		VkPipelineLayout                    m_defaultPipelineLayout = VK_NULL_HANDLE;
		PipelineMap                         m_pipelines;
		
		DefaultPushConstants                m_pushConstants;
		
		Framework::Vulkan::CShaderModule    m_vertexShaderModule;
		Framework::Vulkan::CShaderModule    m_fragmentShaderModule;
	};
}

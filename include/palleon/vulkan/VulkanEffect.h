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
	
	class CVulkanEffect;
	typedef std::shared_ptr<CVulkanEffect> VulkanEffectPtr;
	
	class CVulkanEffect : public CEffect
	{
	public:
		CVulkanEffect(Framework::Vulkan::CDevice&);
		~CVulkanEffect();
		
		virtual void  PrepareDraw(VkCommandBuffer) = 0;
		
		VkPipeline    GetPipelineForMesh(CMesh*, VkRenderPass);
		
		VkPipelineLayout         GetPipelineLayout() const;
		VkDescriptorSetLayout    GetDescriptorSetLayout() const;
		
	protected:
		typedef std::unordered_map<VULKAN_PIPELINE_KEY, VkPipeline> PipelineMap;
		
		Framework::Vulkan::CDevice*         m_device = nullptr;
		
		PipelineMap                         m_pipelines;
		
		//These need to be filled by classes that inherit this one
		//This class will be responsible for cleaning up these
		Framework::Vulkan::CShaderModule    m_vertexShaderModule;
		Framework::Vulkan::CShaderModule    m_fragmentShaderModule;
		VkPipelineLayout                    m_pipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout               m_descriptorSetLayout = VK_NULL_HANDLE;
	};
}

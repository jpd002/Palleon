cmake_minimum_required(VERSION 2.8)
project(Palleon)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include ../../Framework/include)

add_library(Palleon
	../src/Box2.cpp
	../src/ButtonBase.cpp
	../src/Frustum.cpp
	../src/graphics/Camera.cpp
	../src/graphics/CubeMesh.cpp
	../src/graphics/EffectParameter.cpp
	../src/graphics/EffectParameterBag.cpp
	../src/graphics/Emitter.cpp
	../src/graphics/GenericEffect.cpp
	../src/graphics/GraphicDevice.cpp
	../src/graphics/Label.cpp
	../src/graphics/Material.cpp
	../src/graphics/Mesh.cpp
	../src/graphics/NinePatch.cpp
	../src/graphics/RenderTarget.cpp
	../src/graphics/Scene.cpp
	../src/graphics/SceneNode.cpp
	../src/graphics/ShaderBuilder.cpp
	../src/graphics/SphereMesh.cpp
	../src/graphics/Sprite.cpp
	../src/graphics/Texture.cpp
	../src/graphics/TextureLoader.cpp
	../src/graphics/VertexBuffer.cpp
	../src/graphics/Viewport.cpp
	../src/InputManager.cpp
	../src/LayoutNode.cpp
	../src/Log.cpp
	../src/NinePatchButton.cpp
	../src/ParseUtils.cpp
	../src/resources/DdsImage.cpp
	../src/resources/EmitterDescriptor.cpp
	../src/resources/FontDescriptor.cpp
	../src/resources/NinePatchDescriptor.cpp
	../src/resources/SceneDescriptor.cpp
	../src/resources/Package.cpp
	../src/resources/ResourceManager.cpp
	../src/resources/TextureConverters.cpp
	../src/resources/TextureResource.cpp
	../src/RumbleAnimation.cpp
	../src/SpriteButton.cpp
	../src/TextureAnimation.cpp
	../src/unix/UnixApplicationWindow.cpp
	../src/unix/UnixLog.cpp
	../src/unix/UnixMain.cpp
	../src/unix/UnixResourceManager.cpp
	../src/vulkan/VulkanGraphicDevice.cpp
	../src/vulkan/VulkanTexture.cpp
	../src/vulkan/VulkanVertexBuffer.cpp
	../src/Widget.cpp
)


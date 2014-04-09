cmake_minimum_required(VERSION 2.6)
project(Meleon)

set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wno-warn-absolute-paths")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

set (FRAMEWORK_ROOT ../../Framework/)

include_directories(
	../include 
	/usr/local/include 
	/usr/local/include/c++/v1 
	/usr/local/include/mysql 
	/Users/jpd002/Libraries/boost_1_55_0
	${FRAMEWORK_ROOT}/include
)

link_directories(
	../${FRAMEWORK_ROOT}/build_html5/build/
)

add_library(Meleon
	../src/Box2.cpp
	../src/Camera.cpp
	../src/CubeMesh.cpp
	../src/gles/GlEsEffect.cpp
	../src/gles/GlEsGraphicDevice.cpp
	../src/gles/GlEsShadowMapEffect.cpp
	../src/gles/GlEsTexture.cpp
	../src/gles/GlEsUberEffect.cpp
	../src/gles/GlEsUberEffectGenerator.cpp
	../src/gles/GlEsUberEffectProvider.cpp
	../src/gles/GlEsVertexBuffer.cpp
	../src/GraphicDevice.cpp
	../src/html5/Html5Canvas.cpp
	../src/html5/Html5GraphicDevice.cpp
	../src/InputManager.cpp
	../src/Material.cpp
	../src/Mesh.cpp
	../src/resources/Package.cpp
	../src/resources/ResourceManager.cpp
	../src/SceneNode.cpp
	../src/SphereMesh.cpp
	../src/Texture.cpp
	../src/VertexBuffer.cpp
	../src/Viewport.cpp
	../src/Widget.cpp
)

add_executable(ShadowMapping.html
	../samples/ShadowMapping/src/Application.cpp
	../samples/ShadowMapping/src/TouchFreeCamera.cpp
)

target_link_libraries(ShadowMapping.html Framework Meleon)


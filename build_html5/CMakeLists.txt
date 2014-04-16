cmake_minimum_required(VERSION 2.6)
project(Palleon)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wno-warn-absolute-paths")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

set(BOOST_ROOT /Users/jpd002/Libraries/boost_1_55_0)
set(ZLIB_ROOT /Users/jpd002/Libraries/zlib-1.2.8)
set(ZLIB_LIB_ROOT ../../zlib-1.2.8/)
set(FRAMEWORK_ROOT ../../Framework/)

include_directories(
	../include 
	${BOOST_ROOT}
	${ZLIB_ROOT}
	${FRAMEWORK_ROOT}/include
)

link_directories(
	../${FRAMEWORK_ROOT}/build_html5/build/
	../${ZLIB_LIB_ROOT}/build_html5/build/
)

add_library(Palleon
	../src/AxisMesh.cpp
	../src/Box2.cpp
	../src/ButtonBase.cpp
	../src/Camera.cpp
	../src/ConeMesh.cpp
	../src/CubeMesh.cpp
	../src/EffectParameter.cpp
	../src/Frustum.cpp
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
	../src/html5/Html5ResourceManager.cpp
	../src/InputManager.cpp
	../src/Label.cpp
	../src/Material.cpp
	../src/Mesh.cpp
	../src/NinePatch.cpp
	../src/NinePatchButton.cpp
	../src/ParseUtils.cpp
	../src/resources/DdsImage.cpp
	../src/resources/EmitterDescriptor.cpp
	../src/resources/FontDescriptor.cpp
	../src/resources/NinePatchDescriptor.cpp
	../src/resources/Package.cpp
	../src/resources/ResourceManager.cpp
	../src/resources/SceneDescriptor.cpp
	../src/resources/TextureResource.cpp
	../src/Scene.cpp
	../src/SceneNode.cpp
	../src/SphereMesh.cpp
	../src/Sprite.cpp
	../src/SpriteButton.cpp
	../src/Texture.cpp
	../src/TextureLoader.cpp
	../src/VertexBuffer.cpp
	../src/Viewport.cpp
	../src/Widget.cpp
)

add_executable(ShadowMapping.html
	../samples/ShadowMapping/src/Application.cpp
	../samples/ShadowMapping/src/TouchFreeCamera.cpp
)

add_executable(QuakeViewer.html
	../samples/QuakeViewer/src/BatchMesh.cpp
	../samples/QuakeViewer/src/BspFaceMesh.cpp
	../samples/QuakeViewer/src/BspFile.cpp
	../samples/QuakeViewer/src/BspMapMaterial.cpp
	../samples/QuakeViewer/src/BspMapMeshProvider.cpp
	../samples/QuakeViewer/src/BspMapPass.cpp
	../samples/QuakeViewer/src/BspMapResourceProvider.cpp
	../samples/QuakeViewer/src/BspMapTcMod.cpp
	../samples/QuakeViewer/src/ContextBase.cpp
	../samples/QuakeViewer/src/LevelSelectionContext.cpp
	../samples/QuakeViewer/src/LevelViewContext.cpp
	../samples/QuakeViewer/src/PakFile.cpp
	../samples/QuakeViewer/src/QuakeEntity.cpp
	../samples/QuakeViewer/src/QuakeEntityParser.cpp
	../samples/QuakeViewer/src/QuakeShaderParser.cpp
	../samples/QuakeViewer/src/QuakeShaderTokenizer.yy.cpp
	../samples/QuakeViewer/src/QuakeViewer.cpp
)

set_target_properties(ShadowMapping.html PROPERTIES LINK_FLAGS "--preload-file ../../samples/ShadowMapping/data@/")
target_link_libraries(ShadowMapping.html Palleon Framework z)

set_target_properties(QuakeViewer.html PROPERTIES LINK_FLAGS "--preload-file ../../samples/QuakeViewer/data@/ -s TOTAL_MEMORY=134217728")
target_link_libraries(QuakeViewer.html Palleon Framework z)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

BOOST_PATH			:= /cygdrive/c/Components/boost_trunk/
FRAMEWORK_PATH		:= /cygdrive/c/Projects/Framework/

LOCAL_MODULE		:= libPalleon
LOCAL_SRC_FILES		:=	../../src/android/AndroidActivity.cpp \
						../../src/android/AndroidGraphicDevice.cpp \
						../../src/android/AndroidLog.cpp \
						../../src/android/AndroidResourceManager.cpp \
						../../src/Box2.cpp \
						../../src/ButtonBase.cpp \
						../../src/Camera.cpp \
						../../src/CubeMesh.cpp \
						../../src/gles/GlEsEffect.cpp \
						../../src/gles/GlEsGraphicDevice.cpp \
						../../src/gles/GlEsShadowMapEffect.cpp \
						../../src/gles/GlEsTexture.cpp \
						../../src/gles/GlEsUberEffect.cpp \
						../../src/gles/GlEsUberEffectGenerator.cpp \
						../../src/gles/GlEsUberEffectProvider.cpp \
						../../src/gles/GlEsVertexBuffer.cpp \
						../../src/GraphicDevice.cpp \
						../../src/InputManager.cpp \
						../../src/Label.cpp \
						../../src/LayoutNode.cpp \
						../../src/Log.cpp \
						../../src/Material.cpp \
						../../src/Mesh.cpp \
						../../src/NinePatch.cpp \
						../../src/NinePatchButton.cpp \
						../../src/ParseUtils.cpp \
						../../src/resources/DdsImage.cpp \
						../../src/resources/EmitterDescriptor.cpp \
						../../src/resources/FontDescriptor.cpp \
						../../src/resources/NinePatchDescriptor.cpp \
						../../src/resources/SceneDescriptor.cpp \
						../../src/resources/Package.cpp \
						../../src/resources/ResourceManager.cpp \
						../../src/resources/TextureConverters.cpp \
						../../src/resources/TextureResource.cpp \
						../../src/Scene.cpp \
						../../src/SceneNode.cpp \
						../../src/SphereMesh.cpp \
						../../src/Sprite.cpp \
						../../src/SpriteButton.cpp \
						../../src/Texture.cpp \
						../../src/TextureLoader.cpp \
						../../src/VertexBuffer.cpp \
						../../src/Viewport.cpp \
						../../src/Widget.cpp
LOCAL_C_INCLUDES	:= $(BOOST_PATH) $(FRAMEWORK_PATH)/include $(LOCAL_PATH)/../../include
LOCAL_CPP_FEATURES	:= exceptions rtti

include $(BUILD_STATIC_LIBRARY)

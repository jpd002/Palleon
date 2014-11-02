LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

BOOST_PATH			:= /cygdrive/c/Components/boost_trunk/
FRAMEWORK_PATH		:= /cygdrive/c/Projects/Framework/

LOCAL_MODULE		:= libPalleon
LOCAL_SRC_FILES		:=	../../src/android/AndroidActivity.cpp \
						../../src/android/AndroidAssetStream.cpp \
						../../src/android/AndroidGraphicDevice.cpp \
						../../src/android/AndroidLog.cpp \
						../../src/android/AndroidResourceManager.cpp \
						../../src/Box2.cpp \
						../../src/ButtonBase.cpp \
						../../src/gles/GlEsEffect.cpp \
						../../src/gles/GlEsGenericEffect.cpp \
						../../src/gles/GlEsGraphicDevice.cpp \
						../../src/gles/GlEsShaderGenerator.cpp \
						../../src/gles/GlEsShadowMapEffect.cpp \
						../../src/gles/GlEsTexture.cpp \
						../../src/gles/GlEsRenderTarget.cpp \
						../../src/gles/GlEsUberEffect.cpp \
						../../src/gles/GlEsUberEffectGenerator.cpp \
						../../src/gles/GlEsUberEffectProvider.cpp \
						../../src/gles/GlEsVertexBuffer.cpp \
						../../src/graphics/Camera.cpp \
						../../src/graphics/CubeMesh.cpp \
						../../src/graphics/EffectParameter.cpp \
						../../src/graphics/EffectParameterBag.cpp \
						../../src/graphics/GenericEffect.cpp \
						../../src/graphics/GraphicDevice.cpp \
						../../src/graphics/Material.cpp \
						../../src/graphics/Mesh.cpp \
						../../src/graphics/NinePatch.cpp \
						../../src/graphics/RenderTarget.cpp \
						../../src/graphics/Scene.cpp \
						../../src/graphics/SceneNode.cpp \
						../../src/graphics/ShaderBuilder.cpp \
						../../src/graphics/SphereMesh.cpp \
						../../src/graphics/Sprite.cpp \
						../../src/graphics/Texture.cpp \
						../../src/graphics/TextureLoader.cpp \
						../../src/graphics/VertexBuffer.cpp \
						../../src/graphics/Viewport.cpp \
						../../src/InputManager.cpp \
						../../src/graphics/Label.cpp \
						../../src/LayoutNode.cpp \
						../../src/Log.cpp \
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
						../../src/SpriteButton.cpp \
						../../src/Widget.cpp
LOCAL_C_INCLUDES	:= $(BOOST_PATH) $(FRAMEWORK_PATH)/include $(LOCAL_PATH)/../../include
LOCAL_CPP_FEATURES	:= exceptions rtti

include $(BUILD_STATIC_LIBRARY)

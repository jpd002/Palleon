#pragma once

#include "palleon/PlatformDef.h"

//Maths
#include "palleon/MathDef.h"
#include "palleon/Vector2.h"
#include "palleon/Vector3.h"
#include "palleon/Vector4.h"
#include "palleon/Matrix4.h"
#include "palleon/Quaternion.h"
#include "palleon/Box2.h"
#include "palleon/Tween.h"
#include "palleon/MathOps.h"

//Resources
#include "palleon/resources/ResourceManager.h"
#include "palleon/resources/FontDescriptor.h"
#include "palleon/resources/NinePatchDescriptor.h"
#include "palleon/resources/EmitterDescriptor.h"
#include "palleon/resources/SceneDescriptor.h"
#include "palleon/resources/Package.h"
#include "palleon/resources/PvrImage.h"

//Graphics
#include "palleon/GraphicDevice.h"

#include "palleon/Mesh.h"
#include "palleon/MeshProvider.h"
#include "palleon/CubeMesh.h"
#include "palleon/SphereMesh.h"
#include "palleon/ConeMesh.h"
#include "palleon/AxisMesh.h"

#include "palleon/Label.h"
#include "palleon/Sprite.h"
#include "palleon/NinePatch.h"
#include "palleon/Emitter.h"
#include "palleon/EmitterModifier.h"

#include "palleon/TextureLoader.h"

#if defined(PALLEON_WIN32)
#include "palleon/win32/Dx11GenericEffect.h"
#endif
#if defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
#include "palleon/gles/GlEsGenericEffect.h"
#endif

//Animation
#include "palleon/IAnimation.h"
#include "palleon/AnimationController.h"
#include "palleon/AnimationCurve.h"
#include "palleon/TextureAnimation.h"
#include "palleon/RumbleAnimation.h"

//Widgets
#include "palleon/InputManager.h"
#include "palleon/SpriteButton.h"
#include "palleon/NinePatchButton.h"

//Audio
#include "palleon/AudioManager.h"

//Scene
#include "palleon/Scene.h"
#include "palleon/ParseUtils.h"

#include "palleon/EmbedRemoteCall.h"
#include "palleon/Application.h"
#include "palleon/Log.h"

#ifdef PALLEON_ANDROID
extern void palleon_library_link();
#else
static void palleon_library_link() {}
#endif

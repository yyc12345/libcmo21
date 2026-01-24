#pragma once

/**
 * \file
 * This header file is exposed to end user.
 * In end user program, user can only simply include this single header file
 * to include all necessity about Virtools.
 * This file is pretty like \c CKAll.h in original Virtools SDK.
 * 
 * This file should not be included in LibCmo project inside.
 * If you want to have initialized LibCmo development environment within this project,
 * please include VTInternal.hpp instead.
*/

// Include internal header
#include "LibCmo/VTInternal.hpp"

// CK2 Misc
#include "LibCmo/CK2/CKContext.hpp"
#include "LibCmo/CK2/CKStateChunk.hpp"
#include "LibCmo/CK2/CKFile.hpp"

// CK2 Data handlers
#include "LibCmo/CK2/DataHandlers/CKBitmapHandler.hpp"

// CK2 Objects
#include "LibCmo/CK2/ObjImpls/CKObject.hpp"
#include "LibCmo/CK2/ObjImpls/CKSceneObject.hpp"
#include "LibCmo/CK2/ObjImpls/CKBeObject.hpp"
#include "LibCmo/CK2/ObjImpls/CKGroup.hpp"
#include "LibCmo/CK2/ObjImpls/CKRenderObject.hpp"
#include "LibCmo/CK2/ObjImpls/CK3dEntity.hpp"
#include "LibCmo/CK2/ObjImpls/CK3dObject.hpp"
#include "LibCmo/CK2/ObjImpls/CKTexture.hpp"
#include "LibCmo/CK2/ObjImpls/CKMaterial.hpp"
#include "LibCmo/CK2/ObjImpls/CKMesh.hpp"
#include "LibCmo/CK2/ObjImpls/CKLight.hpp"
#include "LibCmo/CK2/ObjImpls/CKTargetLight.hpp"
#include "LibCmo/CK2/ObjImpls/CKCamera.hpp"
#include "LibCmo/CK2/ObjImpls/CKTargetCamera.hpp"

// CK2 Managers
#include "LibCmo/CK2/MgrImpls/CKBaseManager.hpp"
#include "LibCmo/CK2/MgrImpls/CKObjectManager.hpp"
#include "LibCmo/CK2/MgrImpls/CKPathManager.hpp"

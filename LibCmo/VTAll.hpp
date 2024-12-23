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
#include "VTInternal.hpp"

// CK2 Misc
#include "CK2/CKContext.hpp"
#include "CK2/CKStateChunk.hpp"
#include "CK2/CKFile.hpp"

// CK2 Data handlers
#include "CK2/DataHandlers/CKBitmapHandler.hpp"

// CK2 Objects
#include "CK2/ObjImpls/CKObject.hpp"
#include "CK2/ObjImpls/CKSceneObject.hpp"
#include "CK2/ObjImpls/CKBeObject.hpp"
#include "CK2/ObjImpls/CKGroup.hpp"
#include "CK2/ObjImpls/CKRenderObject.hpp"
#include "CK2/ObjImpls/CK3dEntity.hpp"
#include "CK2/ObjImpls/CK3dObject.hpp"
#include "CK2/ObjImpls/CKTexture.hpp"
#include "CK2/ObjImpls/CKMaterial.hpp"
#include "CK2/ObjImpls/CKMesh.hpp"
#include "CK2/ObjImpls/CKLight.hpp"
#include "CK2/ObjImpls/CKTargetLight.hpp"
#include "CK2/ObjImpls/CKCamera.hpp"
#include "CK2/ObjImpls/CKTargetCamera.hpp"

// CK2 Managers
#include "CK2/MgrImpls/CKBaseManager.hpp"
#include "CK2/MgrImpls/CKObjectManager.hpp"
#include "CK2/MgrImpls/CKPathManager.hpp"

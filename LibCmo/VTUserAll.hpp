#pragma once

/*
This file is provided for user of LibCmo.
This file should not be included in LibCmo project inside.
Use VTAll.hpp for project internal including.
*/

#include "VTAll.hpp"
#include "CK2/CKContext.hpp"
#include "CK2/CKStateChunk.hpp"
#include "CK2/CKFile.hpp"

// Data handlers
#include "CK2/DataHandlers/CKBitmapHandler.hpp"

// Objects
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

// Managers
#include "CK2/MgrImpls/CKBaseManager.hpp"
#include "CK2/MgrImpls/CKObjectManager.hpp"
#include "CK2/MgrImpls/CKPathManager.hpp"

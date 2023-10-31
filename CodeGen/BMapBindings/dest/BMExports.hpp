LIBCMO_EXPORT bool BMInit();
LIBCMO_EXPORT bool BMDispose();
LIBCMO_EXPORT bool BMFile_Load(
	BMPARAM_IN(LibCmo::CKSTRING, file_name),
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)
);
LIBCMO_EXPORT bool BMFile_Create(
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)
);
LIBCMO_EXPORT bool BMFile_Save(
	BMPARAM_IN(BMap::BMFile*, map_file),
	BMPARAM_IN(LibCmo::CKSTRING, file_name),
	BMPARAM_IN(LibCmo::CKINT, compreess_level)
);
LIBCMO_EXPORT bool BMFile_Free(
	BMPARAM_IN(BMap::BMFile*, map_file)
);
LIBCMO_EXPORT bool BMFile_GetGroupCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMFile_GetGroup(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_CreateGroup(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_Get3dObjectCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMFile_Get3dObject(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_Create3dObject(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_GetMeshCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMFile_GetMesh(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_CreateMesh(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_GetMaterialCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMFile_GetMaterial(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_CreateMaterial(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_GetTextureCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMFile_GetTexture(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMFile_CreateTexture(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id));
LIBCMO_EXPORT bool BMMeshTrans_New(BMPARAM_OUT(BMap::BMMeshTransition*, out_trans));
LIBCMO_EXPORT bool BMMeshTrans_Delete(BMPARAM_IN(BMap::BMMeshTransition*, trans));
LIBCMO_EXPORT bool BMMeshTrans_PrepareVertexCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMeshTrans_PrepareVertex(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareNormalCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMeshTrans_PrepareNormal(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareUVCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMeshTrans_PrepareUV(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector2*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareMtlSlotCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMeshTrans_PrepareMtlSlot(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CK2::CK_ID*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceVertexIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceNormalIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceUVIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceMtlSlot(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem));
LIBCMO_EXPORT bool BMMeshTrans_Parse(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(BMap::BMFile*, bmfile), BMPARAM_IN(LibCmo::CK2::CK_ID, objid));
LIBCMO_EXPORT bool BMObject_GetName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKSTRING, out_name));
LIBCMO_EXPORT bool BMObject_SetName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, name));
LIBCMO_EXPORT bool BMGroup_AddObject(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, memberid));
LIBCMO_EXPORT bool BMGroup_GetObjectCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMGroup_GetObject(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, pos), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_objid));
LIBCMO_EXPORT bool BMTexture_GetFileName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKSTRING, out_filename));
LIBCMO_EXPORT bool BMTexture_LoadImage(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, filename));
LIBCMO_EXPORT bool BMTexture_SaveImage(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, filename));
LIBCMO_EXPORT bool BMTexture_GetSaveOptions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, out_saveopt));
LIBCMO_EXPORT bool BMTexture_SetSaveOptions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, saveopt));
LIBCMO_EXPORT bool BMTexture_GetVideoFormat(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VX_PIXELFORMAT, out_vfmt));
LIBCMO_EXPORT bool BMTexture_SetVideoFormat(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VX_PIXELFORMAT, vfmt));
LIBCMO_EXPORT bool BMMaterial_GetDiffuse(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
LIBCMO_EXPORT bool BMMaterial_SetDiffuse(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
LIBCMO_EXPORT bool BMMaterial_GetAmbient(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
LIBCMO_EXPORT bool BMMaterial_SetAmbient(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
LIBCMO_EXPORT bool BMMaterial_GetSpecular(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
LIBCMO_EXPORT bool BMMaterial_SetSpecular(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
LIBCMO_EXPORT bool BMMaterial_GetEmissive(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val));
LIBCMO_EXPORT bool BMMaterial_SetEmissive(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col));
LIBCMO_EXPORT bool BMMaterial_GetSpecularPower(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val));
LIBCMO_EXPORT bool BMMaterial_SetSpecularPower(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val));
LIBCMO_EXPORT bool BMMaterial_GetTexture(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_texid));
LIBCMO_EXPORT bool BMMaterial_SetTexture(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, texid));
LIBCMO_EXPORT bool BMMaterial_GetTextureBorderColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_val));
LIBCMO_EXPORT bool BMMaterial_SetTextureBorderColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, val));
LIBCMO_EXPORT bool BMMaterial_GetTextureBlendMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_BLENDMODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetTextureBlendMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_BLENDMODE, val));
LIBCMO_EXPORT bool BMMaterial_GetTextureMinMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_FILTERMODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetTextureMinMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_FILTERMODE, val));
LIBCMO_EXPORT bool BMMaterial_GetTextureMagMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_FILTERMODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetTextureMagMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_FILTERMODE, val));
LIBCMO_EXPORT bool BMMaterial_GetTextureAddressMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_ADDRESSMODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetTextureAddressMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_ADDRESSMODE, val));
LIBCMO_EXPORT bool BMMaterial_GetSourceBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXBLEND_MODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetSourceBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXBLEND_MODE, val));
LIBCMO_EXPORT bool BMMaterial_GetDestBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXBLEND_MODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetDestBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXBLEND_MODE, val));
LIBCMO_EXPORT bool BMMaterial_GetFillMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXFILL_MODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetFillMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXFILL_MODE, val));
LIBCMO_EXPORT bool BMMaterial_GetShadeMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXSHADE_MODE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetShadeMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXSHADE_MODE, val));
LIBCMO_EXPORT bool BMMaterial_GetAlphaTestEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
LIBCMO_EXPORT bool BMMaterial_SetAlphaTestEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
LIBCMO_EXPORT bool BMMaterial_GetAlphaBlendEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
LIBCMO_EXPORT bool BMMaterial_SetAlphaBlendEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
LIBCMO_EXPORT bool BMMaterial_GetPerspectiveCorrectionEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
LIBCMO_EXPORT bool BMMaterial_SetPerspectiveCorrectionEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
LIBCMO_EXPORT bool BMMaterial_GetZWriteEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
LIBCMO_EXPORT bool BMMaterial_SetZWriteEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
LIBCMO_EXPORT bool BMMaterial_GetTwoSidedEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val));
LIBCMO_EXPORT bool BMMaterial_SetTwoSidedEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled));
LIBCMO_EXPORT bool BMMaterial_GetAlphaRef(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKBYTE, out_val));
LIBCMO_EXPORT bool BMMaterial_SetAlphaRef(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKBYTE, val));
LIBCMO_EXPORT bool BMMaterial_GetAlphaFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXCMPFUNC, out_val));
LIBCMO_EXPORT bool BMMaterial_SetAlphaFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXCMPFUNC, val));
LIBCMO_EXPORT bool BMMaterial_GetZFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXCMPFUNC, out_val));
LIBCMO_EXPORT bool BMMaterial_SetZFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXCMPFUNC, val));
LIBCMO_EXPORT bool BMMesh_GetVertexCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMMesh_SetVertexCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMesh_GetVertexPositions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
LIBCMO_EXPORT bool BMMesh_GetVertexNormals(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem));
LIBCMO_EXPORT bool BMMesh_GetVertexUVs(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector2*, out_mem));
LIBCMO_EXPORT bool BMMesh_GetFaceCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMMesh_SetFaceCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMesh_GetFaceIndices(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKWORD*, out_mem));
LIBCMO_EXPORT bool BMMesh_GetFaceMaterialSlotIndexs(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKWORD*, out_mem));
LIBCMO_EXPORT bool BMMesh_GetMaterialSlotCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count));
LIBCMO_EXPORT bool BMMesh_SetMaterialSlotCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count));
LIBCMO_EXPORT bool BMMesh_GetMaterialSlot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, index), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_mtlid));
LIBCMO_EXPORT bool BMMesh_SetMaterialSlot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, index), BMPARAM_IN(LibCmo::CK2::CK_ID, mtlid));
LIBCMO_EXPORT bool BM3dEntity_GetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxMatrix, out_mat));
LIBCMO_EXPORT bool BM3dEntity_SetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxMatrix, mat));
LIBCMO_EXPORT bool BM3dEntity_GetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_meshid));
LIBCMO_EXPORT bool BM3dEntity_SetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, meshid));
LIBCMO_EXPORT bool BM3dEntity_GetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_isVisible));
LIBCMO_EXPORT bool BM3dEntity_SetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, is_visible));

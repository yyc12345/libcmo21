#pragma once

#include "BMap.hpp"

#pragma region Init / Dispose

LIBCMO_EXPORT void BMInit();
LIBCMO_EXPORT void BMDispose();

#pragma endregion

#pragma region BMFile

LIBCMO_EXPORT BMap::BMFile* BMFile_Load(LibCmo::CKSTRING file_name, LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]);
LIBCMO_EXPORT BMap::BMFile* BMFile_Create(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]);
LIBCMO_EXPORT bool BMFile_Save(BMap::BMFile* map_file, LibCmo::CKSTRING file_name, LibCmo::CKINT compreess_level);
LIBCMO_EXPORT void BMFile_Free(BMap::BMFile* map_file);

#pragma endregion

#pragma region BMMeshTransition


#pragma endregion



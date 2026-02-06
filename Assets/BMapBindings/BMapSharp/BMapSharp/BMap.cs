using System;
using System.Runtime.InteropServices;
using BMapSharp.BMapMarshalers;
using BMapSharp.VirtoolsTypes;

namespace BMapSharp {

    public static class BMap {

        /// <summary>The callback function of BMap.</summary>
        /// <param name="msg">The message content need to be printed.</param>
        internal delegate void OutputCallback([In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler))] string msg);

        // Decide the file name of loaded DLL.
#if BMAP_OS_WINDOWS
        private const string DLL_NAME = "BMap.dll";
#elif BMAP_OS_LINUX
        private const string DLL_NAME = "BMap.so";
#elif BMAP_OS_MACOS
        private const string DLL_NAME = "BMap.dylib";
#else
        private const string DLL_NAME = "BMap.bin";
#endif

        #region Function Defines

        // ##### GENERATED FUNCTIONS BEGIN #####

        /// <summary>BMInit</summary>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMInit", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMInit();
        /// <summary>BMDispose</summary>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMDispose", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMDispose();
        /// <summary>BMFile_Load</summary>
        /// <param name="file_name">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <param name="temp_folder">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <param name="texture_folder">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <param name="raw_callback">Direction: input. C++ type: BMap::NakedOutputCallback. </param>
        /// <param name="encoding_count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="encodings">Direction: input. C++ type: LibCmo::CKSTRING*. </param>
        /// <param name="out_file">Direction: output. C++ type: BMap::BMFile*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Load", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Load([In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string file_name, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string temp_folder, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string texture_folder, [In, MarshalAs(UnmanagedType.FunctionPtr)] OutputCallback raw_callback, [In, MarshalAs(UnmanagedType.U4)] uint encoding_count, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringArrayMarshaler))] string[] encodings, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_file);
        /// <summary>BMFile_Create</summary>
        /// <param name="temp_folder">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <param name="texture_folder">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <param name="raw_callback">Direction: input. C++ type: BMap::NakedOutputCallback. </param>
        /// <param name="encoding_count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="encodings">Direction: input. C++ type: LibCmo::CKSTRING*. </param>
        /// <param name="out_file">Direction: output. C++ type: BMap::BMFile*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Create", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Create([In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string temp_folder, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string texture_folder, [In, MarshalAs(UnmanagedType.FunctionPtr)] OutputCallback raw_callback, [In, MarshalAs(UnmanagedType.U4)] uint encoding_count, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringArrayMarshaler))] string[] encodings, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_file);
        /// <summary>BMFile_Save</summary>
        /// <param name="map_file">Direction: input. C++ type: BMap::BMFile*. </param>
        /// <param name="file_name">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <param name="texture_save_opt">Direction: input. C++ type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <param name="use_compress">Direction: input. C++ type: bool. </param>
        /// <param name="compreess_level">Direction: input. C++ type: LibCmo::CKINT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Save", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Save([In, MarshalAs(UnmanagedType.SysInt)] IntPtr map_file, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string file_name, [In, MarshalAs(UnmanagedType.U4)] CK_TEXTURE_SAVEOPTIONS texture_save_opt, [In, MarshalAs(UnmanagedType.U1)] bool use_compress, [In, MarshalAs(UnmanagedType.I4)] int compreess_level);
        /// <summary>BMFile_Free</summary>
        /// <param name="map_file">Direction: input. C++ type: BMap::BMFile*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Free", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Free([In, MarshalAs(UnmanagedType.SysInt)] IntPtr map_file);
        /// <summary>BMFile_GetGroupCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetGroupCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetGroupCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetGroup</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetGroup", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetGroup([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateGroup</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_CreateGroup", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateGroup([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_Get3dObjectCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Get3dObjectCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Get3dObjectCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_Get3dObject</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Get3dObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Get3dObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_Create3dObject</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_Create3dObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Create3dObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetMeshCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetMeshCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMeshCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetMesh</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateMesh</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_CreateMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetMaterialCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetMaterialCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMaterialCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetMaterial</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetMaterial", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMaterial([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateMaterial</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_CreateMaterial", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateMaterial([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetTextureCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetTextureCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTextureCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetTexture</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateTexture</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_CreateTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetTargetLightCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetTargetLightCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTargetLightCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetTargetLight</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetTargetLight", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTargetLight([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateTargetLight</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_CreateTargetLight", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateTargetLight([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetTargetCameraCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetTargetCameraCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTargetCameraCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetTargetCamera</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_GetTargetCamera", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTargetCamera([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateTargetCamera</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMFile_CreateTargetCamera", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateTargetCamera([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMMeshTrans_New</summary>
        /// <param name="out_trans">Direction: output. C++ type: BMap::BMMeshTransition*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_New", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_New([Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_trans);
        /// <summary>BMMeshTrans_Delete</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_Delete", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_Delete([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans);
        /// <summary>BMMeshTrans_PrepareVertexCount</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareVertexCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareVertexCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareVertex</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::VxMath::VxVector3*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareVertex", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareVertex([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareNormalCount</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareNormalCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareNormalCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareNormal</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::VxMath::VxVector3*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareNormal", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareNormal([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareUVCount</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareUVCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareUVCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareUV</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::VxMath::VxVector2*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareUV", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareUV([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareMtlSlotCount</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareMtlSlotCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareMtlSlotCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareMtlSlot</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CK2::CK_ID*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareMtlSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareMtlSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceCount</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareFaceCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareFaceVertexIndices</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CKDWORD*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareFaceVertexIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceVertexIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceNormalIndices</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CKDWORD*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareFaceNormalIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceNormalIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceUVIndices</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CKDWORD*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareFaceUVIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceUVIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceMtlSlot</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CKDWORD*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_PrepareFaceMtlSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceMtlSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_Parse</summary>
        /// <param name="trans">Direction: input. C++ type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. </param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMeshTrans_Parse", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_Parse([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid);
        /// <summary>BMObject_GetName</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_name">Direction: output. C++ type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMObject_GetName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMObject_GetName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler))] out string out_name);
        /// <summary>BMObject_SetName</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="name">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMObject_SetName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMObject_SetName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string name);
        /// <summary>BMGroup_AddObject</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="memberid">Direction: input. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMGroup_AddObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMGroup_AddObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint memberid);
        /// <summary>BMGroup_GetObjectCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMGroup_GetObjectCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMGroup_GetObjectCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMGroup_GetObject</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="pos">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_objid">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMGroup_GetObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMGroup_GetObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint pos, [Out, MarshalAs(UnmanagedType.U4)] out uint out_objid);
        /// <summary>BMTexture_GetFileName</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_filename">Direction: output. C++ type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_GetFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetFileName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler))] out string out_filename);
        /// <summary>BMTexture_LoadImage</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="filename">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_LoadImage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_LoadImage([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string filename);
        /// <summary>BMTexture_SaveImage</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="filename">Direction: input. C++ type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_SaveImage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SaveImage([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMOwnedStringMarshaler))] string filename);
        /// <summary>BMTexture_GetSaveOptions</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_saveopt">Direction: output. C++ type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_GetSaveOptions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetSaveOptions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out CK_TEXTURE_SAVEOPTIONS out_saveopt);
        /// <summary>BMTexture_SetSaveOptions</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="saveopt">Direction: input. C++ type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_SetSaveOptions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SetSaveOptions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] CK_TEXTURE_SAVEOPTIONS saveopt);
        /// <summary>BMTexture_GetVideoFormat</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_vfmt">Direction: output. C++ type: LibCmo::VxMath::VX_PIXELFORMAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_GetVideoFormat", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetVideoFormat([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VX_PIXELFORMAT out_vfmt);
        /// <summary>BMTexture_SetVideoFormat</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="vfmt">Direction: input. C++ type: LibCmo::VxMath::VX_PIXELFORMAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMTexture_SetVideoFormat", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SetVideoFormat([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VX_PIXELFORMAT vfmt);
        /// <summary>BMMaterial_GetDiffuse</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetDiffuse", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetDiffuse([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetDiffuse</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Direction: input. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetDiffuse", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetDiffuse([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetAmbient</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetAmbient", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAmbient([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetAmbient</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Direction: input. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetAmbient", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAmbient([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetSpecular</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetSpecular", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSpecular([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetSpecular</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Direction: input. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetSpecular", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSpecular([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetEmissive</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetEmissive", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetEmissive([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetEmissive</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Direction: input. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetEmissive", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetEmissive([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetSpecularPower</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetSpecularPower", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSpecularPower([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMMaterial_SetSpecularPower</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetSpecularPower", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSpecularPower([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMMaterial_GetTexture</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_texid">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_texid);
        /// <summary>BMMaterial_SetTexture</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="texid">Direction: input. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint texid);
        /// <summary>BMMaterial_GetTextureBorderColor</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTextureBorderColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureBorderColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetTextureBorderColor</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTextureBorderColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureBorderColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetTextureBlendMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTextureBlendMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureBlendMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_BLENDMODE out_val);
        /// <summary>BMMaterial_SetTextureBlendMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTextureBlendMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureBlendMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_BLENDMODE val);
        /// <summary>BMMaterial_GetTextureMinMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTextureMinMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureMinMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_FILTERMODE out_val);
        /// <summary>BMMaterial_SetTextureMinMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTextureMinMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureMinMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_FILTERMODE val);
        /// <summary>BMMaterial_GetTextureMagMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTextureMagMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureMagMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_FILTERMODE out_val);
        /// <summary>BMMaterial_SetTextureMagMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTextureMagMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureMagMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_FILTERMODE val);
        /// <summary>BMMaterial_GetTextureAddressMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTextureAddressMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureAddressMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_ADDRESSMODE out_val);
        /// <summary>BMMaterial_SetTextureAddressMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTextureAddressMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureAddressMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_ADDRESSMODE val);
        /// <summary>BMMaterial_GetSourceBlend</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetSourceBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSourceBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXBLEND_MODE out_val);
        /// <summary>BMMaterial_SetSourceBlend</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetSourceBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSourceBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXBLEND_MODE val);
        /// <summary>BMMaterial_GetDestBlend</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetDestBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetDestBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXBLEND_MODE out_val);
        /// <summary>BMMaterial_SetDestBlend</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetDestBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetDestBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXBLEND_MODE val);
        /// <summary>BMMaterial_GetFillMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXFILL_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetFillMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetFillMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXFILL_MODE out_val);
        /// <summary>BMMaterial_SetFillMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXFILL_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetFillMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetFillMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXFILL_MODE val);
        /// <summary>BMMaterial_GetShadeMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXSHADE_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetShadeMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetShadeMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXSHADE_MODE out_val);
        /// <summary>BMMaterial_SetShadeMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXSHADE_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetShadeMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetShadeMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXSHADE_MODE val);
        /// <summary>BMMaterial_GetAlphaTestEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetAlphaTestEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaTestEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetAlphaTestEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Direction: input. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetAlphaTestEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaTestEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetAlphaBlendEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetAlphaBlendEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaBlendEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetAlphaBlendEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Direction: input. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetAlphaBlendEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaBlendEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetPerspectiveCorrectionEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetPerspectiveCorrectionEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetPerspectiveCorrectionEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetPerspectiveCorrectionEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Direction: input. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetPerspectiveCorrectionEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetPerspectiveCorrectionEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetZWriteEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetZWriteEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetZWriteEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetZWriteEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Direction: input. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetZWriteEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetZWriteEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetTwoSidedEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetTwoSidedEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTwoSidedEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetTwoSidedEnabled</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Direction: input. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetTwoSidedEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTwoSidedEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetAlphaRef</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKBYTE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetAlphaRef", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaRef([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out byte out_val);
        /// <summary>BMMaterial_SetAlphaRef</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKBYTE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetAlphaRef", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaRef([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] byte val);
        /// <summary>BMMaterial_GetAlphaFunc</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetAlphaFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXCMPFUNC out_val);
        /// <summary>BMMaterial_SetAlphaFunc</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetAlphaFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXCMPFUNC val);
        /// <summary>BMMaterial_GetZFunc</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_GetZFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetZFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXCMPFUNC out_val);
        /// <summary>BMMaterial_SetZFunc</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMaterial_SetZFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetZFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXCMPFUNC val);
        /// <summary>BMMesh_GetLitMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mode">Direction: output. C++ type: LibCmo::VxMath::VXMESH_LITMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetLitMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetLitMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXMESH_LITMODE out_mode);
        /// <summary>BMMesh_SetLitMode</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="mode">Direction: input. C++ type: LibCmo::VxMath::VXMESH_LITMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_SetLitMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetLitMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXMESH_LITMODE mode);
        /// <summary>BMMesh_GetVertexCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetVertexCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMMesh_SetVertexCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_SetVertexCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetVertexCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMesh_GetVertexPositions</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::VxMath::VxVector3*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetVertexPositions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexPositions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetVertexNormals</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::VxMath::VxVector3*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetVertexNormals", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexNormals([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetVertexUVs</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::VxMath::VxVector2*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetVertexUVs", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexUVs([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetFaceCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetFaceCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetFaceCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMMesh_SetFaceCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_SetFaceCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetFaceCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMesh_GetFaceIndices</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CKWORD*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetFaceIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetFaceIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetFaceMaterialSlotIndexs</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Direction: output. C++ type: LibCmo::CKWORD*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetFaceMaterialSlotIndexs", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetFaceMaterialSlotIndexs([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetMaterialSlotCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetMaterialSlotCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetMaterialSlotCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMMesh_SetMaterialSlotCount</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="count">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_SetMaterialSlotCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetMaterialSlotCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMesh_GetMaterialSlot</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="index">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_mtlid">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_GetMaterialSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetMaterialSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint index, [Out, MarshalAs(UnmanagedType.U4)] out uint out_mtlid);
        /// <summary>BMMesh_SetMaterialSlot</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="index">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="mtlid">Direction: input. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMMesh_SetMaterialSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetMaterialSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint index, [In, MarshalAs(UnmanagedType.U4)] uint mtlid);
        /// <summary>BM3dEntity_GetWorldMatrix</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mat">Direction: output. C++ type: LibCmo::VxMath::VxMatrix. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BM3dEntity_GetWorldMatrix", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_GetWorldMatrix([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxMatrix out_mat);
        /// <summary>BM3dEntity_SetWorldMatrix</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="mat">Direction: input. C++ type: LibCmo::VxMath::VxMatrix. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BM3dEntity_SetWorldMatrix", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_SetWorldMatrix([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxMatrix mat);
        /// <summary>BM3dEntity_GetCurrentMesh</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_meshid">Direction: output. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BM3dEntity_GetCurrentMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_GetCurrentMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_meshid);
        /// <summary>BM3dEntity_SetCurrentMesh</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="meshid">Direction: input. C++ type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BM3dEntity_SetCurrentMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_SetCurrentMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint meshid);
        /// <summary>BM3dEntity_GetVisibility</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_isVisible">Direction: output. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BM3dEntity_GetVisibility", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_GetVisibility([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_isVisible);
        /// <summary>BM3dEntity_SetVisibility</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="is_visible">Direction: input. C++ type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BM3dEntity_SetVisibility", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_SetVisibility([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool is_visible);
        /// <summary>BMLight_GetType</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VXLIGHT_TYPE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetType", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetType([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXLIGHT_TYPE out_val);
        /// <summary>BMLight_SetType</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::VxMath::VXLIGHT_TYPE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetType", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetType([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXLIGHT_TYPE val);
        /// <summary>BMLight_GetColor</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMLight_SetColor</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Direction: input. C++ type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMLight_GetConstantAttenuation</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetConstantAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetConstantAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetConstantAttenuation</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetConstantAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetConstantAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetLinearAttenuation</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetLinearAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetLinearAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetLinearAttenuation</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetLinearAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetLinearAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetQuadraticAttenuation</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetQuadraticAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetQuadraticAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetQuadraticAttenuation</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetQuadraticAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetQuadraticAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetRange</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetRange", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetRange([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetRange</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetRange", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetRange([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetHotSpot</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetHotSpot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetHotSpot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetHotSpot</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetHotSpot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetHotSpot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetFalloff</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetFalloff", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetFalloff([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetFalloff</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetFalloff", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetFalloff([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetFalloffShape</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_GetFalloffShape", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetFalloffShape([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetFalloffShape</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMLight_SetFalloffShape", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetFalloffShape([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMCamera_GetProjectionType</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CK2::CK_CAMERA_PROJECTION. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_GetProjectionType", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_GetProjectionType([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out CK_CAMERA_PROJECTION out_val);
        /// <summary>BMCamera_SetProjectionType</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CK2::CK_CAMERA_PROJECTION. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_SetProjectionType", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_SetProjectionType([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] CK_CAMERA_PROJECTION val);
        /// <summary>BMCamera_GetOrthographicZoom</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_GetOrthographicZoom", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_GetOrthographicZoom([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMCamera_SetOrthographicZoom</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_SetOrthographicZoom", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_SetOrthographicZoom([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMCamera_GetFrontPlane</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_GetFrontPlane", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_GetFrontPlane([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMCamera_SetFrontPlane</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_SetFrontPlane", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_SetFrontPlane([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMCamera_GetBackPlane</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_GetBackPlane", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_GetBackPlane([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMCamera_SetBackPlane</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_SetBackPlane", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_SetBackPlane([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMCamera_GetFov</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Direction: output. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_GetFov", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_GetFov([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMCamera_SetFov</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Direction: input. C++ type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_SetFov", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_SetFov([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMCamera_GetAspectRatio</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_width">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="out_height">Direction: output. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_GetAspectRatio", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_GetAspectRatio([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_width, [Out, MarshalAs(UnmanagedType.U4)] out uint out_height);
        /// <summary>BMCamera_SetAspectRatio</summary>
        /// <param name="bmfile">Direction: input. C++ type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Direction: input. C++ type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="width">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <param name="height">Direction: input. C++ type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(DLL_NAME, EntryPoint = "BMCamera_SetAspectRatio", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMCamera_SetAspectRatio([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint width, [In, MarshalAs(UnmanagedType.U4)] uint height);

        // ##### GENERATED FUNCTIONS END #####

        #endregion

    }

}

using System;
using System.Runtime.InteropServices;
using System.Text;
using BMapSharp.VirtoolsTypes;

namespace BMapSharp {

    public static class BMap {

        /// <summary>BMapSharp module specific exception.</summary>
        public class BMapException : Exception {
            public BMapException() {}
            public BMapException(string message)
                : base(message) {}
            public BMapException(string message, Exception inner)
                : base(message, inner) {}
        }

        /// <summary>The callback function of BMap.</summary>
        /// <param name="msg">The message content need to be printed.</param>
        public delegate void OutputCallback([In, MarshalAs(UnmanagedType.LPUTF8Str)] string msg);

        /// <summary>The custom marshaler for BMap string array.</summary>
        public class BMStringArrayMarshaler : ICustomMarshaler {
            // References:
            // https://stackoverflow.com/questions/18498452/how-do-i-write-a-custom-marshaler-which-allows-data-to-flow-from-native-to-manag
            // https://learn.microsoft.com/en-us/dotnet/fundamentals/runtime-libraries/system-runtime-interopservices-icustommarshaler
            // 
            // NOTE: I do not create a member to store the object we are marshaling.
            // Because my binding do not have In, Out parameter. All parameters are In OR Out.
            // So there is no reason to keep that member.

            private static readonly BMStringArrayMarshaler g_Instance = new BMStringArrayMarshaler();
            public static ICustomMarshaler GetInstance(string pstrCookie) => g_Instance;

            // For respecting the standard of BMap, 
            // the native memory we created is a simple array and each item is a pointer to a NULL-terminated UTF8 string.
            // Please note the array self is not NULL-terminated because its length is provided by another argument in function calling.
            // However, this memory layout is not good for our marshaling.
            // We can not know the size of array we created. Because we need iterate it when freeing or fetching data.
            // We also can not know the size of string we created because we need read them when parsing them to C# string.
            // 
            // So the solution we made is adding an uint32_t header before the array to indicate the size of array.
            // And also add an uint32_t header for each string to indicate the length of string (in bytes, NULL exclusive).
            // So the pointer put in array is not the address we allocated, it has an offset.
            // Also we return native pointer is not the address we allocated, it also has an offset.

            private static readonly int szLengthHeaderSize = Marshal.SizeOf<int>();
            private static readonly int szArrayItemSize = Marshal.SizeOf<IntPtr>();
            private static readonly int szStringItemSize = Marshal.SizeOf<byte>();

            public IntPtr MarshalManagedToNative(object ManagedObj) {
                // Check nullptr object.
                if (ManagedObj is null) return IntPtr.Zero;
                // Check argument type.
                string[] castManagedObj = ManagedObj as string[];
                if (castManagedObj is null)
                    throw new MarshalDirectiveException("BMStringArrayMashaler must be used on an string array.");

                // Allocate string items first
                int szArrayItemCount = castManagedObj.Length;
                IntPtr[] apStrings = new IntPtr[szArrayItemCount];
                for (int i = 0; i < szArrayItemCount; ++i) {
                    // Encode string first.
                    byte[] encString = Encoding.UTF8.GetBytes(castManagedObj[i]);
                    // Allocate string memory with extra NULL terminal.
                    int szStringItemCount = encString.Length;
                    IntPtr pString = Marshal.AllocHGlobal(szStringItemSize * (szStringItemCount + 1) + szLengthHeaderSize);
                    // Setup length field
                    Marshal.WriteInt32(pString, 0, szStringItemCount);
                    // Copy string data with offset.
                    IntPtr pFakeString = pString + szLengthHeaderSize;
                    Marshal.Copy(encString, 0, pFakeString, szStringItemCount);
                    // Set NULL terminal.
                    Marshal.WriteByte(pFakeString, szStringItemSize * szStringItemCount, 0);
                    // Set item in string pointer
                    apStrings[i] = pFakeString;
                }

                // Allocate array pointer now.
                IntPtr pArray = Marshal.AllocHGlobal(szArrayItemSize * szArrayItemCount + szLengthHeaderSize);
                // Setup length field
                Marshal.WriteInt32(pArray, 0, szArrayItemCount);
                // Copy string pointer data with offset.
                IntPtr pFakeArray = pArray + szLengthHeaderSize;
                Marshal.Copy(apStrings, 0, pFakeArray, szArrayItemCount);

                // Return value
                return pFakeArray;
            }

            public object MarshalNativeToManaged(IntPtr pNativeData) {
                // Check nullptr
                if (pNativeData == IntPtr.Zero) return null;

                // Get real array pointer
                IntPtr pFakeArray = pNativeData;
                IntPtr pArray = pFakeArray - szLengthHeaderSize;

                // Get the count of array and read string pointers
                int szArrayItemCount = Marshal.ReadInt32(pArray, 0);
                IntPtr[] apStrings = new IntPtr[szArrayItemCount];
                Marshal.Copy(pFakeArray, apStrings, 0, szArrayItemCount);

                // Iterate the array and process each string one by one.
                string[] ret = new string[szArrayItemCount];
                for (int i = 0; i < szArrayItemCount; ++i) {
                    // Get string pointer
                    IntPtr pFakeString = apStrings[i];
                    if (pFakeString == IntPtr.Zero) {
                        ret[i] = null;
                        continue;
                    }
                    IntPtr pString = pFakeString - szLengthHeaderSize;
                    // Read string length
                    int szStringItemCount = Marshal.ReadInt32(pString, 0);
                    // Read string body
                    byte[] encString = new byte[szStringItemCount];
                    Marshal.Copy(pFakeString, encString, 0, szStringItemCount);
                    // Decode string with UTF8
                    ret[i] = Encoding.UTF8.GetString(encString);
                }

                // Return result
                return ret;
            }

            public void CleanUpNativeData(IntPtr pNativeData) {
                // Check nullptr
                if (pNativeData == IntPtr.Zero) return;

                // Get real array pointer
                IntPtr pFakeArray = pNativeData;
                IntPtr pArray = pFakeArray - szLengthHeaderSize;

                // Get the count of array and read string pointers
                int szArrayItemCount = Marshal.ReadInt32(pArray, 0);
                IntPtr[] apStrings = new IntPtr[szArrayItemCount];
                Marshal.Copy(pFakeArray, apStrings, 0, szArrayItemCount);

                // Iterate the array and free them one by one.
                for (int i = 0; i < szArrayItemCount; ++i) {
                    // Get string pointer
                    IntPtr pFakeString = apStrings[i];
                    if (pFakeString == IntPtr.Zero) continue;
                    IntPtr pString = pFakeString - szLengthHeaderSize;
                    // Free string pointer
                    Marshal.FreeHGlobal(pString);
                }

                // Free array self
                Marshal.FreeHGlobal(pArray);
            }

            public void CleanUpManagedData(object ManagedObj) {
                // Do nothing, because managed data do not need any clean up.
            }

            public int GetNativeDataSize() {
                // Return -1 to indicate the managed type this marshaler handles is not a value type.
                return -1;
            }

        }

        // Decide the file name of loaded DLL.

#if BMAP_OS_WINDOWS
        private const string g_DllName = "BMap.dll";
#elif BMAP_OS_LINUX
        private const string g_DllName = "BMap.so";
#elif BMAP_OS_MACOS
        private const string g_DllName = "BMap.dylib";
#else
        private const string g_DllName = "BMap.bin";
#endif

        #region Function Defines

        // ##### GENERATED FUNCTIONS BEGIN #####

        /// <summary>BMInit</summary>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMInit", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMInit();
        /// <summary>BMDispose</summary>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMDispose", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMDispose();
        /// <summary>BMFile_Load</summary>
        /// <param name="file_name">Type: LibCmo::CKSTRING. </param>
        /// <param name="temp_folder">Type: LibCmo::CKSTRING. </param>
        /// <param name="texture_folder">Type: LibCmo::CKSTRING. </param>
        /// <param name="raw_callback">Type: BMap::NakedOutputCallback. </param>
        /// <param name="encoding_count">Type: LibCmo::CKDWORD. </param>
        /// <param name="encodings">Type: LibCmo::CKSTRING*. </param>
        /// <param name="out_file">Type: BMap::BMFile*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Load", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Load([In, MarshalAs(UnmanagedType.LPUTF8Str)] string file_name, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string temp_folder, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string texture_folder, [In, MarshalAs(UnmanagedType.FunctionPtr)] OutputCallback raw_callback, [In, MarshalAs(UnmanagedType.U4)] uint encoding_count, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringArrayMarshaler))] string[] encodings, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_file);
        /// <summary>BMFile_Create</summary>
        /// <param name="temp_folder">Type: LibCmo::CKSTRING. </param>
        /// <param name="texture_folder">Type: LibCmo::CKSTRING. </param>
        /// <param name="raw_callback">Type: BMap::NakedOutputCallback. </param>
        /// <param name="encoding_count">Type: LibCmo::CKDWORD. </param>
        /// <param name="encodings">Type: LibCmo::CKSTRING*. </param>
        /// <param name="out_file">Type: BMap::BMFile*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Create", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Create([In, MarshalAs(UnmanagedType.LPUTF8Str)] string temp_folder, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string texture_folder, [In, MarshalAs(UnmanagedType.FunctionPtr)] OutputCallback raw_callback, [In, MarshalAs(UnmanagedType.U4)] uint encoding_count, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringArrayMarshaler))] string[] encodings, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_file);
        /// <summary>BMFile_Save</summary>
        /// <param name="map_file">Type: BMap::BMFile*. </param>
        /// <param name="file_name">Type: LibCmo::CKSTRING. </param>
        /// <param name="texture_save_opt">Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <param name="use_compress">Type: bool. </param>
        /// <param name="compreess_level">Type: LibCmo::CKINT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Save", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Save([In, MarshalAs(UnmanagedType.SysInt)] IntPtr map_file, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string file_name, [In, MarshalAs(UnmanagedType.U4)] uint texture_save_opt, [In, MarshalAs(UnmanagedType.U1)] bool use_compress, [In, MarshalAs(UnmanagedType.I4)] int compreess_level);
        /// <summary>BMFile_Free</summary>
        /// <param name="map_file">Type: BMap::BMFile*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Free", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Free([In, MarshalAs(UnmanagedType.SysInt)] IntPtr map_file);
        /// <summary>BMFile_GetGroupCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetGroupCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetGroupCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetGroup</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetGroup", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetGroup([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateGroup</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_CreateGroup", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateGroup([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_Get3dObjectCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Get3dObjectCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Get3dObjectCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_Get3dObject</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Get3dObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Get3dObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_Create3dObject</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Create3dObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Create3dObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetMeshCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetMeshCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMeshCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetMesh</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateMesh</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_CreateMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetMaterialCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetMaterialCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMaterialCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetMaterial</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetMaterial", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetMaterial([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateMaterial</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_CreateMaterial", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateMaterial([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_GetTextureCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetTextureCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTextureCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetTexture</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateTexture</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_CreateTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMMeshTrans_New</summary>
        /// <param name="out_trans">Type: BMap::BMMeshTransition*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_New", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_New([Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_trans);
        /// <summary>BMMeshTrans_Delete</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_Delete", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_Delete([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans);
        /// <summary>BMMeshTrans_PrepareVertexCount</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareVertexCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareVertexCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareVertex</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::VxMath::VxVector3*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareVertex", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareVertex([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareNormalCount</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareNormalCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareNormalCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareNormal</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::VxMath::VxVector3*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareNormal", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareNormal([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareUVCount</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareUVCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareUVCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareUV</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::VxMath::VxVector2*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareUV", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareUV([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareMtlSlotCount</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareMtlSlotCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareMtlSlotCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareMtlSlot</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::CK2::CK_ID*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareMtlSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareMtlSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceCount</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareFaceCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMeshTrans_PrepareFaceVertexIndices</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::CKDWORD*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareFaceVertexIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceVertexIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceNormalIndices</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::CKDWORD*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareFaceNormalIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceNormalIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceUVIndices</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::CKDWORD*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareFaceUVIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceUVIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_PrepareFaceMtlSlot</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="out_mem">Type: LibCmo::CKDWORD*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_PrepareFaceMtlSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_PrepareFaceMtlSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMeshTrans_Parse</summary>
        /// <param name="trans">Type: BMap::BMMeshTransition*. The pointer to corresponding BMMeshTransition.</param>
        /// <param name="bmfile">Type: BMap::BMFile*. </param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMeshTrans_Parse", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMeshTrans_Parse([In, MarshalAs(UnmanagedType.SysInt)] IntPtr trans, [In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid);
        /// <summary>BMObject_GetName</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_name">Type: LibCmo::CKSTRING. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMObject_GetName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMObject_GetName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.LPUTF8Str)] out string out_name);
        /// <summary>BMObject_SetName</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="name">Type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMObject_SetName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMObject_SetName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string name);
        /// <summary>BMGroup_AddObject</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="memberid">Type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMGroup_AddObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMGroup_AddObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint memberid);
        /// <summary>BMGroup_GetObjectCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMGroup_GetObjectCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMGroup_GetObjectCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMGroup_GetObject</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="pos">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_objid">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMGroup_GetObject", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMGroup_GetObject([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint pos, [Out, MarshalAs(UnmanagedType.U4)] out uint out_objid);
        /// <summary>BMTexture_GetFileName</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_filename">Type: LibCmo::CKSTRING. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_GetFileName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetFileName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.LPUTF8Str)] out string out_filename);
        /// <summary>BMTexture_LoadImage</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="filename">Type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_LoadImage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_LoadImage([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string filename);
        /// <summary>BMTexture_SaveImage</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="filename">Type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_SaveImage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SaveImage([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.LPUTF8Str)] string filename);
        /// <summary>BMTexture_GetSaveOptions</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_saveopt">Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_GetSaveOptions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetSaveOptions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_saveopt);
        /// <summary>BMTexture_SetSaveOptions</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="saveopt">Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_SetSaveOptions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SetSaveOptions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint saveopt);
        /// <summary>BMTexture_GetVideoFormat</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_vfmt">Type: LibCmo::VxMath::VX_PIXELFORMAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_GetVideoFormat", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetVideoFormat([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_vfmt);
        /// <summary>BMTexture_SetVideoFormat</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="vfmt">Type: LibCmo::VxMath::VX_PIXELFORMAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_SetVideoFormat", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SetVideoFormat([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint vfmt);
        /// <summary>BMMaterial_GetDiffuse</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VxColor. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetDiffuse", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetDiffuse([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetDiffuse</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetDiffuse", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetDiffuse([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetAmbient</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VxColor. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetAmbient", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAmbient([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetAmbient</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAmbient", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAmbient([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetSpecular</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VxColor. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetSpecular", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSpecular([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetSpecular</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetSpecular", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSpecular([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetEmissive</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VxColor. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetEmissive", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetEmissive([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMMaterial_SetEmissive</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetEmissive", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetEmissive([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMMaterial_GetSpecularPower</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetSpecularPower", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSpecularPower([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMMaterial_SetSpecularPower</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetSpecularPower", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSpecularPower([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMMaterial_GetTexture</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_texid">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_texid);
        /// <summary>BMMaterial_SetTexture</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="texid">Type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTexture", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTexture([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint texid);
        /// <summary>BMMaterial_GetTextureBorderColor</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureBorderColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureBorderColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetTextureBorderColor</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureBorderColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureBorderColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetTextureBlendMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureBlendMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureBlendMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetTextureBlendMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureBlendMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureBlendMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetTextureMinMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureMinMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureMinMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetTextureMinMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureMinMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureMinMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetTextureMagMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureMagMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureMagMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetTextureMagMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureMagMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureMagMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetTextureAddressMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureAddressMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureAddressMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetTextureAddressMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureAddressMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureAddressMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetSourceBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXBLEND_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetSourceBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSourceBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetSourceBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetSourceBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSourceBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetDestBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXBLEND_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetDestBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetDestBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetDestBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetDestBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetDestBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetFillMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXFILL_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetFillMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetFillMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetFillMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXFILL_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetFillMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetFillMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetShadeMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXSHADE_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetShadeMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetShadeMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetShadeMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXSHADE_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetShadeMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetShadeMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetAlphaTestEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetAlphaTestEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaTestEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetAlphaTestEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAlphaTestEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaTestEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetAlphaBlendEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetAlphaBlendEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaBlendEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetAlphaBlendEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAlphaBlendEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaBlendEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetPerspectiveCorrectionEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetPerspectiveCorrectionEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetPerspectiveCorrectionEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetPerspectiveCorrectionEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetPerspectiveCorrectionEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetPerspectiveCorrectionEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetZWriteEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetZWriteEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetZWriteEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetZWriteEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetZWriteEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetZWriteEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetTwoSidedEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTwoSidedEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTwoSidedEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_val);
        /// <summary>BMMaterial_SetTwoSidedEnabled</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="enabled">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTwoSidedEnabled", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTwoSidedEnabled([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool enabled);
        /// <summary>BMMaterial_GetAlphaRef</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKBYTE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetAlphaRef", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaRef([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetAlphaRef</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKBYTE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAlphaRef", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaRef([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetAlphaFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXCMPFUNC. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetAlphaFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetAlphaFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAlphaFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMaterial_GetZFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXCMPFUNC. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetZFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetZFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_val);
        /// <summary>BMMaterial_SetZFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetZFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetZFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint val);
        /// <summary>BMMesh_GetLitMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mode">Type: LibCmo::VxMath::VXMESH_LITMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetLitMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetLitMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_mode);
        /// <summary>BMMesh_SetLitMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="mode">Type: LibCmo::VxMath::VXMESH_LITMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_SetLitMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetLitMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint mode);
        /// <summary>BMMesh_GetVertexCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetVertexCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMMesh_SetVertexCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_SetVertexCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetVertexCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMesh_GetVertexPositions</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Type: LibCmo::VxMath::VxVector3*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetVertexPositions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexPositions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetVertexNormals</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Type: LibCmo::VxMath::VxVector3*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetVertexNormals", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexNormals([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetVertexUVs</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Type: LibCmo::VxMath::VxVector2*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetVertexUVs", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetVertexUVs([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetFaceCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetFaceCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetFaceCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMMesh_SetFaceCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_SetFaceCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetFaceCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMesh_GetFaceIndices</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Type: LibCmo::CKWORD*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetFaceIndices", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetFaceIndices([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetFaceMaterialSlotIndexs</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mem">Type: LibCmo::CKWORD*. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetFaceMaterialSlotIndexs", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetFaceMaterialSlotIndexs([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_mem);
        /// <summary>BMMesh_GetMaterialSlotCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetMaterialSlotCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetMaterialSlotCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMMesh_SetMaterialSlotCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="count">Type: LibCmo::CKDWORD. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_SetMaterialSlotCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetMaterialSlotCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint count);
        /// <summary>BMMesh_GetMaterialSlot</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="index">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_mtlid">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetMaterialSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetMaterialSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint index, [Out, MarshalAs(UnmanagedType.U4)] out uint out_mtlid);
        /// <summary>BMMesh_SetMaterialSlot</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="index">Type: LibCmo::CKDWORD. </param>
        /// <param name="mtlid">Type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_SetMaterialSlot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetMaterialSlot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint index, [In, MarshalAs(UnmanagedType.U4)] uint mtlid);
        /// <summary>BM3dObject_GetWorldMatrix</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mat">Type: LibCmo::VxMath::VxMatrix. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dObject_GetWorldMatrix", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dObject_GetWorldMatrix([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxMatrix out_mat);
        /// <summary>BM3dObject_SetWorldMatrix</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="mat">Type: LibCmo::VxMath::VxMatrix. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dObject_SetWorldMatrix", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dObject_SetWorldMatrix([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxMatrix mat);
        /// <summary>BM3dObject_GetCurrentMesh</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_meshid">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dObject_GetCurrentMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dObject_GetCurrentMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_meshid);
        /// <summary>BM3dObject_SetCurrentMesh</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="meshid">Type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dObject_SetCurrentMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dObject_SetCurrentMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint meshid);
        /// <summary>BM3dObject_GetVisibility</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_isVisible">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dObject_GetVisibility", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dObject_GetVisibility([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_isVisible);
        /// <summary>BM3dObject_SetVisibility</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="is_visible">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dObject_SetVisibility", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dObject_SetVisibility([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool is_visible);

        // ##### GENERATED FUNCTIONS END #####

        #endregion

    }

}

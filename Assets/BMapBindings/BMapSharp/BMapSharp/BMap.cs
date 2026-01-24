using System;
using System.Runtime.InteropServices;
using System.Text;
using BMapSharp.VirtoolsTypes;

namespace BMapSharp {

    /// <summary>BMapSharp module specific exception.</summary>
    public class BMapException : Exception {
        public BMapException() {}
        public BMapException(string message)
            : base(message) {}
        public BMapException(string message, Exception inner)
            : base(message, inner) {}
        public static void ThrowIfFailed(bool condition) {
            if (!condition) throw new BMapException("BMap operation failed.");
        }
    }

    public static class BMap {

        #region Custom Marshalers

        // References:
        // https://stackoverflow.com/questions/18498452/how-do-i-write-a-custom-marshaler-which-allows-data-to-flow-from-native-to-manag
        // https://learn.microsoft.com/en-us/dotnet/fundamentals/runtime-libraries/system-runtime-interopservices-icustommarshaler
        // 
        // NOTE: I do not create a member to store the object we are marshaling.
        // Because my binding do not have In, Out parameter. All parameters are In OR Out.
        // So there is no reason to keep that member.

        // IDK why Microsoft try to call ICustomMarshaler.CleanUpNativeData without calling ICustomMarshaler.MarshalManagedToNative.
        // It is trying to free the pointer managed by LibCmo self (for example, it will try to free we got string when getting object name)!
        // So as the compromise, we use "cookie" feature to explicit specify the marshaler In/Out behavior when getting it.
        [Flags]
        internal enum MarshalerType {
            None = 0b0,
            In = 0b1,
            Out = 0b10
        }

        /// <summary>The custom marshaler for BMap string array.</summary>
        public class BMStringArrayMarshaler : ICustomMarshaler {
            private static readonly BMStringArrayMarshaler g_InInstance = new BMStringArrayMarshaler(MarshalerType.In);
            private static readonly BMStringArrayMarshaler g_OutInstance = new BMStringArrayMarshaler(MarshalerType.Out);
            public static ICustomMarshaler GetInstance(string pstrCookie) {
                if (pstrCookie == "In") return g_InInstance;
                else if (pstrCookie == "Out") return g_OutInstance;
                else throw new MarshalDirectiveException("Not supported cookie string for BMStringArrayMarshaler.");
            }

            private readonly MarshalerType m_MarshalerType;
            private BMStringArrayMarshaler(MarshalerType marshaler_type) {
                m_MarshalerType = marshaler_type;
            }

            // For respecting the standard of BMap, 
            // the native memory we created is a simple array and each item is a pointer to a NULL-terminated UTF8 string.
            // Please note the array self is also NULL-terminated otherwise we don't know its length.

            public IntPtr MarshalManagedToNative(object ManagedObj) {
                // Check marshaler type
                if (!m_MarshalerType.HasFlag(MarshalerType.In)) return IntPtr.Zero;
                // Check nullptr object.
                if (ManagedObj is null) return IntPtr.Zero;
                // Check argument type.
                string[] castManagedObj = ManagedObj as string[];
                if (castManagedObj is null)
                    throw new MarshalDirectiveException("BMStringArrayMashaler must be used on an string array.");

                // Allocate string items first
                int szArrayItemCount = castManagedObj.Length;
                int szArrayItemSize = Marshal.SizeOf<IntPtr>();
                IntPtr[] apString = new IntPtr[szArrayItemCount];
                for (int i = 0; i < szArrayItemCount; ++i) {
                    // Check null string
                    string stringObj = castManagedObj[i];
                    if (stringObj is null) apString[i] = IntPtr.Zero;
                    else apString[i] = BMStringMarshaler.ToNative(stringObj);
                }

                // Allocate array pointer now.
                IntPtr pArray = Marshal.AllocHGlobal(szArrayItemSize * (szArrayItemCount + 1));
                // Copy string pointer data
                Marshal.Copy(apString, 0, pArray, szArrayItemCount);
                // Setup NULL ternimal
                Marshal.WriteIntPtr(pArray + (szArrayItemSize * szArrayItemCount), IntPtr.Zero);

                // Return value
                return pArray;
            }

            public object MarshalNativeToManaged(IntPtr pNativeData) {
                // Check marshaler type
                if (!m_MarshalerType.HasFlag(MarshalerType.Out)) return null;
                // Check nullptr
                if (pNativeData == IntPtr.Zero) return null;

                // Get the length of array
                int szArrayItemCount = BMStringArrayMarshaler.GetArrayLength(pNativeData);
                int szArrayItemSize = Marshal.SizeOf<IntPtr>();
                // Prepare array cache and read it.
                IntPtr[] apString = new IntPtr[szArrayItemCount];
                Marshal.Copy(pNativeData, apString, 0, szArrayItemCount);

                // Iterate the array and process each string one by one.
                string[] ret = new string[szArrayItemCount];
                for (int i = 0; i < szArrayItemCount; ++i) {
                    // Get string pointer
                    IntPtr pString = apString[i];
                    if (pString == IntPtr.Zero) {
                        ret[i] = null;
                        continue;
                    }
                    // Extract string
                    ret[i] = BMStringMarshaler.ToManaged(pString);
                }

                // Return result
                return ret;
            }

            public void CleanUpNativeData(IntPtr pNativeData) {
                // Check marshaler type
                if (!m_MarshalerType.HasFlag(MarshalerType.In)) return;
                // Check nullptr
                if (pNativeData == IntPtr.Zero) return;

                // Get the length of array
                int szArrayItemCount = BMStringArrayMarshaler.GetArrayLength(pNativeData);
                int szArrayItemSize = Marshal.SizeOf<IntPtr>();
                // Prepare array cache and read it.
                IntPtr[] apString = new IntPtr[szArrayItemCount];
                Marshal.Copy(pNativeData, apString, 0, szArrayItemCount);
                // Free array self
                Marshal.FreeHGlobal(pNativeData);

                // Iterate the string pointer array and free them one by one.
                foreach (IntPtr pString in apString) {
                    // Free string pointer
                    if (pString == IntPtr.Zero) continue;
                    Marshal.FreeHGlobal(pString);
                }
            }

            public void CleanUpManagedData(object ManagedObj) {
                // Do nothing, because managed data do not need any clean up.
            }

            public int GetNativeDataSize() {
                // Return -1 to indicate the managed type this marshaler handles is not a value type.
                return -1;
            }

            /// <summary>
            /// Return the length of array created by this marshaler.
            /// </summary>
            /// <param name="ptr">The pointer to array for checking.</param>
            /// <returns>The length of array (NULL terminal exclusive).</returns>
            internal static int GetArrayLength(IntPtr ptr) {
                int count = 0, unit = Marshal.SizeOf<IntPtr>();
                while (Marshal.ReadIntPtr(ptr) != IntPtr.Zero) {
                    ptr += unit;
                    ++count;
                }
                return count;
            }
        }

        public class BMStringMarshaler : ICustomMarshaler {
            private static readonly BMStringMarshaler g_InInstance = new BMStringMarshaler(MarshalerType.In);
            private static readonly BMStringMarshaler g_OutInstance = new BMStringMarshaler(MarshalerType.Out);
            public static ICustomMarshaler GetInstance(string pstrCookie) {
                if (pstrCookie == "In") return g_InInstance;
                else if (pstrCookie == "Out") return g_OutInstance;
                else throw new MarshalDirectiveException("Not supported cookie string for BMStringMarshaler.");
            }

            private readonly MarshalerType m_MarshalerType;
            private BMStringMarshaler(MarshalerType marshaler_type) {
                m_MarshalerType = marshaler_type;
            }

            public IntPtr MarshalManagedToNative(object ManagedObj) {
                // Check marshaler type
                if (!m_MarshalerType.HasFlag(MarshalerType.In)) return IntPtr.Zero;
                // Check requirements.
                if (ManagedObj is null) return IntPtr.Zero;
                string castManagedObj = ManagedObj as string;
                if (castManagedObj is null)
                    throw new MarshalDirectiveException("BMStringMarshaler must be used on a string.");
                // Call self
                return BMStringMarshaler.ToNative(castManagedObj);
            }

            public object MarshalNativeToManaged(IntPtr pNativeData) {
                // Check marshaler type
                if (!m_MarshalerType.HasFlag(MarshalerType.Out)) return null;
                // Check nullptr
                if (pNativeData == IntPtr.Zero) return null;
                // Call self
                return BMStringMarshaler.ToManaged(pNativeData);
            }

            public void CleanUpNativeData(IntPtr pNativeData) {
                // Check marshaler type
                if (!m_MarshalerType.HasFlag(MarshalerType.In)) return;
                // Check nullptr
                if (pNativeData == IntPtr.Zero) return;
                // Free native pointer
                Marshal.FreeHGlobal(pNativeData);
            }

            public void CleanUpManagedData(object ManagedObj) {
                // Do nothing, because managed data do not need any clean up.
            }

            public int GetNativeDataSize() {
                // Return -1 to indicate the managed type this marshaler handles is not a value type.
                return -1;
            }

            /// <summary>
            /// Return the length in byte of given pointer represented C style string.
            /// </summary>
            /// <param name="ptr">The pointer for checking.</param>
            /// <returns>The length of C style string (NUL exclusive).</returns>
            internal static int GetCStringLength(IntPtr ptr) {
                int count = 0, unit = Marshal.SizeOf<byte>();
                while (Marshal.ReadByte(ptr) != (byte)0) {
                    ptr += unit;
                    ++count;
                }
                return count;
            }
            
            /// <summary>
            /// Convert given string object to native data.
            /// This function is shared by 2 marshalers.
            /// </summary>
            /// <param name="obj">String object. Caller must make sure this object is not null.</param>
            /// <returns>The created native data pointer.</returns>
            internal static IntPtr ToNative(string obj) {
                // Encode string first
                byte[] encString = Encoding.UTF8.GetBytes(obj);
                // Allocate string memory with extra NUL.
                int szStringItemCount = encString.Length;
                int szStringItemSize = Marshal.SizeOf<byte>();
                IntPtr pString = Marshal.AllocHGlobal(szStringItemSize * (szStringItemCount + 1));
                // Copy encoded string data
                Marshal.Copy(encString, 0, pString, szStringItemCount);
                // Setup NUL
                Marshal.WriteByte(pString + (szStringItemSize * szStringItemCount), (byte)0);
                // Return value
                return pString;
            }
            /// <summary>
            /// Extract managed string from given native pointer holding C style string data.
            /// This function is shared by 2 marshalers.
            /// </summary>
            /// <param name="ptr">Native pointer holding string data. Caller must make sure this pointer is not nullptr.</param>
            /// <returns>The extracted managed string data.</returns>
            internal static string ToManaged(IntPtr ptr) {
                // Get the length of given string.
                int szStringItemCount = BMStringMarshaler.GetCStringLength(ptr);
                int szStringItemSize = Marshal.SizeOf<byte>();
                // Prepare cache and copy string data
                byte[] encString = new byte[szStringItemCount];
                Marshal.Copy(ptr, encString, 0, szStringItemCount);
                // Decode string and return
                return Encoding.UTF8.GetString(encString);
            }
        }

        #endregion

        /// <summary>The callback function of BMap.</summary>
        /// <param name="msg">The message content need to be printed.</param>
        internal delegate void OutputCallback([In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler))] string msg);

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
        internal static extern bool BMFile_Load([In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string file_name, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string temp_folder, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string texture_folder, [In, MarshalAs(UnmanagedType.FunctionPtr)] OutputCallback raw_callback, [In, MarshalAs(UnmanagedType.U4)] uint encoding_count, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringArrayMarshaler), MarshalCookie = "In")] string[] encodings, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_file);
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
        internal static extern bool BMFile_Create([In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string temp_folder, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string texture_folder, [In, MarshalAs(UnmanagedType.FunctionPtr)] OutputCallback raw_callback, [In, MarshalAs(UnmanagedType.U4)] uint encoding_count, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringArrayMarshaler), MarshalCookie = "In")] string[] encodings, [Out, MarshalAs(UnmanagedType.SysInt)] out IntPtr out_file);
        /// <summary>BMFile_Save</summary>
        /// <param name="map_file">Type: BMap::BMFile*. </param>
        /// <param name="file_name">Type: LibCmo::CKSTRING. </param>
        /// <param name="texture_save_opt">Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <param name="use_compress">Type: bool. </param>
        /// <param name="compreess_level">Type: LibCmo::CKINT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_Save", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_Save([In, MarshalAs(UnmanagedType.SysInt)] IntPtr map_file, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string file_name, [In, MarshalAs(UnmanagedType.U4)] CK_TEXTURE_SAVEOPTIONS texture_save_opt, [In, MarshalAs(UnmanagedType.U1)] bool use_compress, [In, MarshalAs(UnmanagedType.I4)] int compreess_level);
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
        /// <summary>BMFile_GetTargetLightCount</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_count">Type: LibCmo::CKDWORD. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetTargetLightCount", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTargetLightCount([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_count);
        /// <summary>BMFile_GetTargetLight</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="idx">Type: LibCmo::CKDWORD. </param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_GetTargetLight", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_GetTargetLight([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint idx, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
        /// <summary>BMFile_CreateTargetLight</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="out_id">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMFile_CreateTargetLight", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMFile_CreateTargetLight([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [Out, MarshalAs(UnmanagedType.U4)] out uint out_id);
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
        internal static extern bool BMObject_GetName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "Out")] out string out_name);
        /// <summary>BMObject_SetName</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="name">Type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMObject_SetName", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMObject_SetName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string name);
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
        internal static extern bool BMTexture_GetFileName([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "Out")] out string out_filename);
        /// <summary>BMTexture_LoadImage</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="filename">Type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_LoadImage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_LoadImage([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string filename);
        /// <summary>BMTexture_SaveImage</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="filename">Type: LibCmo::CKSTRING. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_SaveImage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SaveImage([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(BMStringMarshaler), MarshalCookie = "In")] string filename);
        /// <summary>BMTexture_GetSaveOptions</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_saveopt">Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_GetSaveOptions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetSaveOptions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out CK_TEXTURE_SAVEOPTIONS out_saveopt);
        /// <summary>BMTexture_SetSaveOptions</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="saveopt">Type: LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_SetSaveOptions", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SetSaveOptions([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] CK_TEXTURE_SAVEOPTIONS saveopt);
        /// <summary>BMTexture_GetVideoFormat</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_vfmt">Type: LibCmo::VxMath::VX_PIXELFORMAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_GetVideoFormat", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_GetVideoFormat([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VX_PIXELFORMAT out_vfmt);
        /// <summary>BMTexture_SetVideoFormat</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="vfmt">Type: LibCmo::VxMath::VX_PIXELFORMAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMTexture_SetVideoFormat", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMTexture_SetVideoFormat([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VX_PIXELFORMAT vfmt);
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
        internal static extern bool BMMaterial_GetTextureBlendMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_BLENDMODE out_val);
        /// <summary>BMMaterial_SetTextureBlendMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_BLENDMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureBlendMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureBlendMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_BLENDMODE val);
        /// <summary>BMMaterial_GetTextureMinMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureMinMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureMinMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_FILTERMODE out_val);
        /// <summary>BMMaterial_SetTextureMinMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureMinMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureMinMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_FILTERMODE val);
        /// <summary>BMMaterial_GetTextureMagMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureMagMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureMagMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_FILTERMODE out_val);
        /// <summary>BMMaterial_SetTextureMagMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_FILTERMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureMagMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureMagMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_FILTERMODE val);
        /// <summary>BMMaterial_GetTextureAddressMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetTextureAddressMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetTextureAddressMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXTEXTURE_ADDRESSMODE out_val);
        /// <summary>BMMaterial_SetTextureAddressMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXTEXTURE_ADDRESSMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetTextureAddressMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetTextureAddressMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXTEXTURE_ADDRESSMODE val);
        /// <summary>BMMaterial_GetSourceBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXBLEND_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetSourceBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetSourceBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXBLEND_MODE out_val);
        /// <summary>BMMaterial_SetSourceBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetSourceBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetSourceBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXBLEND_MODE val);
        /// <summary>BMMaterial_GetDestBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXBLEND_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetDestBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetDestBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXBLEND_MODE out_val);
        /// <summary>BMMaterial_SetDestBlend</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXBLEND_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetDestBlend", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetDestBlend([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXBLEND_MODE val);
        /// <summary>BMMaterial_GetFillMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXFILL_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetFillMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetFillMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXFILL_MODE out_val);
        /// <summary>BMMaterial_SetFillMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXFILL_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetFillMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetFillMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXFILL_MODE val);
        /// <summary>BMMaterial_GetShadeMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXSHADE_MODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetShadeMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetShadeMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXSHADE_MODE out_val);
        /// <summary>BMMaterial_SetShadeMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXSHADE_MODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetShadeMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetShadeMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXSHADE_MODE val);
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
        internal static extern bool BMMaterial_GetAlphaRef([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out byte out_val);
        /// <summary>BMMaterial_SetAlphaRef</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKBYTE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAlphaRef", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaRef([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] byte val);
        /// <summary>BMMaterial_GetAlphaFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXCMPFUNC. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetAlphaFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetAlphaFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXCMPFUNC out_val);
        /// <summary>BMMaterial_SetAlphaFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetAlphaFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetAlphaFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXCMPFUNC val);
        /// <summary>BMMaterial_GetZFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXCMPFUNC. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_GetZFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_GetZFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXCMPFUNC out_val);
        /// <summary>BMMaterial_SetZFunc</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXCMPFUNC. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMaterial_SetZFunc", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMaterial_SetZFunc([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXCMPFUNC val);
        /// <summary>BMMesh_GetLitMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mode">Type: LibCmo::VxMath::VXMESH_LITMODE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_GetLitMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_GetLitMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXMESH_LITMODE out_mode);
        /// <summary>BMMesh_SetLitMode</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="mode">Type: LibCmo::VxMath::VXMESH_LITMODE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMMesh_SetLitMode", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMMesh_SetLitMode([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXMESH_LITMODE mode);
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
        /// <summary>BM3dEntity_GetWorldMatrix</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_mat">Type: LibCmo::VxMath::VxMatrix. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dEntity_GetWorldMatrix", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_GetWorldMatrix([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxMatrix out_mat);
        /// <summary>BM3dEntity_SetWorldMatrix</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="mat">Type: LibCmo::VxMath::VxMatrix. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dEntity_SetWorldMatrix", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_SetWorldMatrix([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxMatrix mat);
        /// <summary>BM3dEntity_GetCurrentMesh</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_meshid">Type: LibCmo::CK2::CK_ID. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dEntity_GetCurrentMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_GetCurrentMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out uint out_meshid);
        /// <summary>BM3dEntity_SetCurrentMesh</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="meshid">Type: LibCmo::CK2::CK_ID. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dEntity_SetCurrentMesh", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_SetCurrentMesh([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] uint meshid);
        /// <summary>BM3dEntity_GetVisibility</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_isVisible">Type: bool. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dEntity_GetVisibility", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_GetVisibility([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U1)] out bool out_isVisible);
        /// <summary>BM3dEntity_SetVisibility</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="is_visible">Type: bool. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BM3dEntity_SetVisibility", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BM3dEntity_SetVisibility([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U1)] bool is_visible);
        /// <summary>BMLight_GetType</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VXLIGHT_TYPE. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetType", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetType([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.U4)] out VXLIGHT_TYPE out_val);
        /// <summary>BMLight_SetType</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::VxMath::VXLIGHT_TYPE. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetType", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetType([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.U4)] VXLIGHT_TYPE val);
        /// <summary>BMLight_GetColor</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::VxMath::VxColor. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.Struct)] out VxColor out_val);
        /// <summary>BMLight_SetColor</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="col">Type: LibCmo::VxMath::VxColor. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetColor", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetColor([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.Struct)] VxColor col);
        /// <summary>BMLight_GetConstantAttenuation</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetConstantAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetConstantAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetConstantAttenuation</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetConstantAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetConstantAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetLinearAttenuation</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetLinearAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetLinearAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetLinearAttenuation</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetLinearAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetLinearAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetQuadraticAttenuation</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetQuadraticAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetQuadraticAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetQuadraticAttenuation</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetQuadraticAttenuation", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetQuadraticAttenuation([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetRange</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetRange", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetRange([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetRange</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetRange", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetRange([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetHotSpot</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetHotSpot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetHotSpot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetHotSpot</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetHotSpot", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetHotSpot([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetFalloff</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetFalloff", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetFalloff([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetFalloff</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetFalloff", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetFalloff([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);
        /// <summary>BMLight_GetFalloffShape</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="out_val">Type: LibCmo::CKFLOAT. This is OUT parameter. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_GetFalloffShape", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_GetFalloffShape([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [Out, MarshalAs(UnmanagedType.R4)] out float out_val);
        /// <summary>BMLight_SetFalloffShape</summary>
        /// <param name="bmfile">Type: BMap::BMFile*. The pointer to corresponding BMFile.</param>
        /// <param name="objid">Type: LibCmo::CK2::CK_ID. The CKID of object you accessing.</param>
        /// <param name="val">Type: LibCmo::CKFLOAT. </param>
        /// <returns>True if no error, otherwise False.</returns>
        [DllImport(g_DllName, EntryPoint = "BMLight_SetFalloffShape", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, ExactSpelling = true)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool BMLight_SetFalloffShape([In, MarshalAs(UnmanagedType.SysInt)] IntPtr bmfile, [In, MarshalAs(UnmanagedType.U4)] uint objid, [In, MarshalAs(UnmanagedType.R4)] float val);

        // ##### GENERATED FUNCTIONS END #####

        #endregion

    }

}

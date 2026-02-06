using System;
using System.Text;
using System.Runtime.InteropServices;

namespace BMapSharp.BMapMarshalers {

    // References:
    // https://stackoverflow.com/questions/18498452/how-do-i-write-a-custom-marshaler-which-allows-data-to-flow-from-native-to-manag
    // https://learn.microsoft.com/en-us/dotnet/fundamentals/runtime-libraries/system-runtime-interopservices-icustommarshaler
    // 
    // NOTE: I do not create a member to store the object we are marshaling.
    // Because my binding do not have In, Out parameter. All parameters are In OR Out.
    // So there is no reason to keep that member.

    // YYC MARK:
    // When receiving UTF8 string pointer given by BMap as managed string,
    // I don't know why Microsoft try to call ICustomMarshaler.CleanUpNativeData without calling ICustomMarshaler.MarshalManagedToNative.
    // It is trying to free the pointer managed by LibCmo self (for example, it will try to free we got string when getting object name)!
    // So as the compromise, we introduce 2 different marshalers for In / Out string marshaling respectively.
    // BMStringMarshaler for receiving string from BMap (OUT direction), and BMPOwnedStringMarshaler for passing string to BMap (IN direction).
    // The name of marshaler for string array marshaling also following this pattern.

    public class BMStringMarshaler : ICustomMarshaler {
        private static readonly BMStringMarshaler INSTANCE = new BMStringMarshaler();

        public static ICustomMarshaler GetInstance(string pstrCookie) {
            return BMStringMarshaler.INSTANCE;
        }

        public IntPtr MarshalManagedToNative(object ManagedObj) {
            // For OUT direction, we do not convert any managed data into native data.
            // Return nullptr instead.
            return IntPtr.Zero;
        }

        public object MarshalNativeToManaged(IntPtr pNativeData) {
            // Check nullptr
            if (pNativeData == IntPtr.Zero) return null;
            // Call self
            return BMStringMarshaler.ToManaged(pNativeData);
        }

        public void CleanUpNativeData(IntPtr pNativeData) {
            // For OUT direction, we do not convert any managed data into native data.
            // Do nothing here.
        }

        public void CleanUpManagedData(object ManagedObj) {
            // Managed data will be cleaned by C# GC.
            // So we do nothing here.
        }

        public int GetNativeDataSize() {
            // Return -1 to indicate the size of the native data to be marshaled is variable.
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

    public class BMOwnedStringMarshaler : ICustomMarshaler {
        private static readonly BMOwnedStringMarshaler INSTANCE = new BMOwnedStringMarshaler();

        public static ICustomMarshaler GetInstance(string pstrCookie) {
            return BMOwnedStringMarshaler.INSTANCE;
        }

        public IntPtr MarshalManagedToNative(object ManagedObj) {
            // Check requirements.
            if (ManagedObj is null) return IntPtr.Zero;
            string castManagedObj = ManagedObj as string;
            if (castManagedObj is null)
                throw new MarshalDirectiveException("BMStringMarshaler must be used on a string.");
            // Call self
            return BMOwnedStringMarshaler.ToNative(castManagedObj);
        }

        public object MarshalNativeToManaged(IntPtr pNativeData) {
            // For IN direction, we do not convert any native data into managed data.
            // Return null instead.
            return null;
        }

        public void CleanUpNativeData(IntPtr pNativeData) {
            // Check nullptr
            if (pNativeData == IntPtr.Zero) return;
            // Free native pointer
            Marshal.FreeHGlobal(pNativeData);
        }

        public void CleanUpManagedData(object ManagedObj) {
            // For IN direction, we do not convert any native data into managed data.
            // Do nothing here.
        }

        public int GetNativeDataSize() {
            // Return -1 to indicate the size of the native data to be marshaled is variable.
            return -1;
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
    }

    // YYC MARK:
    // For respecting the standard of BMap, 
    // the native memory we created for string array is a simple array and each item is a pointer to a NULL-terminated UTF8 string.
    // Please note the array self is also NULL-terminated otherwise we don't know its length.

    public class BMStringArrayMarshaler : ICustomMarshaler {
        private static readonly BMStringArrayMarshaler INSTANCE = new BMStringArrayMarshaler();

        public static ICustomMarshaler GetInstance(string pstrCookie) {
            return BMStringArrayMarshaler.INSTANCE;
        }

        public IntPtr MarshalManagedToNative(object ManagedObj) {
            // For OUT direction, we do not convert any managed data into native data.
            // Return nullptr instead.
            return IntPtr.Zero;
        }

        public object MarshalNativeToManaged(IntPtr pNativeData) {
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
            // For OUT direction, we do not convert any managed data into native data.
            // Do nothing here.
        }

        public void CleanUpManagedData(object ManagedObj) {
            // Managed data will be cleaned by C# GC.
            // So we do nothing here.
        }

        public int GetNativeDataSize() {
            // Return -1 to indicate the size of the native data to be marshaled is variable.
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

    public class BMOwnedStringArrayMarshaler : ICustomMarshaler {
        private static readonly BMOwnedStringArrayMarshaler INSTANCE = new BMOwnedStringArrayMarshaler();

        public static ICustomMarshaler GetInstance(string pstrCookie) {
            return BMOwnedStringArrayMarshaler.INSTANCE;
        }

        public IntPtr MarshalManagedToNative(object ManagedObj) {
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
                else apString[i] = BMOwnedStringMarshaler.ToNative(stringObj);
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
            // For IN direction, we do not convert any native data into managed data.
            // Return null instead.
            return null;
        }

        public void CleanUpNativeData(IntPtr pNativeData) {
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
            // For IN direction, we do not convert any native data into managed data.
            // Do nothing here.
        }

        public int GetNativeDataSize() {
            // Return -1 to indicate the size of the native data to be marshaled is variable.
            return -1;
        }
    }

}

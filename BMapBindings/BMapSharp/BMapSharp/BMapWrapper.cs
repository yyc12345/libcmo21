using System;
using System.Reflection.Metadata.Ecma335;
using System.Runtime.InteropServices;
using BMapSharp.VirtoolsTypes;

namespace BMapSharp.BMapWrapper {

    /// <summary>
    /// The guard of native BMap environment.
    /// This class initialize native BMap environment when constructing and free it when destructing.
    /// </summary>
    internal sealed class BMapGuard : SafeHandle {
        private static readonly IntPtr MAGIC_HANDLE = (IntPtr)61;
        internal BMapGuard() : base(Utils.INVALID_PTR, true) {
            BMapException.ThrowIfFailed(BMap.BMInit());
            this.handle = MAGIC_HANDLE;
        }

        public override bool IsInvalid => this.handle == Utils.INVALID_PTR;
        protected override bool ReleaseHandle() {
            return BMap.BMDispose();
        }
    }

    public static class Utils {
        /// <summary>The representation of invalid raw pointer.</summary>
        internal static readonly IntPtr INVALID_PTR = IntPtr.Zero;
        /// <summary>The representation of invalid CK_ID.</summary>
        internal static readonly uint INVALID_CKID = 0u;
        /// <summary>
        /// The function used as callback for BMap.
        /// It just writes the data in console.
        /// </summary>
        internal static void BMapSharpCallback(string msg) {
            Console.WriteLine(msg);
        }

        private static readonly BMapGuard Singleton = new BMapGuard();
        public static bool IsBMapAvailable() {
            return !Singleton.IsInvalid;
        }
    }

    public abstract class AbstractPointer : SafeHandle {
        internal AbstractPointer(IntPtr raw_pointer) : base(Utils.INVALID_PTR, true) {
            this.handle = raw_pointer;
        }

        public override bool IsInvalid => this.handle == Utils.INVALID_PTR;
        protected override bool ReleaseHandle() => throw new NotImplementedException();

        protected bool isValid() => this.handle != Utils.INVALID_PTR;
        protected IntPtr getPointer() => this.handle;

        // protected AbstractPointer(IntPtr raw_pointer) : base(raw_pointer, true) {}

        // protected IntPtr GetPointer() => this.handle;
        // public override bool IsInvalid { get { return this.handle == Utils.INVALID_PTR; } }

        // #region IComparable

        // public int CompareTo(AbstractPointer other) {
        //     return m_RawPointer.CompareTo(other.m_RawPointer);
        // }

        // #endregion

        // #region IEquatable

        // public override bool Equals(object obj) => this.Equals(obj as AbstractPointer);
        // public bool Equals(AbstractPointer other) {
        //     if (other is null) return false;
        //     if (Object.ReferenceEquals(this, other)) return true;
        //     // if (this.GetType() != other.GetType()) return false;
        //     return this.m_RawPointer == other.m_RawPointer;
        // }

        // public static bool operator ==(AbstractPointer lhs, AbstractPointer rhs) {
        //     if (lhs is null) {
        //         if (rhs is null) return true;
        //         return false;
        //     }
        //     return lhs.Equals(rhs);
        // }
        // public static bool operator !=(AbstractPointer lhs, AbstractPointer rhs) => !(lhs == rhs);

        // #endregion

        #region Misc

        public override int GetHashCode() => this.handle.GetHashCode();
        public override string ToString() => this.handle.ToString();

        #endregion
    }

    public abstract class AbstractCKObject {
        internal AbstractCKObject(IntPtr raw_pointer, uint ckid) {
            m_RawPointer = raw_pointer;
            m_CKID = ckid;
        }

        private readonly IntPtr m_RawPointer;
        private readonly uint m_CKID;
        protected bool isValid() => m_RawPointer != Utils.INVALID_PTR && m_RawPointer != Utils.INVALID_CKID;
        protected IntPtr getPointer() => m_RawPointer;
        protected uint getCKID() => m_CKID;

        // private uint m_CKID;

        // protected AbstractCKObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer) {
        //     m_CKID = ckid;
        // }

        // protected override bool IsValid() => base.IsValid() && m_CKID != Utils.INVALID_CKID;
        // protected uint GetCKID() => m_CKID;

        // #region IComparable

        // public int CompareTo(AbstractCKObject other) {
        //     var ret = base.CompareTo((AbstractPointer)other);
        //     if (ret != 0) return ret;
        //     return m_CKID.CompareTo(other.m_CKID);
        // }

        // #endregion

        // #region IEquatable

        // public override bool Equals(object obj) => this.Equals(obj as AbstractCKObject);
        // public bool Equals(AbstractCKObject other) {
        //     if (other is null) return false;
        //     if (Object.ReferenceEquals(this, other)) return true;

        // }

        // public static bool operator ==(AbstractCKObject left, AbstractCKObject right) =>
        //     ((AbstractPointer)left == (AbstractPointer)right) && left.m_CKID == right.m_CKID;
        // public static bool operator !=(AbstractCKObject left, AbstractCKObject right) =>
        //     ((AbstractPointer)left != (AbstractPointer)right) || left.m_CKID != right.m_CKID;

        // #endregion

        #region Misc

        public override int GetHashCode() => HashCode.Combine(m_RawPointer, m_CKID);
        public override string ToString() => $"{m_RawPointer}, {m_CKID}";

        #endregion
    }

    public class BMObject : AbstractCKObject {
        internal BMObject(nint raw_pointer, uint ckid) : base(raw_pointer, ckid) {}

        public string GetName() {
            BMapException.ThrowIfFailed(BMap.BMObject_GetName(
                this.getPointer(), this.getCKID(), out string out_name
            ));
            return out_name;
        }
        public void SetName(string name) {
            BMapException.ThrowIfFailed(BMap.BMObject_SetName(
                this.getPointer(), this.getCKID(), name
            ));
        }
    }

    public class BMTexture : BMObject {
        internal BMTexture(nint raw_pointer, uint ckid) : base(raw_pointer, ckid) {}
    }

    public class BMMaterial : BMObject {
        internal BMMaterial(nint raw_pointer, uint ckid) : base(raw_pointer, ckid) {}
    }

    public class BMMesh : BMObject {
        internal BMMesh(nint raw_pointer, uint ckid) : base(raw_pointer, ckid) {}
    }

    public class BM3dObject : BMObject {
        internal BM3dObject(nint raw_pointer, uint ckid) : base(raw_pointer, ckid) {}
    }

    public class BMGroup : BMObject {
        internal BMGroup(nint raw_pointer, uint ckid) : base(raw_pointer, ckid) {}
    }

    public sealed class BMFileReader : AbstractPointer {
        private static IntPtr AllocateHandle(string file_name, string temp_folder, string texture_folder, string[] encodings) {
            BMapException.ThrowIfFailed(BMap.BMFile_Load(
                file_name, temp_folder, texture_folder, 
                Utils.BMapSharpCallback, 
                (uint)encodings.Length, encodings,
                out IntPtr out_file
            ));
            return out_file;
        }
        protected override bool ReleaseHandle() {
            return BMap.BMFile_Free(this.handle);
        }
        public BMFileReader(string file_name, string temp_folder, string texture_folder, string[] encodings)
        : base(AllocateHandle(file_name, temp_folder, texture_folder, encodings)) {}

        public uint GetTextureCount() {
            BMapException.ThrowIfFailed(BMap.BMFile_GetTextureCount(this.getPointer(), out uint out_count));
            return out_count;
        }
        public IEnumerable<BMTexture> GetTextures() {
            uint count = GetTextureCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMFile_GetTexture(this.getPointer(), i, out uint out_id));
                yield return new BMTexture(this.getPointer(), out_id);
            }
        }

        public uint GetMaterialCount() {
            BMapException.ThrowIfFailed(BMap.BMFile_GetMaterialCount(this.getPointer(), out uint out_count));
            return out_count;
        }
        public IEnumerable<BMMaterial> GetMaterials() {
            uint count = GetMaterialCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMFile_GetMaterial(this.getPointer(), i, out uint out_id));
                yield return new BMMaterial(this.getPointer(), out_id);
            }
        }

        public uint GetMeshCount() {
            BMapException.ThrowIfFailed(BMap.BMFile_GetMeshCount(this.getPointer(), out uint out_count));
            return out_count;
        }
        public IEnumerable<BMMesh> GetMeshes() {
            uint count = GetMeshCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMFile_GetMesh(this.getPointer(), i, out uint out_id));
                yield return new BMMesh(this.getPointer(), out_id);
            }
        }

        public uint Get3dObjectCount() {
            BMapException.ThrowIfFailed(BMap.BMFile_Get3dObjectCount(this.getPointer(), out uint out_count));
            return out_count;
        }
        public IEnumerable<BM3dObject> Get3dObjects() {
            uint count = Get3dObjectCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMFile_Get3dObject(this.getPointer(), i, out uint out_id));
                yield return new BM3dObject(this.getPointer(), out_id);
            }
        }

        public uint GetGroupCount() {
            BMapException.ThrowIfFailed(BMap.BMFile_GetGroupCount(this.getPointer(), out uint out_count));
            return out_count;
        }
        public IEnumerable<BMGroup> GetGroups() {
            uint count = GetGroupCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMFile_GetGroup(this.getPointer(), i, out uint out_id));
                yield return new BMGroup(this.getPointer(), out_id);
            }
        }

    }

}

using System;
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

        #region Constant Values

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

        #endregion

        #region Help Functions

        private static void StructAssigner<T>(IntPtr pstruct, uint count, IEnumerable<T> iem) {
            var stride = Marshal.SizeOf<T>();
            var itor = iem.GetEnumerator();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(itor.MoveNext());
                Marshal.StructureToPtr<T>(itor.Current, pstruct, false);
                pstruct += stride;
            }
        }
        internal static void VxVector3Assigner(IntPtr pstruct, uint count, IEnumerable<VxVector3> iem)
            => StructAssigner<VxVector3>(pstruct, count, iem);
        internal static void VxVector2Assigner(IntPtr pstruct, uint count, IEnumerable<VxVector2> iem)
            => StructAssigner<VxVector2>(pstruct, count, iem);
        internal static void CKFaceIndicesAssigner(IntPtr pstruct, uint count, IEnumerable<CKFaceIndices> iem)
            => StructAssigner<CKFaceIndices>(pstruct, count, iem);
        internal static void CKShortFaceIndicesAssigner(IntPtr pstruct, uint count, IEnumerable<CKShortFaceIndices> iem)
            => StructAssigner<CKShortFaceIndices>(pstruct, count, iem); // TODO: There is a padding bug!!!
        
        private static IEnumerable<T> StructIterator<T>(IntPtr pstruct, uint count) {
            var stride = Marshal.SizeOf<T>();
            for (uint i = 0; i < count; ++i) {
                yield return Marshal.PtrToStructure<T>(pstruct);
                pstruct += stride;
            }
        }
        internal static IEnumerable<VxVector3> VxVector3Iterator(IntPtr pstruct, uint count)
            => StructIterator<VxVector3>(pstruct, count);
        internal static IEnumerable<VxVector2> VxVector2Iterator(IntPtr pstruct, uint count)
            => StructIterator<VxVector2>(pstruct, count);
        internal static IEnumerable<CKFaceIndices> CKFaceIndicesIterator(IntPtr pstruct, uint count)
            => StructIterator<CKFaceIndices>(pstruct, count);
        internal static IEnumerable<CKShortFaceIndices> CKShortFaceIndicesIterator(IntPtr pstruct, uint count)
            => StructIterator<CKShortFaceIndices>(pstruct, count); // TODO: There is a padding bug!!!

        #endregion

        #region End User Exposed

        private static readonly BMapGuard Singleton = new BMapGuard();
        public static bool IsBMapAvailable() {
            return !Singleton.IsInvalid;
        }

        #endregion
    }

    public abstract class AbstractPointer : SafeHandle {
        internal AbstractPointer(IntPtr raw_pointer) : base(Utils.INVALID_PTR, true) {
            this.handle = raw_pointer;
        }

        public override bool IsInvalid => this.handle == Utils.INVALID_PTR;
        protected override bool ReleaseHandle() => throw new NotImplementedException();

        internal bool isValid() => this.handle != Utils.INVALID_PTR;
        internal IntPtr getPointer() => this.handle;

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

    public abstract class AbstractCKObject : SafeHandle {
        // Same as AbstractPointer, but not own this handle.
        internal AbstractCKObject(IntPtr raw_pointer, uint ckid) : base(Utils.INVALID_PTR, false) {
            this.handle = raw_pointer;
            m_CKID = ckid;
        }

        public override bool IsInvalid => this.handle == Utils.INVALID_PTR;
        protected override bool ReleaseHandle() => throw new NotImplementedException();

        private readonly uint m_CKID;
        internal bool isValid() => this.handle != Utils.INVALID_PTR && m_CKID != Utils.INVALID_CKID;
        internal IntPtr getPointer() => this.handle;
        internal uint getCKID() => m_CKID;

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

        public override int GetHashCode() => HashCode.Combine(this.handle, m_CKID);
        public override string ToString() => $"{this.handle}, {m_CKID}";

        #endregion
    }

    public class BMObject : AbstractCKObject {
        internal BMObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public string GetName() {
            BMapException.ThrowIfFailed(BMap.BMObject_GetName(getPointer(), getCKID(), out string out_name));
            return out_name;
        }
        public void SetName(string name) {
            BMapException.ThrowIfFailed(BMap.BMObject_SetName(getPointer(), getCKID(), name));
        }
    }

    public class BMTexture : BMObject {
        internal BMTexture(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }
    }

    public class BMMaterial : BMObject {
        internal BMMaterial(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }
    }

    public class BMMesh : BMObject {
        internal BMMesh(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }
    }

    public class BM3dObject : BMObject {
        internal BM3dObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VxMatrix GetWorldMatrix() {
            BMapException.ThrowIfFailed(BMap.BM3dObject_GetWorldMatrix(getPointer(), getCKID(), out VxMatrix out_mat));
            return out_mat;
        }

        public void SetWorldMatrix(VxMatrix mat) {
            BMapException.ThrowIfFailed(BMap.BM3dObject_SetWorldMatrix(getPointer(), getCKID(), mat));
        }

        public BMMesh GetCurrentMesh() {
            BMapException.ThrowIfFailed(BMap.BM3dObject_GetCurrentMesh(getPointer(), getCKID(), out uint out_meshid));
            if (out_meshid == Utils.INVALID_CKID) return null;
            else return new BMMesh(getPointer(), out_meshid);
        }

        public void SetCurrentMesh(BMMesh mesh) {
            uint meshid = (mesh is null) ? Utils.INVALID_CKID : mesh.getCKID();
            BMapException.ThrowIfFailed(BMap.BM3dObject_SetCurrentMesh(getPointer(), getCKID(), meshid));
        }

        public bool GetVisibility() {
            BMapException.ThrowIfFailed(BMap.BM3dObject_GetVisibility(getPointer(), getCKID(), out bool out_isVisible));
            return out_isVisible;
        }

        public void SetVisibility(bool visb) {
            BMapException.ThrowIfFailed(BMap.BM3dObject_SetVisibility(getPointer(), getCKID(), visb));
        }
    }

    public class BMGroup : BMObject {
        internal BMGroup(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public void AddObject(BM3dObject member) {
            BMapException.ThrowIfFailed(BMap.BMGroup_AddObject(getPointer(), getCKID(), member.getCKID()));
        }

        public uint GetObjectCount() {
            BMapException.ThrowIfFailed(BMap.BMGroup_GetObjectCount(getPointer(), getCKID(), out uint out_count));
            return out_count;
        }

        public IEnumerable<BM3dObject> GetObjects() {
            var size = GetObjectCount();
            for (uint i = 0; i < size; ++i) {
                BMapException.ThrowIfFailed(BMap.BMGroup_GetObject(getPointer(), getCKID(), i, out uint out_objid));
                yield return new BM3dObject(getPointer(), out_objid);
            }
        }
    }

    public sealed class BMFileReader : AbstractPointer {
        private static IntPtr allocateHandle(string file_name, string temp_folder, string texture_folder, string[] encodings) {
            BMapException.ThrowIfFailed(BMap.BMFile_Load(
                file_name, temp_folder, texture_folder,
                Utils.BMapSharpCallback,
                (uint)encodings.Length, encodings,
                out IntPtr out_file
            ));
            return out_file;
        }
        protected override bool ReleaseHandle() {
            return BMap.BMFile_Free(this.getPointer());
        }
        public BMFileReader(string file_name, string temp_folder, string texture_folder, string[] encodings)
        : base(allocateHandle(file_name, temp_folder, texture_folder, encodings)) { }

        private delegate bool FctProtoGetCount(IntPtr bmf, out uint cnt);
        private delegate bool FctProtoGetObject(IntPtr bmf, uint idx, out uint id);
        private delegate T FctProtoCreateInstance<T>(IntPtr bmf, uint id);
        private uint getCKObjectCount(FctProtoGetCount fct_cnt) {
            BMapException.ThrowIfFailed(fct_cnt(this.getPointer(), out uint out_count));
            return out_count;
        }
        private IEnumerable<T> getCKObjects<T>(FctProtoGetCount fct_cnt, FctProtoGetObject fct_obj, FctProtoCreateInstance<T> fct_crt) {
            uint count = getCKObjectCount(fct_cnt);
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(fct_obj(this.getPointer(), i, out uint out_id));
                yield return fct_crt(this.getPointer(), out_id);
            }
        }

        public uint GetTextureCount() =>
            getCKObjectCount(BMap.BMFile_GetTextureCount);
        public IEnumerable<BMTexture> GetTextures() =>
            getCKObjects<BMTexture>(BMap.BMFile_GetTextureCount, BMap.BMFile_GetTexture, (bmf, id) => new BMTexture(bmf, id));
        public uint GetMaterialCount() =>
            getCKObjectCount(BMap.BMFile_GetMaterialCount);
        public IEnumerable<BMMaterial> GetMaterials() =>
            getCKObjects<BMMaterial>(BMap.BMFile_GetMaterialCount, BMap.BMFile_GetMaterial, (bmf, id) => new BMMaterial(bmf, id));
        public uint GetMeshCount() =>
            getCKObjectCount(BMap.BMFile_GetMeshCount);
        public IEnumerable<BMMesh> GetMeshes() =>
            getCKObjects<BMMesh>(BMap.BMFile_GetMeshCount, BMap.BMFile_GetMesh, (bmf, id) => new BMMesh(bmf, id));
        public uint Get3dObjectCount() =>
            getCKObjectCount(BMap.BMFile_Get3dObjectCount);
        public IEnumerable<BM3dObject> Get3dObjects() =>
            getCKObjects<BM3dObject>(BMap.BMFile_Get3dObjectCount, BMap.BMFile_Get3dObject, (bmf, id) => new BM3dObject(bmf, id));
        public uint GetGroupCount() =>
            getCKObjectCount(BMap.BMFile_GetGroupCount);
        public IEnumerable<BMGroup> GetGroups() =>
            getCKObjects<BMGroup>(BMap.BMFile_GetGroupCount, BMap.BMFile_GetGroup, (bmf, id) => new BMGroup(bmf, id));

    }

    public sealed class BMFileWriter : AbstractPointer {
        private static IntPtr allocateHandle(string temp_folder, string texture_folder, string[] encodings) {
            BMapException.ThrowIfFailed(BMap.BMFile_Create(
                temp_folder, texture_folder,
                Utils.BMapSharpCallback,
                (uint)encodings.Length, encodings,
                out IntPtr out_file
            ));
            return out_file;
        }
        protected override bool ReleaseHandle() {
            return BMap.BMFile_Free(this.getPointer());
        }
        public BMFileWriter(string temp_folder, string texture_folder, string[] encodings)
        : base(allocateHandle(temp_folder, texture_folder, encodings)) { }

        private delegate bool FctProtoCreateObject(IntPtr bmf, out uint id);
        private delegate T FctProtoCreateInstance<T>(IntPtr bmf, uint id);
        private T createCKObject<T>(FctProtoCreateObject fct_crt, FctProtoCreateInstance<T> fct_inst) {
            BMapException.ThrowIfFailed(fct_crt(this.getPointer(), out uint out_id));
            return fct_inst(this.getPointer(), out_id);
        }

        public BMTexture CreateTexture() => createCKObject<BMTexture>(BMap.BMFile_CreateTexture, (bmf, id) => new BMTexture(bmf, id));
        public BMMaterial CreateMaterial() => createCKObject<BMMaterial>(BMap.BMFile_CreateMaterial, (bmf, id) => new BMMaterial(bmf, id));
        public BMMesh CreateMesh() => createCKObject<BMMesh>(BMap.BMFile_CreateMesh, (bmf, id) => new BMMesh(bmf, id));
        public BM3dObject Create3dObject() => createCKObject<BM3dObject>(BMap.BMFile_Create3dObject, (bmf, id) => new BM3dObject(bmf, id));
        public BMGroup CreateGroup() => createCKObject<BMGroup>(BMap.BMFile_CreateGroup, (bmf, id) => new BMGroup(bmf, id));
    }

    public sealed class BMMeshTrans : AbstractPointer {
        private static IntPtr allocateHandle() {
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_New(out IntPtr out_trans));
            return out_trans;
        }
        protected override bool ReleaseHandle() {
            return BMap.BMMeshTrans_Delete(this.getPointer());
        }
        public BMMeshTrans() : base(allocateHandle()) { }


    }

}

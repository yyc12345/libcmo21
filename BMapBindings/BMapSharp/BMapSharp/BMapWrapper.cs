using System;
using System.Runtime.CompilerServices;
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
            Console.WriteLine($"[BMapSharp] {msg}");
        }

        #endregion

        #region Help Functions

        private static void StructAssigner<T>(IntPtr pstruct, uint count, IEnumerable<T> iem) {
            var stride = Marshal.SizeOf<T>();
            var itor = iem.GetEnumerator();
            for (uint i = 0; i < count; ++i) {
                if (!itor.MoveNext()) throw new BMapException("The length of given data is too short when assigning struct array.");
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
            => StructAssigner<CKShortFaceIndices>(pstruct, count, iem);
        internal static void ShortAssigner(IntPtr pstruct, uint count, IEnumerable<short> iem)
            => StructAssigner<short>(pstruct, count, iem);
        internal static void CKIDAssigner(IntPtr pstruct, uint count, IEnumerable<uint> iem)
            => StructAssigner<uint>(pstruct, count, iem);
        internal static void CKDWORDAssigner(IntPtr pstruct, uint count, IEnumerable<uint> iem)
            => StructAssigner<uint>(pstruct, count, iem);

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
            => StructIterator<CKShortFaceIndices>(pstruct, count);
        internal static IEnumerable<short> ShortIterator(IntPtr pstruct, uint count)
            => StructIterator<short>(pstruct, count);
        internal static IEnumerable<uint> CKIDIterator(IntPtr pstruct, uint count)
            => StructIterator<uint>(pstruct, count);
        internal static IEnumerable<uint> CKDWORDIterator(IntPtr pstruct, uint count)
            => StructIterator<uint>(pstruct, count);

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

        #region Subclass Utilities

        protected delegate bool FctGenericValueGetter<T>(IntPtr bmf, uint id, out T val);
        protected delegate bool FctGenericValueSetter<T>(IntPtr bmf, uint id, T val);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected T getGenericValue<T>(FctGenericValueGetter<T> fct) {
            BMapException.ThrowIfFailed(fct(getPointer(), getCKID(), out T out_val));
            return out_val;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected void setGenericValue<T>(FctGenericValueSetter<T> fct, T val) {
            BMapException.ThrowIfFailed(fct(getPointer(), getCKID(), val));
        }

        #endregion
    }

    public class BMObject : AbstractCKObject {
        internal BMObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public string GetName() => getGenericValue<string>(BMap.BMObject_GetName);
        public void SetName(string name) => setGenericValue<string>(BMap.BMObject_SetName, name);
    }

    public class BMTexture : BMObject {
        internal BMTexture(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public string GetFileName() => getGenericValue<string>(BMap.BMTexture_GetFileName);

        public void LoadImage(string filepath) {
            BMapException.ThrowIfFailed(BMap.BMTexture_LoadImage(getPointer(), getCKID(), filepath));
        }
        public void SaveImage(string filepath) {
            BMapException.ThrowIfFailed(BMap.BMTexture_SaveImage(getPointer(), getCKID(), filepath));
        }

        public CK_TEXTURE_SAVEOPTIONS GetSaveOptions() => getGenericValue<CK_TEXTURE_SAVEOPTIONS>(BMap.BMTexture_GetSaveOptions);
        public void SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opt) => setGenericValue<CK_TEXTURE_SAVEOPTIONS>(BMap.BMTexture_SetSaveOptions, opt);
        public VX_PIXELFORMAT GetVideoFormat() => getGenericValue<VX_PIXELFORMAT>(BMap.BMTexture_GetVideoFormat);
        public void SetVideoFormat(VX_PIXELFORMAT vfmt) => setGenericValue<VX_PIXELFORMAT>(BMap.BMTexture_SetVideoFormat, vfmt);
    }

    public class BMMaterial : BMObject {
        internal BMMaterial(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VxColor GetDiffuse() => getGenericValue<VxColor>(BMap.BMMaterial_GetDiffuse);
        public void SetDiffuse(VxColor col) => setGenericValue<VxColor>(BMap.BMMaterial_SetDiffuse, col);
        public VxColor GetAmbient() => getGenericValue<VxColor>(BMap.BMMaterial_GetAmbient);
        public void SetAmbient(VxColor col) => setGenericValue<VxColor>(BMap.BMMaterial_SetAmbient, col);
        public VxColor GetSpecular() => getGenericValue<VxColor>(BMap.BMMaterial_GetSpecular);
        public void SetSpecular(VxColor col) => setGenericValue<VxColor>(BMap.BMMaterial_SetSpecular, col);
        public VxColor GetEmissive() => getGenericValue<VxColor>(BMap.BMMaterial_GetEmissive);
        public void SetEmissive(VxColor col) => setGenericValue<VxColor>(BMap.BMMaterial_SetEmissive, col);

        public float GetSpecularPower() => getGenericValue<float>(BMap.BMMaterial_GetSpecularPower);
        public void SetSpecularPower(float val) => setGenericValue<float>(BMap.BMMaterial_SetSpecularPower, val);

        public VxColor GetTextureBorderColor() {
            BMapException.ThrowIfFailed(BMap.BMMaterial_GetTextureBorderColor(getPointer(), getCKID(), out uint out_val));
            return new VxColor(out_val);
        }
        public void SetTextureBorderColor(VxColor col) {
            BMapException.ThrowIfFailed(BMap.BMMaterial_SetTextureBorderColor(getPointer(), getCKID(), col.ToDword()));
        }

        public VXTEXTURE_BLENDMODE GetTextureBlendMode() => getGenericValue<VXTEXTURE_BLENDMODE>(BMap.BMMaterial_GetTextureBlendMode);
        public void SetTextureBlendMode(VXTEXTURE_BLENDMODE val) => setGenericValue<VXTEXTURE_BLENDMODE>(BMap.BMMaterial_SetTextureBlendMode, val);
        public VXTEXTURE_FILTERMODE GetTextureMinMode() => getGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_GetTextureMinMode);
        public void SetTextureMinMode(VXTEXTURE_FILTERMODE val) => setGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_SetTextureMinMode, val);
        public VXTEXTURE_FILTERMODE GetTextureMagMode() => getGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_GetTextureMagMode);
        public void SetTextureMagMode(VXTEXTURE_FILTERMODE val) => setGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_SetTextureMagMode, val);
        public VXTEXTURE_ADDRESSMODE GetTextureAddressMode() => getGenericValue<VXTEXTURE_ADDRESSMODE>(BMap.BMMaterial_GetTextureAddressMode);
        public void SetTextureAddressMode(VXTEXTURE_ADDRESSMODE val) => setGenericValue<VXTEXTURE_ADDRESSMODE>(BMap.BMMaterial_SetTextureAddressMode, val);
        public VXBLEND_MODE GetSourceBlend() => getGenericValue<VXBLEND_MODE>(BMap.BMMaterial_GetSourceBlend);
        public void SetSourceBlend(VXBLEND_MODE val) => setGenericValue<VXBLEND_MODE>(BMap.BMMaterial_SetSourceBlend, val);
        public VXBLEND_MODE GetDestBlend() => getGenericValue<VXBLEND_MODE>(BMap.BMMaterial_GetDestBlend);
        public void SetDestBlend(VXBLEND_MODE val) => setGenericValue<VXBLEND_MODE>(BMap.BMMaterial_SetDestBlend, val);
        public VXFILL_MODE GetFillMode() => getGenericValue<VXFILL_MODE>(BMap.BMMaterial_GetFillMode);
        public void SetFillMode(VXFILL_MODE val) => setGenericValue<VXFILL_MODE>(BMap.BMMaterial_SetFillMode, val);
        public VXSHADE_MODE GetShadeMode() => getGenericValue<VXSHADE_MODE>(BMap.BMMaterial_GetShadeMode);
        public void SetShadeMode(VXSHADE_MODE val) => setGenericValue<VXSHADE_MODE>(BMap.BMMaterial_SetShadeMode, val);

        public bool GetAlphaTestEnabled() => getGenericValue<bool>(BMap.BMMaterial_GetAlphaTestEnabled);
        public void SetAlphaTestEnabled(bool val) => setGenericValue<bool>(BMap.BMMaterial_SetAlphaTestEnabled, val);
        public bool GetAlphaBlendEnabled() => getGenericValue<bool>(BMap.BMMaterial_GetAlphaBlendEnabled);
        public void SetAlphaBlendEnabled(bool val) => setGenericValue<bool>(BMap.BMMaterial_SetAlphaBlendEnabled, val);
        public bool GetPerspectiveCorrectionEnabled() => getGenericValue<bool>(BMap.BMMaterial_GetPerspectiveCorrectionEnabled);
        public void SetPerspectiveCorrectionEnabled(bool val) => setGenericValue<bool>(BMap.BMMaterial_SetPerspectiveCorrectionEnabled, val);
        public bool GetZWriteEnabled() => getGenericValue<bool>(BMap.BMMaterial_GetZWriteEnabled);
        public void SetZWriteEnabled(bool val) => setGenericValue<bool>(BMap.BMMaterial_SetZWriteEnabled, val);
        public bool GetTwoSidedEnabled() => getGenericValue<bool>(BMap.BMMaterial_GetTwoSidedEnabled);
        public void SetTwoSidedEnabled(bool val) => setGenericValue<bool>(BMap.BMMaterial_SetTwoSidedEnabled, val);

        public byte GetAlphaRef() => getGenericValue<byte>(BMap.BMMaterial_GetAlphaRef);
        public void SetAlphaRef(byte val) => setGenericValue<byte>(BMap.BMMaterial_SetAlphaRef, val);

        public VXCMPFUNC GetAlphaFunc() => getGenericValue<VXCMPFUNC>(BMap.BMMaterial_GetAlphaFunc);
        public void SetAlphaFunc(VXCMPFUNC val) => setGenericValue<VXCMPFUNC>(BMap.BMMaterial_SetAlphaFunc, val);
        public VXCMPFUNC GetZFunc() => getGenericValue<VXCMPFUNC>(BMap.BMMaterial_GetZFunc);
        public void SetZFunc(VXCMPFUNC val) => setGenericValue<VXCMPFUNC>(BMap.BMMaterial_SetZFunc, val);

    }

    public class BMMesh : BMObject {
        internal BMMesh(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VXMESH_LITMODE GetLitMode() => getGenericValue<VXMESH_LITMODE>(BMap.BMMesh_GetLitMode);
        public void SetLitMode(VXMESH_LITMODE mode) => setGenericValue<VXMESH_LITMODE>(BMap.BMMesh_SetLitMode, mode);

        public uint GetVertexCount() => getGenericValue<uint>(BMap.BMMesh_GetVertexCount);
        public void SetVertexCount(uint count) => setGenericValue<uint>(BMap.BMMesh_SetVertexCount, count);
        public IEnumerable<VxVector3> GetVertexPositions() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexPositions(getPointer(), getCKID(), out IntPtr out_mem));
            return Utils.VxVector3Iterator(out_mem, GetVertexCount());
        }
        public void SetVertexPositions(IEnumerable<VxVector3> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexPositions(getPointer(), getCKID(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, GetVertexCount(), iem);
        }
        public IEnumerable<VxVector3> GetVertexNormals() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexNormals(getPointer(), getCKID(), out IntPtr out_mem));
            return Utils.VxVector3Iterator(out_mem, GetVertexCount());
        }
        public void SetVertexNormals(IEnumerable<VxVector3> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexNormals(getPointer(), getCKID(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, GetVertexCount(), iem);
        }
        public IEnumerable<VxVector2> GetVertexUVs() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexUVs(getPointer(), getCKID(), out IntPtr out_mem));
            return Utils.VxVector2Iterator(out_mem, GetVertexCount());
        }
        public void SetVertexUVs(IEnumerable<VxVector2> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexUVs(getPointer(), getCKID(), out IntPtr out_mem));
            Utils.VxVector2Assigner(out_mem, GetVertexCount(), iem);
        }

        public uint GetFaceCount() => getGenericValue<uint>(BMap.BMMesh_GetFaceCount);
        public void SetFaceCount(uint count) => setGenericValue<uint>(BMap.BMMesh_SetFaceCount, count);
        public IEnumerable<CKShortFaceIndices> GetFaceIndices() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceIndices(getPointer(), getCKID(), out IntPtr out_mem));
            return Utils.CKShortFaceIndicesIterator(out_mem, GetFaceCount());
        }
        public void SetFaceIndices(IEnumerable<CKShortFaceIndices> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceIndices(getPointer(), getCKID(), out IntPtr out_mem));
            Utils.CKShortFaceIndicesAssigner(out_mem, GetFaceCount(), iem);
        }
        public IEnumerable<short> GetFaceMaterialSlotIndexs() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceMaterialSlotIndexs(getPointer(), getCKID(), out IntPtr out_mem));
            return Utils.ShortIterator(out_mem, GetFaceCount());
        }
        public void SetFaceMaterialSlotIndexs(IEnumerable<short> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceMaterialSlotIndexs(getPointer(), getCKID(), out IntPtr out_mem));
            Utils.ShortAssigner(out_mem, GetFaceCount(), iem);
        }
        
        public uint GetMaterialSlotCount() => getGenericValue<uint>(BMap.BMMesh_GetMaterialSlotCount);
        public void SetMaterialSlotCount(uint count) => setGenericValue<uint>(BMap.BMMesh_SetMaterialSlotCount, count);
        public IEnumerable<BMMaterial> GetMaterialSlots() {
            uint count = GetMaterialSlotCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMMesh_GetMaterialSlot(getPointer(), getCKID(), i, out uint out_mtlid));
                if (out_mtlid == Utils.INVALID_CKID) yield return null;
                else yield return new BMMaterial(getPointer(), out_mtlid);
            }
        }
        public void SetMaterialSlots(IEnumerable<BMMaterial> iem) {
            uint count = GetMaterialSlotCount();
            var itor = iem.GetEnumerator();
            for (uint i = 0; i < count; ++i) {
                if (!itor.MoveNext()) throw new BMapException("The length of given material array is too short when assigning material slots.");
                uint mtlid = itor.Current is null ? Utils.INVALID_CKID : itor.Current.getCKID();
                BMapException.ThrowIfFailed(BMap.BMMesh_SetMaterialSlot(getPointer(), getCKID(), i, mtlid));
            }
        }

    }

    public class BM3dObject : BMObject {
        internal BM3dObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VxMatrix GetWorldMatrix() => getGenericValue<VxMatrix>(BMap.BM3dObject_GetWorldMatrix);
        public void SetWorldMatrix(VxMatrix mat) => setGenericValue<VxMatrix>(BMap.BM3dObject_SetWorldMatrix, mat);

        public BMMesh GetCurrentMesh() {
            BMapException.ThrowIfFailed(BMap.BM3dObject_GetCurrentMesh(getPointer(), getCKID(), out uint out_meshid));
            if (out_meshid == Utils.INVALID_CKID) return null;
            else return new BMMesh(getPointer(), out_meshid);
        }
        public void SetCurrentMesh(BMMesh mesh) {
            uint meshid = (mesh is null) ? Utils.INVALID_CKID : mesh.getCKID();
            BMapException.ThrowIfFailed(BMap.BM3dObject_SetCurrentMesh(getPointer(), getCKID(), meshid));
        }

        public bool GetVisibility() => getGenericValue<bool>(BMap.BM3dObject_GetVisibility);
        public void SetVisibility(bool visb) => setGenericValue<bool>(BMap.BM3dObject_SetVisibility, visb);
    }

    public class BMGroup : BMObject {
        internal BMGroup(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public void AddObject(BM3dObject member) {
            BMapException.ThrowIfFailed(BMap.BMGroup_AddObject(getPointer(), getCKID(), member.getCKID()));
        }

        public uint GetObjectCount() => getGenericValue<uint>(BMap.BMGroup_GetObjectCount);
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

        public void Save(string filename, CK_TEXTURE_SAVEOPTIONS texture_save_opt, bool use_compress, int compress_level) {
            BMapException.ThrowIfFailed(BMap.BMFile_Save(
                getPointer(),
                filename,
                texture_save_opt,
                use_compress,
                compress_level
            ));
        }

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

        public void Parse(BMMesh objmesh) {
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_Parse(
                getPointer(),
                objmesh.getPointer(),
                objmesh.getCKID()
            ));
        }

        public void PrepareVertex(uint count, IEnumerable<VxVector3> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareVertexCount(getPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareVertex(getPointer(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, count, iem);
        }
        public void PrepareNormal(uint count, IEnumerable<VxVector3> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareNormalCount(getPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareNormal(getPointer(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, count, iem);
        }
        public void PrepareUV(uint count, IEnumerable<VxVector2> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareUVCount(getPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareUV(getPointer(), out IntPtr out_mem));
            Utils.VxVector2Assigner(out_mem, count, iem);
        }

        public void PrepareMtlSlot(uint count, IEnumerable<BMMaterial> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareMtlSlotCount(getPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareMtlSlot(getPointer(), out IntPtr out_mem));
            var cast_iem = iem.Select((mtl) => mtl is null ? Utils.INVALID_CKID : mtl.getCKID());
            Utils.CKIDAssigner(out_mem, count, cast_iem);
        }

        public void PrepareFace(uint count, IEnumerable<CKFaceIndices> vec_idx, IEnumerable<CKFaceIndices> nml_idx, IEnumerable<CKFaceIndices> uv_idx, IEnumerable<uint> mtl_idx) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceCount(getPointer(), count));
            // Get data address
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceVertexIndices(getPointer(), out IntPtr raw_vec_idx));
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceNormalIndices(getPointer(), out IntPtr raw_nml_idx));
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceUVIndices(getPointer(), out IntPtr raw_uv_idx));
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceMtlSlot(getPointer(), out IntPtr raw_mtl_idx));
            // Assign data
            Utils.CKFaceIndicesAssigner(raw_vec_idx, count, vec_idx);
            Utils.CKFaceIndicesAssigner(raw_nml_idx, count, nml_idx);
            Utils.CKFaceIndicesAssigner(raw_uv_idx, count, uv_idx);
            Utils.CKDWORDAssigner(raw_mtl_idx, count, mtl_idx);
        }

    }

}

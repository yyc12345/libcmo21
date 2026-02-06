using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using BMapSharp.VirtoolsTypes;

namespace BMapSharp.BMapWrapper {

    /// <summary>
    /// BMapSharp module specific exception.
    /// </summary>
    public class BMapException : Exception {
        public BMapException() { }
        public BMapException(string message)
            : base(message) { }
        public BMapException(string message, Exception inner)
            : base(message, inner) { }
        public static void ThrowIfFailed(bool condition) {
            if (!condition) throw new BMapException("BMap operation failed.");
        }
    }

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

    // TODO: Maybe I need to implement IEquatable, IComparable<T>, and IComparable for AbstractPointer and AbstractCKObject.
    // But I give it up. I am lazy. What I have written barely works for me now.

    public abstract class AbstractPointer : SafeHandle, IEquatable<AbstractPointer> {
        internal AbstractPointer(IntPtr raw_pointer) : base(Utils.INVALID_PTR, true) {
            this.handle = raw_pointer;
        }

        public override bool IsInvalid => this.handle == Utils.INVALID_PTR;
        protected override bool ReleaseHandle() => throw new NotImplementedException();

        internal bool IsValid() => this.handle != Utils.INVALID_PTR;
        internal IntPtr GetPointer() => this.handle;

        #region IEquatable

        public override bool Equals(object obj) => this.Equals(obj as AbstractPointer);
        public bool Equals(AbstractPointer obj) {
            if (obj is null) return false;
            // Optimization for a common success case
            if (Object.ReferenceEquals(this, obj)) return true;
            // If run-time types are not exactly the same, return false.
            if (this.GetType() != obj.GetType()) return false;
            // Return true if the fields match.
            return this.handle == obj.handle;
        }

        public override int GetHashCode() => this.handle.GetHashCode();

        public static bool operator ==(AbstractPointer lhs, AbstractPointer rhs) {
            if (lhs is null) {
                if (rhs is null) return true;
                // Only left side is null.
                return false;
            }
            // Equals handles case of null on right side
            return lhs.Equals(rhs);
        }
        public static bool operator !=(AbstractPointer lhs, AbstractPointer rhs) => !(lhs == rhs);

        #endregion

        #region Misc

        public override string ToString() => this.handle.ToString();

        #endregion
    }

    public abstract class AbstractCKObject : SafeHandle, IEquatable<AbstractCKObject> {
        // Same as AbstractPointer, but not own this handle.
        internal AbstractCKObject(IntPtr raw_pointer, uint ckid) : base(Utils.INVALID_PTR, false) {
            this.handle = raw_pointer;
            m_CKID = ckid;
        }

        public override bool IsInvalid => this.handle == Utils.INVALID_PTR;
        protected override bool ReleaseHandle() => throw new NotImplementedException();

        private readonly uint m_CKID;
        internal bool IsValid() => this.handle != Utils.INVALID_PTR && m_CKID != Utils.INVALID_CKID;
        internal IntPtr GetPointer() => this.handle;
        internal uint GetCKID() => m_CKID;

        #region IEquatable

        public override bool Equals(object obj) => this.Equals(obj as AbstractCKObject);
        public bool Equals(AbstractCKObject obj) {
            if (obj is null) return false;
            // Optimization for a common success case
            if (Object.ReferenceEquals(this, obj)) return true;
            // If run-time types are not exactly the same, return false.
            if (this.GetType() != obj.GetType()) return false;
            // Return true if the fields match.
            return (this.m_CKID == obj.m_CKID) && (this.handle == obj.handle);
        }

        public override int GetHashCode() => HashCode.Combine(this.handle, m_CKID);

        public static bool operator ==(AbstractCKObject lhs, AbstractCKObject rhs) {
            if (lhs is null) {
                if (rhs is null) return true;
                // Only left side is null.
                return false;
            }
            // Equals handles case of null on right side
            return lhs.Equals(rhs);
        }
        public static bool operator !=(AbstractCKObject lhs, AbstractCKObject rhs) => !(lhs == rhs);

        #endregion

        #region Misc

        public override string ToString() => $"{this.handle}, {m_CKID}";

        #endregion

        #region Subclass Utilities

        protected delegate bool FctGenericValueGetter<T>(IntPtr bmf, uint id, out T val);
        protected delegate bool FctGenericValueSetter<T>(IntPtr bmf, uint id, T val);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected T GetGenericValue<T>(FctGenericValueGetter<T> fct) {
            BMapException.ThrowIfFailed(fct(GetPointer(), GetCKID(), out T out_val));
            return out_val;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        protected void SetGenericValue<T>(FctGenericValueSetter<T> fct, T val) {
            BMapException.ThrowIfFailed(fct(GetPointer(), GetCKID(), val));
        }

        #endregion
    }

    public class BMObject : AbstractCKObject {
        internal BMObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public string GetName() => GetGenericValue<string>(BMap.BMObject_GetName);
        public void SetName(string name) => SetGenericValue<string>(BMap.BMObject_SetName, name);
    }

    public class BMTexture : BMObject {
        internal BMTexture(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public string GetFileName() => GetGenericValue<string>(BMap.BMTexture_GetFileName);

        public void LoadImage(string filepath) {
            BMapException.ThrowIfFailed(BMap.BMTexture_LoadImage(GetPointer(), GetCKID(), filepath));
        }
        public void SaveImage(string filepath) {
            BMapException.ThrowIfFailed(BMap.BMTexture_SaveImage(GetPointer(), GetCKID(), filepath));
        }

        public CK_TEXTURE_SAVEOPTIONS GetSaveOptions() => GetGenericValue<CK_TEXTURE_SAVEOPTIONS>(BMap.BMTexture_GetSaveOptions);
        public void SetSaveOptions(CK_TEXTURE_SAVEOPTIONS opt) => SetGenericValue<CK_TEXTURE_SAVEOPTIONS>(BMap.BMTexture_SetSaveOptions, opt);
        public VX_PIXELFORMAT GetVideoFormat() => GetGenericValue<VX_PIXELFORMAT>(BMap.BMTexture_GetVideoFormat);
        public void SetVideoFormat(VX_PIXELFORMAT vfmt) => SetGenericValue<VX_PIXELFORMAT>(BMap.BMTexture_SetVideoFormat, vfmt);
    }

    public class BMMaterial : BMObject {
        internal BMMaterial(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VxColor GetDiffuse() => GetGenericValue<VxColor>(BMap.BMMaterial_GetDiffuse);
        public void SetDiffuse(VxColor col) => SetGenericValue<VxColor>(BMap.BMMaterial_SetDiffuse, col);
        public VxColor GetAmbient() => GetGenericValue<VxColor>(BMap.BMMaterial_GetAmbient);
        public void SetAmbient(VxColor col) => SetGenericValue<VxColor>(BMap.BMMaterial_SetAmbient, col);
        public VxColor GetSpecular() => GetGenericValue<VxColor>(BMap.BMMaterial_GetSpecular);
        public void SetSpecular(VxColor col) => SetGenericValue<VxColor>(BMap.BMMaterial_SetSpecular, col);
        public VxColor GetEmissive() => GetGenericValue<VxColor>(BMap.BMMaterial_GetEmissive);
        public void SetEmissive(VxColor col) => SetGenericValue<VxColor>(BMap.BMMaterial_SetEmissive, col);

        public float GetSpecularPower() => GetGenericValue<float>(BMap.BMMaterial_GetSpecularPower);
        public void SetSpecularPower(float val) => SetGenericValue<float>(BMap.BMMaterial_SetSpecularPower, val);

        public VxColor GetTextureBorderColor() {
            BMapException.ThrowIfFailed(BMap.BMMaterial_GetTextureBorderColor(GetPointer(), GetCKID(), out uint out_val));
            return new VxColor(out_val);
        }
        public void SetTextureBorderColor(VxColor col) {
            BMapException.ThrowIfFailed(BMap.BMMaterial_SetTextureBorderColor(GetPointer(), GetCKID(), col.ToDword()));
        }

        public VXTEXTURE_BLENDMODE GetTextureBlendMode() => GetGenericValue<VXTEXTURE_BLENDMODE>(BMap.BMMaterial_GetTextureBlendMode);
        public void SetTextureBlendMode(VXTEXTURE_BLENDMODE val) => SetGenericValue<VXTEXTURE_BLENDMODE>(BMap.BMMaterial_SetTextureBlendMode, val);
        public VXTEXTURE_FILTERMODE GetTextureMinMode() => GetGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_GetTextureMinMode);
        public void SetTextureMinMode(VXTEXTURE_FILTERMODE val) => SetGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_SetTextureMinMode, val);
        public VXTEXTURE_FILTERMODE GetTextureMagMode() => GetGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_GetTextureMagMode);
        public void SetTextureMagMode(VXTEXTURE_FILTERMODE val) => SetGenericValue<VXTEXTURE_FILTERMODE>(BMap.BMMaterial_SetTextureMagMode, val);
        public VXTEXTURE_ADDRESSMODE GetTextureAddressMode() => GetGenericValue<VXTEXTURE_ADDRESSMODE>(BMap.BMMaterial_GetTextureAddressMode);
        public void SetTextureAddressMode(VXTEXTURE_ADDRESSMODE val) => SetGenericValue<VXTEXTURE_ADDRESSMODE>(BMap.BMMaterial_SetTextureAddressMode, val);
        public VXBLEND_MODE GetSourceBlend() => GetGenericValue<VXBLEND_MODE>(BMap.BMMaterial_GetSourceBlend);
        public void SetSourceBlend(VXBLEND_MODE val) => SetGenericValue<VXBLEND_MODE>(BMap.BMMaterial_SetSourceBlend, val);
        public VXBLEND_MODE GetDestBlend() => GetGenericValue<VXBLEND_MODE>(BMap.BMMaterial_GetDestBlend);
        public void SetDestBlend(VXBLEND_MODE val) => SetGenericValue<VXBLEND_MODE>(BMap.BMMaterial_SetDestBlend, val);
        public VXFILL_MODE GetFillMode() => GetGenericValue<VXFILL_MODE>(BMap.BMMaterial_GetFillMode);
        public void SetFillMode(VXFILL_MODE val) => SetGenericValue<VXFILL_MODE>(BMap.BMMaterial_SetFillMode, val);
        public VXSHADE_MODE GetShadeMode() => GetGenericValue<VXSHADE_MODE>(BMap.BMMaterial_GetShadeMode);
        public void SetShadeMode(VXSHADE_MODE val) => SetGenericValue<VXSHADE_MODE>(BMap.BMMaterial_SetShadeMode, val);

        public bool GetAlphaTestEnabled() => GetGenericValue<bool>(BMap.BMMaterial_GetAlphaTestEnabled);
        public void SetAlphaTestEnabled(bool val) => SetGenericValue<bool>(BMap.BMMaterial_SetAlphaTestEnabled, val);
        public bool GetAlphaBlendEnabled() => GetGenericValue<bool>(BMap.BMMaterial_GetAlphaBlendEnabled);
        public void SetAlphaBlendEnabled(bool val) => SetGenericValue<bool>(BMap.BMMaterial_SetAlphaBlendEnabled, val);
        public bool GetPerspectiveCorrectionEnabled() => GetGenericValue<bool>(BMap.BMMaterial_GetPerspectiveCorrectionEnabled);
        public void SetPerspectiveCorrectionEnabled(bool val) => SetGenericValue<bool>(BMap.BMMaterial_SetPerspectiveCorrectionEnabled, val);
        public bool GetZWriteEnabled() => GetGenericValue<bool>(BMap.BMMaterial_GetZWriteEnabled);
        public void SetZWriteEnabled(bool val) => SetGenericValue<bool>(BMap.BMMaterial_SetZWriteEnabled, val);
        public bool GetTwoSidedEnabled() => GetGenericValue<bool>(BMap.BMMaterial_GetTwoSidedEnabled);
        public void SetTwoSidedEnabled(bool val) => SetGenericValue<bool>(BMap.BMMaterial_SetTwoSidedEnabled, val);

        public byte GetAlphaRef() => GetGenericValue<byte>(BMap.BMMaterial_GetAlphaRef);
        public void SetAlphaRef(byte val) => SetGenericValue<byte>(BMap.BMMaterial_SetAlphaRef, val);

        public VXCMPFUNC GetAlphaFunc() => GetGenericValue<VXCMPFUNC>(BMap.BMMaterial_GetAlphaFunc);
        public void SetAlphaFunc(VXCMPFUNC val) => SetGenericValue<VXCMPFUNC>(BMap.BMMaterial_SetAlphaFunc, val);
        public VXCMPFUNC GetZFunc() => GetGenericValue<VXCMPFUNC>(BMap.BMMaterial_GetZFunc);
        public void SetZFunc(VXCMPFUNC val) => SetGenericValue<VXCMPFUNC>(BMap.BMMaterial_SetZFunc, val);

    }

    public class BMMesh : BMObject {
        internal BMMesh(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VXMESH_LITMODE GetLitMode() => GetGenericValue<VXMESH_LITMODE>(BMap.BMMesh_GetLitMode);
        public void SetLitMode(VXMESH_LITMODE mode) => SetGenericValue<VXMESH_LITMODE>(BMap.BMMesh_SetLitMode, mode);

        public uint GetVertexCount() => GetGenericValue<uint>(BMap.BMMesh_GetVertexCount);
        public void SetVertexCount(uint count) => SetGenericValue<uint>(BMap.BMMesh_SetVertexCount, count);
        public IEnumerable<VxVector3> GetVertexPositions() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexPositions(GetPointer(), GetCKID(), out IntPtr out_mem));
            return Utils.VxVector3Iterator(out_mem, GetVertexCount());
        }
        public void SetVertexPositions(IEnumerable<VxVector3> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexPositions(GetPointer(), GetCKID(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, GetVertexCount(), iem);
        }
        public IEnumerable<VxVector3> GetVertexNormals() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexNormals(GetPointer(), GetCKID(), out IntPtr out_mem));
            return Utils.VxVector3Iterator(out_mem, GetVertexCount());
        }
        public void SetVertexNormals(IEnumerable<VxVector3> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexNormals(GetPointer(), GetCKID(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, GetVertexCount(), iem);
        }
        public IEnumerable<VxVector2> GetVertexUVs() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexUVs(GetPointer(), GetCKID(), out IntPtr out_mem));
            return Utils.VxVector2Iterator(out_mem, GetVertexCount());
        }
        public void SetVertexUVs(IEnumerable<VxVector2> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetVertexUVs(GetPointer(), GetCKID(), out IntPtr out_mem));
            Utils.VxVector2Assigner(out_mem, GetVertexCount(), iem);
        }

        public uint GetFaceCount() => GetGenericValue<uint>(BMap.BMMesh_GetFaceCount);
        public void SetFaceCount(uint count) => SetGenericValue<uint>(BMap.BMMesh_SetFaceCount, count);
        public IEnumerable<CKShortFaceIndices> GetFaceIndices() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceIndices(GetPointer(), GetCKID(), out IntPtr out_mem));
            return Utils.CKShortFaceIndicesIterator(out_mem, GetFaceCount());
        }
        public void SetFaceIndices(IEnumerable<CKShortFaceIndices> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceIndices(GetPointer(), GetCKID(), out IntPtr out_mem));
            Utils.CKShortFaceIndicesAssigner(out_mem, GetFaceCount(), iem);
        }
        public IEnumerable<short> GetFaceMaterialSlotIndexs() {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceMaterialSlotIndexs(GetPointer(), GetCKID(), out IntPtr out_mem));
            return Utils.ShortIterator(out_mem, GetFaceCount());
        }
        public void SetFaceMaterialSlotIndexs(IEnumerable<short> iem) {
            BMapException.ThrowIfFailed(BMap.BMMesh_GetFaceMaterialSlotIndexs(GetPointer(), GetCKID(), out IntPtr out_mem));
            Utils.ShortAssigner(out_mem, GetFaceCount(), iem);
        }

        public uint GetMaterialSlotCount() => GetGenericValue<uint>(BMap.BMMesh_GetMaterialSlotCount);
        public void SetMaterialSlotCount(uint count) => SetGenericValue<uint>(BMap.BMMesh_SetMaterialSlotCount, count);
        public IEnumerable<BMMaterial> GetMaterialSlots() {
            uint count = GetMaterialSlotCount();
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(BMap.BMMesh_GetMaterialSlot(GetPointer(), GetCKID(), i, out uint out_mtlid));
                if (out_mtlid == Utils.INVALID_CKID) yield return null;
                else yield return new BMMaterial(GetPointer(), out_mtlid);
            }
        }
        public void SetMaterialSlots(IEnumerable<BMMaterial> iem) {
            uint count = GetMaterialSlotCount();
            var itor = iem.GetEnumerator();
            for (uint i = 0; i < count; ++i) {
                if (!itor.MoveNext()) throw new BMapException("The length of given material array is too short when assigning material slots.");
                uint mtlid = itor.Current is null ? Utils.INVALID_CKID : itor.Current.GetCKID();
                BMapException.ThrowIfFailed(BMap.BMMesh_SetMaterialSlot(GetPointer(), GetCKID(), i, mtlid));
            }
        }

    }

    public class BM3dEntity : BMObject {
        internal BM3dEntity(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public VxMatrix GetWorldMatrix() => GetGenericValue<VxMatrix>(BMap.BM3dEntity_GetWorldMatrix);
        public void SetWorldMatrix(VxMatrix mat) => SetGenericValue<VxMatrix>(BMap.BM3dEntity_SetWorldMatrix, mat);

        public BMMesh GetCurrentMesh() {
            BMapException.ThrowIfFailed(BMap.BM3dEntity_GetCurrentMesh(GetPointer(), GetCKID(), out uint out_meshid));
            if (out_meshid == Utils.INVALID_CKID) return null;
            else return new BMMesh(GetPointer(), out_meshid);
        }
        public void SetCurrentMesh(BMMesh mesh) {
            uint meshid = (mesh is null) ? Utils.INVALID_CKID : mesh.GetCKID();
            BMapException.ThrowIfFailed(BMap.BM3dEntity_SetCurrentMesh(GetPointer(), GetCKID(), meshid));
        }

        public bool GetVisibility() => GetGenericValue<bool>(BMap.BM3dEntity_GetVisibility);
        public void SetVisibility(bool visb) => SetGenericValue<bool>(BMap.BM3dEntity_SetVisibility, visb);
    }

    public class BM3dObject : BM3dEntity {
        internal BM3dObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }
    }

    public class BMLight : BM3dEntity {
        internal BMLight(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        // YYC MARK:
        // Name `GetType` is conflict with C# base class function name.
        // So we add a `Light` prefix for it.
        public VXLIGHT_TYPE GetLightType() => GetGenericValue<VXLIGHT_TYPE>(BMap.BMLight_GetType);
        public void SetLightType(VXLIGHT_TYPE val) => SetGenericValue<VXLIGHT_TYPE>(BMap.BMLight_SetType, val);

        public VxColor GetColor() => GetGenericValue<VxColor>(BMap.BMLight_GetColor);
        public void SetColor(VxColor col) => SetGenericValue<VxColor>(BMap.BMLight_SetColor, col);

        public float GetConstantAttenuation() => GetGenericValue<float>(BMap.BMLight_GetConstantAttenuation);
        public void SetConstantAttenuation(float val) => SetGenericValue<float>(BMap.BMLight_SetConstantAttenuation, val);
        public float GetLinearAttenuation() => GetGenericValue<float>(BMap.BMLight_GetLinearAttenuation);
        public void SetLinearAttenuation(float val) => SetGenericValue<float>(BMap.BMLight_SetLinearAttenuation, val);
        public float GetQuadraticAttenuation() => GetGenericValue<float>(BMap.BMLight_GetQuadraticAttenuation);
        public void SetQuadraticAttenuation(float val) => SetGenericValue<float>(BMap.BMLight_SetQuadraticAttenuation, val);

        public float GetRange() => GetGenericValue<float>(BMap.BMLight_GetRange);
        public void SetRange(float val) => SetGenericValue<float>(BMap.BMLight_SetRange, val);

        public float GetHotSpot() => GetGenericValue<float>(BMap.BMLight_GetHotSpot);
        public void SetHotSpot(float val) => SetGenericValue<float>(BMap.BMLight_SetHotSpot, val);
        public float GetFalloff() => GetGenericValue<float>(BMap.BMLight_GetFalloff);
        public void SetFalloff(float val) => SetGenericValue<float>(BMap.BMLight_SetFalloff, val);
        public float GetFalloffShape() => GetGenericValue<float>(BMap.BMLight_GetFalloffShape);
        public void SetFalloffShape(float val) => SetGenericValue<float>(BMap.BMLight_SetFalloffShape, val);
    }

    public class BMTargetLight : BMLight {
        internal BMTargetLight(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }
    }

    public class BMCamera : BM3dEntity {
        internal BMCamera(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public CK_CAMERA_PROJECTION GetProjectionType() => GetGenericValue<CK_CAMERA_PROJECTION>(BMap.BMCamera_GetProjectionType);
        public void SetProjectionType(CK_CAMERA_PROJECTION val) => SetGenericValue<CK_CAMERA_PROJECTION>(BMap.BMCamera_SetProjectionType, val);

        public float GetOrthographicZoom() => GetGenericValue<float>(BMap.BMCamera_GetOrthographicZoom);
        public void SetOrthographicZoom(float val) => SetGenericValue<float>(BMap.BMCamera_SetOrthographicZoom, val);
        
        public float GetFrontPlane() => GetGenericValue<float>(BMap.BMCamera_GetFrontPlane);
        public void SetFrontPlane(float val) => SetGenericValue<float>(BMap.BMCamera_SetFrontPlane, val);
        public float GetBackPlane() => GetGenericValue<float>(BMap.BMCamera_GetBackPlane);
        public void SetBackPlane(float val) => SetGenericValue<float>(BMap.BMCamera_SetBackPlane, val);
        public float GetFov() => GetGenericValue<float>(BMap.BMCamera_GetFov);
        public void SetFov(float val) => SetGenericValue<float>(BMap.BMCamera_SetFov, val);

        public void GetAspectRatio(out uint width, out uint height) {
            BMapException.ThrowIfFailed(BMap.BMCamera_GetAspectRatio(GetPointer(), GetCKID(), out width, out height));
        }
        public void SetAspectRatio(uint width, uint height) {
            BMapException.ThrowIfFailed(BMap.BMCamera_SetAspectRatio(GetPointer(), GetCKID(), width, height));
        }
    }

    public class BMTargetCamera: BMCamera {
        internal BMTargetCamera(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }
    }

    public class BMGroup : BMObject {
        internal BMGroup(IntPtr raw_pointer, uint ckid) : base(raw_pointer, ckid) { }

        public void AddObject(BM3dObject member) {
            BMapException.ThrowIfFailed(BMap.BMGroup_AddObject(GetPointer(), GetCKID(), member.GetCKID()));
        }

        public uint GetObjectCount() => GetGenericValue<uint>(BMap.BMGroup_GetObjectCount);
        public IEnumerable<BM3dObject> GetObjects() {
            var size = GetObjectCount();
            for (uint i = 0; i < size; ++i) {
                BMapException.ThrowIfFailed(BMap.BMGroup_GetObject(GetPointer(), GetCKID(), i, out uint out_objid));
                yield return new BM3dObject(GetPointer(), out_objid);
            }
        }
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
            return BMap.BMFile_Free(this.GetPointer());
        }
        public BMFileReader(string file_name, string temp_folder, string texture_folder, string[] encodings)
        : base(AllocateHandle(file_name, temp_folder, texture_folder, encodings)) { }

        private delegate bool FctProtoGetCount(IntPtr bmf, out uint cnt);
        private delegate bool FctProtoGetObject(IntPtr bmf, uint idx, out uint id);
        private delegate T FctProtoCreateInstance<T>(IntPtr bmf, uint id);
        private uint GetGenericObjectCount(FctProtoGetCount fct_cnt) {
            BMapException.ThrowIfFailed(fct_cnt(this.GetPointer(), out uint out_count));
            return out_count;
        }
        private IEnumerable<T> GetGenericObject<T>(FctProtoGetCount fct_cnt, FctProtoGetObject fct_obj, FctProtoCreateInstance<T> fct_crt) {
            uint count = GetGenericObjectCount(fct_cnt);
            for (uint i = 0; i < count; ++i) {
                BMapException.ThrowIfFailed(fct_obj(this.GetPointer(), i, out uint out_id));
                yield return fct_crt(this.GetPointer(), out_id);
            }
        }

        public uint GetTextureCount() =>
            GetGenericObjectCount(BMap.BMFile_GetTextureCount);
        public IEnumerable<BMTexture> GetTextures() =>
            GetGenericObject<BMTexture>(BMap.BMFile_GetTextureCount, BMap.BMFile_GetTexture, (bmf, id) => new BMTexture(bmf, id));
        public uint GetMaterialCount() =>
            GetGenericObjectCount(BMap.BMFile_GetMaterialCount);
        public IEnumerable<BMMaterial> GetMaterials() =>
            GetGenericObject<BMMaterial>(BMap.BMFile_GetMaterialCount, BMap.BMFile_GetMaterial, (bmf, id) => new BMMaterial(bmf, id));
        public uint GetMeshCount() =>
            GetGenericObjectCount(BMap.BMFile_GetMeshCount);
        public IEnumerable<BMMesh> GetMeshes() =>
            GetGenericObject<BMMesh>(BMap.BMFile_GetMeshCount, BMap.BMFile_GetMesh, (bmf, id) => new BMMesh(bmf, id));
        public uint Get3dObjectCount() =>
            GetGenericObjectCount(BMap.BMFile_Get3dObjectCount);
        public IEnumerable<BM3dObject> Get3dObjects() =>
            GetGenericObject<BM3dObject>(BMap.BMFile_Get3dObjectCount, BMap.BMFile_Get3dObject, (bmf, id) => new BM3dObject(bmf, id));
        public uint GetGroupCount() =>
            GetGenericObjectCount(BMap.BMFile_GetGroupCount);
        public IEnumerable<BMGroup> GetGroups() =>
            GetGenericObject<BMGroup>(BMap.BMFile_GetGroupCount, BMap.BMFile_GetGroup, (bmf, id) => new BMGroup(bmf, id));
        public uint GetTargetLightCount() =>
            GetGenericObjectCount(BMap.BMFile_GetTargetLightCount);
        public IEnumerable<BMTargetLight> GetTargetLights() =>
            GetGenericObject<BMTargetLight>(BMap.BMFile_GetTargetLightCount, BMap.BMFile_GetTargetLight, (bmf, id) => new BMTargetLight(bmf, id));
        public uint GetTargetCameraCount() =>
            GetGenericObjectCount(BMap.BMFile_GetTargetCameraCount);
        public IEnumerable<BMTargetCamera> GetTargetCameras() =>
            GetGenericObject<BMTargetCamera>(BMap.BMFile_GetTargetCameraCount, BMap.BMFile_GetTargetCamera, (bmf, id) => new BMTargetCamera(bmf, id));

    }

    public sealed class BMFileWriter : AbstractPointer {
        private static IntPtr AllocateHandle(string temp_folder, string texture_folder, string[] encodings) {
            BMapException.ThrowIfFailed(BMap.BMFile_Create(
                temp_folder, texture_folder,
                Utils.BMapSharpCallback,
                (uint)encodings.Length, encodings,
                out IntPtr out_file
            ));
            return out_file;
        }
        protected override bool ReleaseHandle() {
            return BMap.BMFile_Free(this.GetPointer());
        }
        public BMFileWriter(string temp_folder, string texture_folder, string[] encodings)
        : base(AllocateHandle(temp_folder, texture_folder, encodings)) { }

        public void Save(string filename, CK_TEXTURE_SAVEOPTIONS texture_save_opt, bool use_compress, int compress_level) {
            BMapException.ThrowIfFailed(BMap.BMFile_Save(
                GetPointer(),
                filename,
                texture_save_opt,
                use_compress,
                compress_level
            ));
        }

        private delegate bool FctProtoCreateObject(IntPtr bmf, out uint id);
        private delegate T FctProtoCreateInstance<T>(IntPtr bmf, uint id);
        private T CreateGenericObject<T>(FctProtoCreateObject fct_crt, FctProtoCreateInstance<T> fct_inst) {
            BMapException.ThrowIfFailed(fct_crt(this.GetPointer(), out uint out_id));
            return fct_inst(this.GetPointer(), out_id);
        }

        public BMTexture CreateTexture() => CreateGenericObject<BMTexture>(BMap.BMFile_CreateTexture, (bmf, id) => new BMTexture(bmf, id));
        public BMMaterial CreateMaterial() => CreateGenericObject<BMMaterial>(BMap.BMFile_CreateMaterial, (bmf, id) => new BMMaterial(bmf, id));
        public BMMesh CreateMesh() => CreateGenericObject<BMMesh>(BMap.BMFile_CreateMesh, (bmf, id) => new BMMesh(bmf, id));
        public BM3dObject Create3dObject() => CreateGenericObject<BM3dObject>(BMap.BMFile_Create3dObject, (bmf, id) => new BM3dObject(bmf, id));
        public BMGroup CreateGroup() => CreateGenericObject<BMGroup>(BMap.BMFile_CreateGroup, (bmf, id) => new BMGroup(bmf, id));
        public BMTargetLight CreateTargetLight() => CreateGenericObject<BMTargetLight>(BMap.BMFile_CreateTargetLight, (bmf, id) => new BMTargetLight(bmf, id));
        public BMTargetCamera CreateTargetCamera() => CreateGenericObject<BMTargetCamera>(BMap.BMFile_CreateTargetCamera, (bmf, id) => new BMTargetCamera(bmf, id));
        
    }

    public sealed class BMMeshTrans : AbstractPointer {
        private static IntPtr AllocateHandle() {
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_New(out IntPtr out_trans));
            return out_trans;
        }
        protected override bool ReleaseHandle() {
            return BMap.BMMeshTrans_Delete(this.GetPointer());
        }
        public BMMeshTrans() : base(AllocateHandle()) { }

        public void Parse(BMMesh objmesh) {
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_Parse(
                GetPointer(),
                objmesh.GetPointer(),
                objmesh.GetCKID()
            ));
        }

        public void PrepareVertex(uint count, IEnumerable<VxVector3> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareVertexCount(GetPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareVertex(GetPointer(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, count, iem);
        }
        public void PrepareNormal(uint count, IEnumerable<VxVector3> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareNormalCount(GetPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareNormal(GetPointer(), out IntPtr out_mem));
            Utils.VxVector3Assigner(out_mem, count, iem);
        }
        public void PrepareUV(uint count, IEnumerable<VxVector2> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareUVCount(GetPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareUV(GetPointer(), out IntPtr out_mem));
            Utils.VxVector2Assigner(out_mem, count, iem);
        }

        public void PrepareMtlSlot(uint count, IEnumerable<BMMaterial> iem) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareMtlSlotCount(GetPointer(), count));
            // Then put data
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareMtlSlot(GetPointer(), out IntPtr out_mem));
            var cast_iem = iem.Select((mtl) => mtl is null ? Utils.INVALID_CKID : mtl.GetCKID());
            Utils.CKIDAssigner(out_mem, count, cast_iem);
        }

        public void PrepareFace(uint count, IEnumerable<CKFaceIndices> vec_idx, IEnumerable<CKFaceIndices> nml_idx, IEnumerable<CKFaceIndices> uv_idx, IEnumerable<uint> mtl_idx) {
            // Prepare count first
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceCount(GetPointer(), count));
            // Get data address
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceVertexIndices(GetPointer(), out IntPtr raw_vec_idx));
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceNormalIndices(GetPointer(), out IntPtr raw_nml_idx));
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceUVIndices(GetPointer(), out IntPtr raw_uv_idx));
            BMapException.ThrowIfFailed(BMap.BMMeshTrans_PrepareFaceMtlSlot(GetPointer(), out IntPtr raw_mtl_idx));
            // Assign data
            Utils.CKFaceIndicesAssigner(raw_vec_idx, count, vec_idx);
            Utils.CKFaceIndicesAssigner(raw_nml_idx, count, nml_idx);
            Utils.CKFaceIndicesAssigner(raw_uv_idx, count, uv_idx);
            Utils.CKDWORDAssigner(raw_mtl_idx, count, mtl_idx);
        }

    }

}

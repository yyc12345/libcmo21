using System;
using System.Runtime.InteropServices;
using System.Numerics;

namespace BMapSharp.VirtoolsTypes {

    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
    public struct VxVector2 {
        [MarshalAs(UnmanagedType.R4)]
        public float X;
        [MarshalAs(UnmanagedType.R4)]
        public float Y;

        public VxVector2(float _x = 0.0f, float _y = 0.0f) {
            this.X = _x;
            this.Y = _y;
        }
        public VxVector2(Vector2 vec) {
            this.FromManaged(vec);
        }
        public void FromManaged(Vector2 vec) {
            this.X = vec.X;
            this.Y = vec.Y;
        }
        public Vector2 ToManaged() {
            return new Vector2(this.X, this.Y);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
    public struct VxVector3 {
        [MarshalAs(UnmanagedType.R4)]
        public float X;
        [MarshalAs(UnmanagedType.R4)]
        public float Y;
        [MarshalAs(UnmanagedType.R4)]
        public float Z;

        public VxVector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
            this.X = _x;
            this.Y = _y;
            this.Z = _z;
        }
        public VxVector3(Vector3 vec) {
            this.FromManaged(vec);
        }
        public void FromManaged(Vector3 vec) {
            this.X = vec.X;
            this.Y = vec.Y;
            this.Z = vec.Z;
        }
        public Vector3 ToManaged() {
            return new Vector3(this.X, this.Y, this.Z);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
    public struct VxColor {
        [MarshalAs(UnmanagedType.R4)]
        public float A;
        [MarshalAs(UnmanagedType.R4)]
        public float R;
        [MarshalAs(UnmanagedType.R4)]
        public float G;
        [MarshalAs(UnmanagedType.R4)]
        public float B;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
    public struct VxVector4 {
        [MarshalAs(UnmanagedType.R4)]
        public float X;
        [MarshalAs(UnmanagedType.R4)]
        public float Y;
        [MarshalAs(UnmanagedType.R4)]
        public float Z;
        [MarshalAs(UnmanagedType.R4)]
        public float W;
    }
    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
    public struct VxMatrix {
        public VxVector4 x;
        public VxVector4 y;
        public VxVector4 z;
        public VxVector4 w;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 4, CharSet = CharSet.Ansi)]
    public struct CKFaceIndices {
        [MarshalAs(UnmanagedType.U4)]
        public uint I1;
        [MarshalAs(UnmanagedType.U4)]
        public uint I2;
        [MarshalAs(UnmanagedType.U4)]
        public uint I3;

        public CKFaceIndices(uint i1 = 0u, uint i2 = 0u, uint i3 = 0u) {
            this.I1 = i1;
            this.I2 = i2;
            this.I3 = i3;
        }
    }

}

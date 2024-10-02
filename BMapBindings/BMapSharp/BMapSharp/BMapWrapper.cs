using System;
using System.Runtime.InteropServices;
using BMapSharp.VirtoolsTypes;

namespace BMapSharp.BMapWrapper {

    public sealed class BMapGuard : IDisposable {
        internal BMapGuard() {
            BMap.BMInit();
        }

        public void Dispose() {
            throw new NotImplementedException();
        }

        public bool IsBMapAvailable() {

        }
    }

    public static class Utilities {
        public static readonly BMapGuard Guard = new BMapGuard();

        internal static readonly IntPtr INVALID_PTR = IntPtr.Zero;
        internal static readonly uint INVALID_CKID = 0u;
        internal static void BMapSharpCallback(string msg) {
            Console.WriteLine(msg);
        }
    }

    public abstract class AbstractPointer : IComparable<AbstractPointer>, IEquatable<AbstractPointer> {
        private IntPtr mRawPointer;

        protected AbstractPointer(IntPtr raw_pointer) {
            mRawPointer = raw_pointer;
        }

        protected virtual bool IsValid() => mRawPointer != Utilities.INVALID_PTR;
        protected IntPtr GetPointer() => mRawPointer;
        protected void SetPointer(IntPtr raw_pointer) => mRawPointer = raw_pointer;

        #region IComparable

        public int CompareTo(AbstractPointer other) {
            return mRawPointer.CompareTo(other.mRawPointer);
        }

        #endregion

        #region IEquatable

        public static bool operator ==(AbstractPointer left, AbstractPointer right) => left.mRawPointer == right.mRawPointer;
        public static bool operator !=(AbstractPointer left, AbstractPointer right) => left.mRawPointer != right.mRawPointer;

        public bool Equals(AbstractPointer other) => this == other;
        public override bool Equals(object obj) {
            return (!(obj is null)) && (obj is AbstractPointer) && this.Equals((AbstractPointer)obj);
        }

        #endregion

        #region Misc

        public override int GetHashCode() => mRawPointer.GetHashCode();
        public override string ToString() => mRawPointer.ToString();

        #endregion
    }

    public abstract class AbstractCKObject : AbstractPointer, IComparable<AbstractCKObject>, IEquatable<AbstractCKObject> {
        private uint mCKID;

        protected AbstractCKObject(IntPtr raw_pointer, uint ckid) : base(raw_pointer) {
            mCKID = ckid;
        }

        protected override bool IsValid() => base.IsValid() && mCKID != Utilities.INVALID_CKID;
        protected uint GetCKID() => mCKID;

        #region IComparable

        public int CompareTo(AbstractCKObject other) {
            var ret = base.CompareTo((AbstractPointer)other);
            if (ret != 0) return ret;
            return mCKID.CompareTo(other.mCKID);
        }

        #endregion

        #region IEquatable

        public static bool operator ==(AbstractCKObject left, AbstractCKObject right) =>
            ((AbstractPointer)left == (AbstractPointer)right) && left.mCKID == right.mCKID;
        public static bool operator !=(AbstractCKObject left, AbstractCKObject right) =>
            ((AbstractPointer)left != (AbstractPointer)right) || left.mCKID != right.mCKID;

        public bool Equals(AbstractCKObject other) => this == other;
        public override bool Equals(object obj) {
            return (!(obj is null)) && (obj is AbstractCKObject) && this.Equals((AbstractCKObject)obj);
        }

        #endregion

        #region Misc

        public override int GetHashCode() => HashCode.Combine(base.GetHashCode(), mCKID);
        public override string ToString() => $"{base.ToString()} {mCKID.ToString()}";

        #endregion
    }

}

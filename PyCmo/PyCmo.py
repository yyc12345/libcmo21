import VTReader, VTStruct, VTConstants

ckFile = VTStruct.CKFile()
VTReader.CKFileReader.Load(ckFile, "D:\\libcmo21\\PyCmo\\Language.old.nmo", VTConstants.CK_LOAD_FLAGS(VTConstants.CK_LOAD_FLAGS.CK_LOAD_DEFAULT))
print(ckFile)
print(str(ckFile.m_FileObjects))


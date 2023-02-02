import VirtoolsReader
import VirtoolsStruct

with open("D:\\libcmo21\\PyCmo\\Gameplay.nmo", 'rb') as fs:
    composition = VirtoolsReader.ReadCKComposition(fs)
    print(str(composition.Header))
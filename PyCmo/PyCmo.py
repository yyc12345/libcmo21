import VTReader
import VTStruct

with open("D:\\libcmo21\\PyCmo\\Gameplay.nmo", 'rb') as fs:
    composition = VTReader.ReadCKComposition(fs)
    print(composition.Header)
# Tools

The developer need to know the loaded data whether is correct when testing LibCmo. So we create this folder and you can use Unvirt and the tools located in this folder to test the correction of loaded data.

Unvirt can show the data of each CKObject, such as Texture, Mesh and etc. For example, Unvirt can provide vertex's position, normal, UV, even the face's indices data for CKMesh. You can use tools to broswer memory to get them, but you couldn't evaluate them how they shape a mesh. This is the reason why this folder existed and in this README I will tell you how to debug the loaded data.

I suggest you to use HxD to broswer memory, but if you have other softwares, use it freely.

## CKTexture

* Install [PixelViewer](https://github.com/carina-studio/PixelViewer) first.
* Change profile to `BGRA_8888` (actually is little-endian RGBA8888, but I think the developer of PixelViewer get confused).
* The image resolution can be gotten from Uvirt. Set it in PixelViewer.
* The image address also can be gotten from Unvirt. Save the memory image data to local file and open it by PixelViewer.

## CKMesh

* Have a executable Python.
* Save VertexPosition, VertexNormal, VertexUV, FaceIndices data into file according to the given memory address by Unvirt.
* Call `MeshConv.py`, set the argument properly, then you will get a converted Wavefront OBJ file.

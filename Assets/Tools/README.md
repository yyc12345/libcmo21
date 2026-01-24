# Tools

The developer need to know the loaded data whether is correct when testing LibCmo.
So we create this folder and you can use Unvirt and the tools located in this folder to test the correction of loaded data.

Unvirt can show the data of each CKObject, such as Texture, Mesh and etc. 
For example, Unvirt can provide vertex's position, normal, UV, even the face's indices data for CKMesh. 
You can use tools to broswer memory to get them, but you couldn't evaluate them how they shape a mesh. 
This is the reason why this folder existed and in this README I will tell you how to debug the loaded data.

## Memory Inspector

I suggest you to use HxD to broswer memory, but if you have other softwares which you have been familiar with, use it freely.

## CKTexture Debugging

* Install [PixelViewer](https://github.com/carina-studio/PixelViewer) first.
* Change profile to `BGRA_8888` (actually is little-endian RGBA8888, but I think the developer of PixelViewer get confused).
* The image resolution can be gotten from Uvirt. Set it in PixelViewer.
* The image address also can be gotten from Unvirt. Save the image data from memory to local file and open it by PixelViewer.

## CKMesh Debugging

See [MeshConv README](./MeshConv/README.md)

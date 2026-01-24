# MeshConv

Build complete Wavefront OBJ file from separated data for debugging libcmo21.

## Usage

- Restore this project by Astral UV.
- Save all mesh components into separate files in this directory.
    * Vertex position as `VertexPosition.bin` for example.
    * Vertex normal as `VertexNormal.bin` for example.
    * Vertex UV as `VertexUV.bin` for example.
    * Face indices as `FaceIndices.bin` for example.
- Execute `uv run main.py -p VertexPosition.bin -n VertexNormal.bin -u VertexUV.bin -i FaceIndices.bin -o mesh.obj -m mesh.mtl` for example. It will utilize previous saved file to generate a Wavefront OBJ file `mesh.obj` and corresponding material file `mesh.mtl`. For the usage of these switches, please pass `--help` to the program.

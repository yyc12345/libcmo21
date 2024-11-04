@ECHO OFF
:: Navigate to root directory
CD /d {{ repo_root_dir }}

:: Create main binary directory
MKDIR bin
CD bin
:: Create build and install folder
MKDIR build
MKDIR install

:: Build project
CD build
cmake -A x64 -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BMAP=ON {{ '-DNEMO_BUILD_DOC=ON' if build_doc }} -DSTB_IMAGE_PATH="D:\CppLib\stb" -DYYCC_PATH="J:\YYCCommonplace\bin\cpp20\install\x64_Release" -DZLIB_HEADER_PATH="D:\zlib" -DZLIB_BINARY_PATH="D:\zlib\contrib\vstudio\vc14\x64\ZlibDllRelease" ../..
cmake --build . --config RelWithDebInfo
{% if build_doc %}
cmake --build . --target NeMoDocuments
{% endif %}
cmake --install . --prefix=../install --config RelWithDebInfo
CD ..

:: Exit to original path
CD ..
ECHO Windows CMake Build Done

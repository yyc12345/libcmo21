@ECHO OFF
:: Check environment
SET README_PATH=%CD%\README.md
IF EXIST %README_PATH% (
    REM DO NOTHING
) ELSE (
    ECHO Error: You must run this script at the root folder of this project!
    EXIT /b
)

:: Create main binary directory
MKDIR bin
CD bin
:: Create build and install folder
MKDIR build
MKDIR install

:: Check build doc switch
IF NOT "%1"=="NODOC" (
    SET BUILD_DOC_SWITCH=ON
) ELSE (
    SET BUILD_DOC_SWITCH=OFF
)

:: Build project
CD build
cmake  -G "Visual Studio 16 2019" -A x64 -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_DOC=%BUILD_DOC_SWITCH% -DSTB_IMAGE_PATH="D:\CppLib\stb" -DYYCC_PATH="J:\YYCCommonplace\bin\cpp20\install\x64_Debug" -DZLIB_HEADER_PATH="D:\zlib" -DZLIB_BINARY_PATH="D:\zlib\contrib\vstudio\vc14\x64\ZlibDllRelease" ../..
pause
cmake --build . --config Release
IF NOT "%1"=="NODOC" (
    cmake --build . --target NeMoDocuments
)
cmake --install . --prefix=../install --config Release
CD ..

:: Exit to original path
CD ..
ECHO Windows CMake Build Done

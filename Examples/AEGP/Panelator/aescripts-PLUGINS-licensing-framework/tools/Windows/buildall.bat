call "c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
msbuild aescriptsTools.sln /p:Platform="x64",Configuration="Release"

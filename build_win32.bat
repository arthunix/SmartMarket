@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
@REM Important: to build on 32-bit systems, the DX12 backends needs '#define ImTextureID ImU64', so we pass it here.
@set OUT_DIR=Debug
@set OUT_EXE=SmartMarket
@set INCLUDES=/I external\imgui /I external\imgui\backends /I external\libcutils /I external\imgui-outofthree-addons\imguidatechooser /I "%WindowsSdkDir%Include\um" /I "%WindowsSdkDir%Include\shared"
@set SOURCES=main.cc external\imgui\backends\imgui_impl_dx12.cpp external\imgui\backends\imgui_impl_win32.cpp external\imgui\imgui*.cpp external\imgui-outofthree-addons\imguidatechooser\imguidatechooser.cpp
cl /c /EHsc external\libcutils\rbtree.c external\libcutils\common.c external\libcutils\list.c
lib common.obj rbtree.obj list.obj /OUT:libcutils.lib
@set LIBS=d3d12.lib d3dcompiler.lib dxgi.lib libcutils.lib
mkdir Debug
cl /nologo /Zi /MD %INCLUDES% /D ImTextureID=ImU64 /D UNICODE /D _UNICODE %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

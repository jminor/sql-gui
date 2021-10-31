@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
set OUT_DIR=Debug
set OUT_EXE=sql-gui
set INCLUDES=/Iimgui/examples /Iimgui /I%SDL2_DIR%\include /Iimgui/examples\libs\gl3w
set SOURCES=main.cpp imgui/examples\imgui_impl_sdl.cpp imgui/examples\imgui_impl_opengl3.cpp imgui\imgui*.cpp imgui/examples\libs\gl3w\GL\gl3w.c
set LIBS=/libpath:%SDL2_DIR%\lib\x86 SDL2.lib SDL2main.lib opengl32.lib
mkdir %OUT_DIR%
cl /nologo /Zi /MD %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS% /subsystem:console

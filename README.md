
# How to Build

- On Windows with Visual Studio's CLI

```
set SDL2_DIR=path_to_your_sdl2_folder
cl /Zi /MD /Iimgui\examples /Iimgui /I%SDL2_DIR%\include /Iimgui\examples\libs\gl3w main.cpp imgui\examples\imgui_impl_sdl.cpp imgui\examples\imgui_impl_opengl3.cpp imgui\imgui*.cpp imgui\examples\libs\gl3w\GL\gl3w.c /FeDebug/sql-gui.exe /FoDebug/ /link /libpath:%SDL2_DIR%\lib\x86 SDL2.lib SDL2main.lib opengl32.lib /subsystem:console
#          ^^ include paths                                 ^^ source files                                                                                  ^^ output exe                    ^^ output dir   ^^ libraries
# or for 64-bit:
cl /Zi /MD /Iimgui\examples /Iimgui /I%SDL2_DIR%\include /Iimgui\examples\libs\gl3w main.cpp imgui\examples\imgui_impl_sdl.cpp imgui\examples\imgui_impl_opengl3.cpp imgui\imgui*.cpp imgui\examples\libs\gl3w\GL\gl3w.c /FeDebug/sql-gui.exe /FoDebug/ /link /libpath:%SDL2_DIR%\lib\x64 SDL2.lib SDL2main.lib opengl32.lib /subsystem:console
```

- On Linux and similar Unixes

```
c++ `sdl2-config --cflags` -I imgui/examples -I imgui -I imgui/examples/libs/gl3w main.cpp imgui/examples/imgui_impl_sdl.cpp imgui/examples/imgui_impl_opengl3.cpp imgui/imgui*.cpp imgui/examples/libs/gl3w/GL/gl3w.c `sdl2-config --libs` -lGL -ldl
```

- On Mac OS X

```
brew install sdl2
c++ `sdl2-config --cflags` -I imgui/examples -I imgui -I imgui/examples/libs/gl3w main.cpp imgui/examples/imgui_impl_sdl.cpp imgui/examples/imgui_impl_opengl3.cpp imgui/imgui*.cpp imgui/examples/libs/gl3w/GL/gl3w.c `sdl2-config --libs` -framework OpenGl -framework CoreFoundation
```

#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL
#

#CXX = g++
#CXX = clang++

EXE = sql-gui
SOURCES = main.cpp
SOURCES += imgui/examples/imgui_impl_sdl.cpp imgui/examples/imgui_impl_opengl3.cpp
SOURCES += imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp
SOURCES += sqlite/sqlite3.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -Iimgui/examples/ -Iimgui/ -Iimgui/backends -Isqlite
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## OPENGL LOADER
##---------------------------------------------------------------------

## Using OpenGL loader: gl3w [default]
SOURCES += imgui/examples/libs/gl3w/GL/gl3w.c
CXXFLAGS += -Iimgui/examples/libs/gl3w -DIMGUI_IMPL_OPENGL_LOADER_GL3W

## Using OpenGL loader: glew
## (This assumes a system-wide installation)
# CXXFLAGS += -lGLEW -DIMGUI_IMPL_OPENGL_LOADER_GLEW

## Using OpenGL loader: glad
# SOURCES += imgui/examples/libs/glad/src/glad.c
# CXXFLAGS += -Iimgui/examples/libs/glad/include -DIMGUI_IMPL_OPENGL_LOADER_GLAD

## Using OpenGL loader: glbinding
## This assumes a system-wide installation
## of either version 3.0.0 (or newer)
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING3
## or the older version 2.x
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -ldl `sdl2-config --libs`

	CXXFLAGS += `sdl2-config --cflags`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib -L/opt/local/lib

	CXXFLAGS += `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
   ECHO_MESSAGE = "MinGW"
   LIBS += -lgdi32 -lopengl32 -limm32 `pkg-config --static --libs sdl2`

   CXXFLAGS += `pkg-config --cflags sdl2`
   CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:imgui/examples/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:imgui/examples/libs/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:imgui/examples/libs/glad/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:sqlite/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
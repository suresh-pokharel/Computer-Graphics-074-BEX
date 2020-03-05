$(shell mkdir -p bin)

INC = ./include
LIBS = ./libs
SRC = ./src
CC = g++

DBFLAGS = -g3 -gdwarf-2 -msse -msse2 -msse3 -Wall -DIMGUI_IMPL_OPENGL_LOADER_GLAD


XFLAGS = -O3 -msse -msse2 -msse3 -Wall
LIBFLAGS = `pkg-config --static --libs glfw3`
BIN = ./bin
HEADER_FILES = $(SRC)/*.h
OBJS = ray.o common.o HandmadeMath.o

UI_CORE_OBJS := glad.o ui.o HandmadeMath.o ui_primitives.o ui_objects.o common.o

DIMGUI_OBJS := imgui.o imgui_demo.o imgui_draw.o imgui_impl_opengl3.o\
							imgui_widgets.o imgui_impl_glfw.o 

UI_OBJS += $(UI_CORE_OBJS)
UI_OBJS += $(DIMGUI_OBJS)

FULL_OBJS = $(addprefix $(BIN)/, $(OBJS) )
FULL_UI_OBJS = $(addprefix $(BIN)/, $(UI_OBJS) )

FULL_UI_CORE_OBJS = $(addprefix $(BIN)/, $(UI_CORE_OBJS) )

.PHONY: run display

ui: $(FULL_UI_OBJS)
	$(CC) $(FULL_UI_OBJS) $(DBFLAGS) -L $(LIBS) $(LIBFLAGS) -o $(BIN)/ui

ray: $(FULL_OBJS)
	$(CC) $(FULL_OBJS) $(XFLAGS) -L $(LIBS) $(LIBFLAGS) -o $(BIN)/app

debug: $(FULL_OBJS) 
	$(CC) $(FULL_OBJS) $(DBFLAGS) -L $(LIBS) $(LIBFLAGS) -o $(BIN)/app
run: app 
	./bin/app
	feh ./images/out.png
disp:
	feh ./images/out.png
test: ./src/test.cpp
	$(CC) $< $(DBFLAGS) -I $(INC) -o $@

$(BIN)/%.o: $(SRC)/%.cpp
	$(CC) $< $(XFLAGS) -I $(INC) -c -o $@

$(BIN)/%.o: $(SRC)/%.c
	$(CC) $< $(XFLAGS) -I $(INC) -c -o $@
clean:
	rm -rf $(FULL_UI_CORE_OBJS) ray.o

ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

mkfile_path :=$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir :=$(dir $(mkfile_path))

# settings

CPP_COMP?=g++
CPP_COMP_FLAGS?=-Wall

BUILDMODE ?=RELEASE
BUILD_DIR ?=$(current_dir)build/

MAKE_CMD:=make

DEP_RAYLIB_DIR:=raylib/
DEP_RAYLIB_OUTNAME:=libraylib.a
DEP_RAYLIB_BUILD_DIR:=$(BUILD_DIR)libs
DEP_RAYLIB_OUTPATH:=$(DEP_RAYLIB_BUILD_DIR)/$(DEP_RAYLIB_OUTNAME)
DEP_RAYLIB_SRC_FILES:=$(shell find $(DEP_RAYLIB_DIR)src/ -maxdepth 1 -name '*.c')

DEP_IMGUI_DIR:=imgui/
DEP_IMGUI_OUTNAME:=imgui.a
DEP_IMGUI_BUILD_DIR:=$(BUILD_DIR)libs
DEP_IMGUI_OUTPATH:=$(DEP_IMGUI_BUILD_DIR)/$(DEP_IMGUI_OUTNAME)
DEP_IMGUI_OBJ_DIR:=$(DEP_IMGUI_BUILD_DIR)/obs/
DEP_IMGUI_SRC_FILES:=$(shell cd $(DEP_IMGUI_DIR) && find . -maxdepth 1 -name '*.cpp')
DEP_IMGUI_OBJ_FILES:=$(addprefix $(DEP_IMGUI_OBJ_DIR),$(notdir ${DEP_IMGUI_SRC_FILES:.cpp=.o}))
DEP_IMGUI_DEP_FILES:=$(patsubst %.o,%.d,$(DEP_IMGUI_OBJ_FILES))

# if stuff

ifeq ($(detected_OS),Windows)
	MAKE_CMD:=mingw32-make
	BASH_PREFX:=bash -c 
endif

all:$(DEP_IMGUI_OUTPATH) $(DEP_RAYLIB_OUTPATH)

$(DEP_RAYLIB_OUTPATH):$(DEP_RAYLIB_SRC_FILES)
	$(BASH_PREFX)"mkdir -p $(DEP_RAYLIB_BUILD_DIR)"
	$(MAKE_CMD) -C "$(DEP_RAYLIB_DIR)/src" PLATFORM=PLATFORM_DESKTOP RAYLIB_BUILD_MODE=$(BUILDMODE) RAYLIB_RELEASE_PATH="$(DEP_RAYLIB_BUILD_DIR)"

$(DEP_IMGUI_OUTPATH):$(DEP_IMGUI_OBJ_FILES)
	$(BASH_PREFX)"mkdir -p $(DEP_IMGUI_BUILD_DIR)"
	ar rvs $@ $(DEP_IMGUI_OBJ_FILES)

$(DEP_IMGUI_OBJ_DIR)%.o: $(DEP_IMGUI_DIR)%.cpp
	$(BASH_PREFX)"mkdir -p $(DEP_IMGUI_OBJ_DIR)"
	$(CPP_COMP) $(CPP_COMP_FLAGS) -c $< -o $@

-include $(DEP_IMGUI_DEP_FILES)

.PHONY:all clean

clean:
	$(MAKE_CMD) -C "$(DEP_RAYLIB_DIR)/src" clean
	rm -rf $(BUILD_DIR)
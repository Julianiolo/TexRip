# detect stuff
ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

mkfile_path :=$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir :=$(dir $(mkfile_path))

# configure stuff here:

BUILDMODE ?=RELEASE
PLATFORM:=PLATFORM_DESKTOP

CPP_COMP:=g++
CPP_COMP_FLAGS:=-Wall -Wno-narrowing
CPP_COMP_CSTD:=-std=c++17
CPP_COMP_DEP_FLAGS=-MMD -MF ${@:.o=.d}
CPP_COMP_DEFS:=$(addprefix -D,$(PLATFORM))

MAKE_CMD:=make

SRC_DIR:=src/
BUILD_DIR:=build/make/
OBJ_DIR:=$(BUILD_DIR)objs/
DEPENDENCIES_DIR:=dependencies/

OUT_NAME:=TexRip
OUT_DIR:=$(BUILD_DIR)TexRip/
OUT_PATH:=$(OUT_DIR)$(OUT_NAME)

# some if stuff here

ifeq ($(BUILDMODE),DEBUG)
	CPP_COMP_FLAGS += -g
endif

ifeq ($(detected_OS),Windows)
	MAKE_CMD:=mingw32-make
	BASH_PREFX:=bash -c 
endif

SRC_FILES:=$(shell $(BASH_PREFX)"find $(SRC_DIR) -name '*.cpp'")
OBJ_FILES:=$(addprefix $(OBJ_DIR),${SRC_FILES:.cpp=.o})
DEP_FILES:=$(patsubst %.o,%.d,$(OBJ_FILES))

DEPENDENCIES_INCLUDE_PATHS:=dependencies/raylib/src/ dependencies/imgui/ dependencies/eigen/
DEPENDENCIES_LIBS_DIR:=$(BUILD_DIR)dependencies/libs

DEP_LIBS:=libraylib.a imgui.a
DEP_LIBS_PATH:=$(addprefix $(DEPENDENCIES_LIBS_DIR)/,$(DEP_LIBS))

DEP_INCLUDE_FLAGS:=$(addprefix -I,$(DEPENDENCIES_INCLUDE_PATHS))
DEP_LIB_DIR_FLAGS:=$(addprefix -L,$(DEPENDENCIES_LIBS_DIR))
DEP_LIBS_FLAGS:=$(addprefix -l:,$(DEP_LIBS))

ifeq ($(detected_OS),Windows)
	EXTRA_FLAGS:=-lopengl32 -lgdi32 -lwinmm -static -static-libgcc -static-libstdc++
	
	ifeq ($(BUILD_MODE), RELEASE)
		EXTRA_FLAGS += -Wl,--subsystem,windows
	endif
else
	EXTRA_FLAGS:=-no-pie -Wl,--no-as-needed -ldl -lpthread
endif

# rules stuff here

all: $(OUT_PATH)

$(OUT_PATH): deps $(OBJ_FILES)
	#BUILDING TEXRIP
	$(BASH_PREFX)"mkdir -p $(OUT_DIR)"
	$(CPP_COMP) $(CPP_COMP_FLAGS) $(CPP_COMP_CSTD) $(CPP_COMP_DEFS) $(DEP_LIB_DIR_FLAGS) -o $@ $(OBJ_FILES) $(DEP_LIBS_FLAGS) $(EXTRA_FLAGS)
	$(BASH_PREFX)"mkdir -p $(OUT_DIR)assets"
	$(BASH_PREFX)"cp -ar $(SRC_DIR)shaders $(OUT_DIR)assets"
	$(BASH_PREFX)"cp -ar ressources $(OUT_DIR)assets"

$(OBJ_DIR)%.o:%.cpp
	#DEPENDENCY: $@
	$(BASH_PREFX)"mkdir -p $(dir $@)"
	$(CPP_COMP) $(CPP_COMP_FLAGS) $(CPP_COMP_CSTD) $(CPP_COMP_DEFS) $(CPP_COMP_DEP_FLAGS) $(DEP_INCLUDE_FLAGS) -o $@ -c $<

deps:
	$(MAKE_CMD) -C $(DEPENDENCIES_DIR) BUILDMODE=$(BUILDMODE) CPP_COMP=$(CPP_COMP) CPP_COMP_FLAGS="$(CPP_COMP_FLAGS)" BUILD_DIR=$(current_dir)$(BUILD_DIR)dependencies/

-include $(DEP_FILES)

#$(shell echo $${$(DEP_LIBS_PATH)// //"\n"} | grep awk 'NR==1{print;}')
#$(shell echo $${$(DEP_LIBS_PATH)// //"\n"} | grep awk 'NR==2{print;}')

clean:
	$(MAKE_CMD) -C $(DEPENDENCIES_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: all
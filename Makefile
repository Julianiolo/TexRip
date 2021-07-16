mkfile_path :=$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir :=$(dir $(mkfile_path))

BUILDMODE ?=RELEASE
PLATFORM:=PLATFORM_DESKTOP

CPP_COMP:=g++
CPP_COMP_FLAGS:=-Wall
CPP_COMP_DEP_FLAGS=-MMD -MF ${@:.o=.d}
CPP_COMP_DEFS:=$(addprefix -D,$(PLATFORM))

SRC_DIR:=src/
BUILD_DIR:=build/make/
OBJ_DIR:=$(BUILD_DIR)objs/
DEPENDENCIES_DIR:=dependencies/

OUT_NAME:=TexRip
OUT_DIR:=$(BUILD_DIR)TexRip/
OUT_PATH:=$(OUT_DIR)$(OUT_NAME)

SRC_FILES:=$(shell find $(SRC_DIR) -name '*.cpp')
OBJ_FILES:=$(addprefix $(OBJ_DIR),${SRC_FILES:.cpp=.o})
DEP_FILES:=$(patsubst %.o,%.d,$(OBJ_FILES))


DEPENDENCIES_INCLUDE_PATHS:=dependencies/raylib/src/ dependencies/imgui/ dependencies/eigen/
DEPENDENCIES_LIBS_DIR:=$(BUILD_DIR)dependencies/libs

DEP_LIBS:=libraylib.a imgui.a
DEP_LIBS_PATH:=$(addprefix $(DEPENDENCIES_LIBS_DIR)/,$(DEP_LIBS))

#ifeq ($(CPP_COMP),g++)
	DEP_INCLUDE_FLAGS:=$(addprefix -I,$(DEPENDENCIES_INCLUDE_PATHS))
	DEP_LIB_DIR_FLAGS:=$(addprefix -L,$(DEPENDENCIES_LIBS_DIR))
	DEP_LIBS_FLAGS:=$(addprefix -l:,$(DEP_LIBS))
#endif

ifeq ($(BUILDMODE),DEBUG)
	CPP_COMP_FLAGS += -g
endif

all: $(OUT_PATH)

$(OUT_PATH): deps $(OBJ_FILES)
	#BUILDING TEXRIP
	mkdir -p $(OUT_DIR)
	$(CPP_COMP) $(CPP_COMP_FLAGS) $(CPP_COMP_DEFS) -no-pie $(DEP_LIB_DIR_FLAGS) -o $@ $(OBJ_FILES) $(DEP_LIBS_FLAGS) -Wl,--no-as-needed -ldl -lpthread
	mkdir -p $(OUT_DIR)assets
	cp -ar $(SRC_DIR)shaders $(OUT_DIR)assets
	cp -ar ressources $(OUT_DIR)assets

$(OBJ_DIR)%.o:%.cpp
	#DEPENDENCY: $@
	mkdir -p $(dir $@)
	$(CPP_COMP) $(CPP_COMP_FLAGS) $(CPP_COMP_DEFS) $(CPP_COMP_DEP_FLAGS) $(DEP_INCLUDE_FLAGS) -o $@ -c $<

deps:
	make -C $(DEPENDENCIES_DIR) BUILDMODE=$(BUILDMODE) CPP_COMP=$(CPP_COMP) CPP_COMP_FLAGS="$(CPP_COMP_FLAGS)" BUILD_DIR=$(current_dir)$(BUILD_DIR)dependencies/

-include $(DEP_FILES)

#$(shell echo $${$(DEP_LIBS_PATH)// //"\n"} | grep awk 'NR==1{print;}')
#$(shell echo $${$(DEP_LIBS_PATH)// //"\n"} | grep awk 'NR==2{print;}')

clean:
	make -C $(DEPENDENCIES_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: all
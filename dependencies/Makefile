mkfile_path :=$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir :=$(dir $(mkfile_path))

CPP_COMP?=g++
CPP_COMP_FLAGS?=-Wall

BUILDMODE ?=RELEASE
BUILD_DIR ?=$(current_dir)build/

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

all:$(DEP_IMGUI_OUTPATH) $(DEP_RAYLIB_OUTPATH)

$(DEP_RAYLIB_OUTPATH):$(DEP_RAYLIB_SRC_FILES)
	mkdir -p $(DEP_RAYLIB_BUILD_DIR)
	make -C "$(DEP_RAYLIB_DIR)/src" PLATFORM=PLATFORM_DESKTOP RAYLIB_BUILD_MODE=$(BUILDMODE) RAYLIB_RELEASE_PATH="$(DEP_RAYLIB_BUILD_DIR)"

$(DEP_IMGUI_OUTPATH):$(DEP_IMGUI_OBJ_FILES)
	echo $(DEP_IMGUI_OBJ_FILES)
	mkdir -p $(DEP_IMGUI_BUILD_DIR)
	ar rvs $@ $(DEP_IMGUI_OBJ_FILES)
#cd $(DEP_IMGUI_OBJ_DIR) && 

#$(DEP_IMGUI_DIR)$(shell find $(DEP_IMGUI_DIR) -name '$(notdir %.cpp)')
$(DEP_IMGUI_OBJ_DIR)%.o: $(DEP_IMGUI_DIR)%.cpp
#	echo $@ #### $< ####### $> ######### $(DEP_IMGUI_DIR)$(shell find $(DEP_IMGUI_DIR) -name '$(notdir ${@:.o=.cpp})')
	mkdir -p $(DEP_IMGUI_OBJ_DIR)
	$(CPP_COMP) $(CPP_COMP_FLAGS) -c $< -o $@

-include $(DEP_IMGUI_DEP_FILES)

.PHONY:all

clean:
	make -C "$(DEP_RAYLIB_DIR)/src" clean
	rm -rf $(BUILD_DIR)
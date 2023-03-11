# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS +=

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Exported code
FLAGS += -Isrc/genlib -Igen/exports -DGENLIB_NO_JSON
SOURCES += $(wildcard src/cmagslib/*.cpp)
SOURCES += $(wildcard src/genlib/*.cpp)
SOURCES += $(wildcard src/modules/waveset-osc/*.cpp)

# Add .cpp files to the build
# VCV module
FLAGS += -Isrc 
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard src/modules/waveset-osc/*.cpp)
SOURCES += $(wildcard src/modules/supersaw/*.cpp)
SOURCES += $(wildcard src/modules/clock/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

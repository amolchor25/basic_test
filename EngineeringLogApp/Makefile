# Everything beginning with "#" is a comment.

# Write the name of your Module/Submodule or the Name of the TARGET to specify the current build
# This will be used as parameter to echo Command

    MESSAGE := EngineeringLogApp

# Indicate whether you are working on BOARD or PC
# (Please use capital letters)
# Example: 1) PLATFORM = BOARD
#          2) PLATFORM = PC
ifeq (0,${MAKELEVEL})
    PLATFORM = BOARD
    export PLATFORM
endif


# Define the Target binary

    TARGET := EngineeringLogApp.out


# Give the Relative path (level) to "SOURCE" Directory
# REL_LEVEL should end with a "/" (back slash)

    REL_LEVEL := ../

# Current directory name to create build directory

    LOCAL_DIR := $(MESSAGE)
# Including files
# Do not Modify.

include $(REL_LEVEL)RECIPES/MakefileVariables.mk
include $(UT_LOCAL_DIR)/UT_AuditLogApp.mk

# Include other Module Interface header files.
# Leave this blank if you donot have any.

INCLUDE_DIR += -I$(REL_LEVEL)LIBRARIES/COMMON/SoftwareCenter
INCLUDE_DIR += -I$(REL_LEVEL)EngineeringLogApp
#BUILD_DIRS += $(REL_LEVEL)BUILD/OBJ/PC/AuditLogApp
#BUILD_DIRS += $(REL_LEVEL)BUILD/OBJ/BOARD/AuditLogApp


#
# Specify the static LIBRARIES to be linked
# Leave this blank if you donot have any.

# Linker Flag

# Modify the Flag if required

 
    LDFLAGS   += -lpthread
    LDFLAGS   += -lPlatform
    LDFLAGS   += -lPlatformLinux
    LDFLAGS   += -lsystemd
    LDFLAGS   += -lutility
    LDFLAGS   += -ldol
    LDFLAGS   += -lrt
    LDFLAGS   += -ltinyxml2
    LDFLAGS   += -lcrypto
    LDFLAGS   += -lssl

	
# Source files for this Build

    SOURCES := $(wildcard SRC/*.cpp)
    SOURCES := $(subst SRC/, ,$(SOURCES))

# Object files for this Build
# Do not modify

    OBJECTS =  $(addprefix $(LOCAL_BUILD)/, $(subst .cpp,.o,$(SOURCES)))

# Search Paths for Header Files
# Donot Modify

    vpath %.h $(INCLUDE_DIR)
    vpath %.cpp $(UT_LOCAL_DIR)

# Search Paths for Source Files
# Donot Modify

    vpath %.cpp $(LOCAL_SRC)


    MAKEBUILD = RELEASE


#################### Targets ####################
# Donot Modify

all : makedirectories googletest common $(TARGET)
	@echo "$(MESSAGE) All done"

$(TARGET) : $(OBJECTS) $(LINK_LIB)
	$(TARGET_RULE)

# Include dependency file
ifneq "$(MAKECMDGOALS)" "clean"
    -include $(subst .o,.d,$(OBJECTS))
endif

ifeq "$(MAKECMDGOALS)" "ut"
	CFLAGS +=-DUT_TARGET
	CPPFLAGS +=-DUT_TARGET
endif

.PHONY:ut
ut : MAKEBUILD = UNITTEST
     export MAKEBUILD

ut : $(UT_TARGET)
	@echo "$(MESSAGE)-UT All done"

$(UT_TARGET) : $(UT_OBJECTS) $(UT_REF_OBJECTS)
	$(CXX) $(UT_OBJECTS) $(UT_REF_OBJECTS) $(LDFLAGS) -o $(GLOBAL_BIN)/$(UT_TARGET)

makedirectories:
	@mkdir -p $(BUILD_DIRS)

common:
	$(MAKE) --directory=$(REL_LEVEL)LIBRARIES/COMMON/

googletest:
	$(MAKE) --directory=$(REL_LEVEL)SYSAPPS

.PHONY:clean
.PHONY:clean
clean:
	$(RM) $(GLOBAL_BIN)/$(TARGET)
	$(RM) $(LOCAL_BUILD)/*.o $(LOCAL_BUILD)/*.d $(LOCAL_BUILD)/*.gcno $(LOCAL_BUILD)/*.gcda
	$(RM) -r $(DOXYGEN_DIR)
	$(RM) $(GLOBAL_BIN)/$(UT_TARGET)
	$(RM) doxygenwarnings.txt

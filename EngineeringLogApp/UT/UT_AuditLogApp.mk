# Define the UT Target binary

    UT_TARGET = ut_EngineeringLogApp.out

# UT Source files for this Buil
    UT_SOURCES := PanelModuleInfoFileProviderTest.cpp
    UT_SOURCES := $(subst UT/, ,$(UT_SOURCES))
	INCLUDE_DIR += -I$(REL_LEVEL)EngineeringLogApp
    #UT_SOURCES := $(subst UT/, ,$(UT_SOURCES))
    $(info $$UT_SOURCES is [${UT_SOURCES}])
# UT Object files for this Build
# Do not modify

    UT_OBJECTS := $(addprefix $(LOCAL_BUILD)/, $(subst .cpp,.o,$(UT_SOURCES)))

# Refference source files for this build

    #UT_REF_SOURCES := $(wildcard SRC/*.cpp)
    #UT_REF_SOURCES := $(subst SRC/, ,$(UT_REF_SOURCES))

# Refference object files for this build
    LDFLAGS   += -ldol
    LDFLAGS   += -lPlatform
    LDFLAGS   += -lPlatformLinux
    LDFLAGS   += -lFtpServer
    LDFLAGS   += -lfmt
    LDFLAGS   += -lrt
    LDFLAGS   += -ltinyxml
    LDFLAGS   += -ltinyxml2
    LDFLAGS   += -lgmock
# Do not modify

    #UT_REF_OBJECTS := $(addprefix $(LOCAL_BUILD)/, $(subst .cpp,.o,$(UT_REF_SOURCES)))
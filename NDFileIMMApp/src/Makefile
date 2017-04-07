TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for the source files that define template functions.
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
CODE_CXXFLAGS=
endif

#USR_INCLUDES += $(GOOGLEPROTO_INCLUDE)

DBD += IMMpluginSupport.dbd

INC += NDFileIMM.h
INC += compressed_file2.h
INC += image_file2.h
INC += drvNDFileIMM.h

LIBRARY_IOC += NDPluginIMM

NDPluginIMM_SRCS += NDFileIMM.cpp
NDPluginIMM_SRCS += compressed_file2.cpp
NDPluginIMM_SRCS += image_file2.cpp

include $(ADCORE)/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


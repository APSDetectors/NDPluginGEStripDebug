#NDPluginFileIMM
Traditional area detector plugin for writing areaDetector Images to an IMM file.
This file format is specific to the XPCS beamlines at the APS.

In order to install this plugin, the following modifications were made in
areaDetector, ADCore, & ioc

In areaDetector/configure/RELEASE.local add:
```
NDPLUGINFILEIMM=$(AREA_DETECTOR)/NDPluginFileIMM
```
To areaDetector/Makefile add:
```
DIRS := $(DIRS) NDPluginFileIMM
$(NDPluginFileIMM)_DEPEND_DIRS += $(ADCore)
```
in $(ADCore)/ADApp/commonDriverMakefile add:
```
ifdef NDPLUGINFILEIMM
  PROD_LIBS             += NDPluginFileIMM
  $(PROD_NAME)_DBD      += NDPluginFileIMM.dbd

endif
```

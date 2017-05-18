# Must have loaded envPaths via st.cmd.linux or st.cmd.win32

errlogInit(20000)

dbLoadDatabase("$(TOP)/dbd/simDetectorApp.dbd")
simDetectorApp_registerRecordDeviceDriver(pdbbase) 

# Prefix for all records
epicsEnvSet("PREFIX", "13SIM1:")
# The port name for the detector
epicsEnvSet("PORT",   "SIM1")
# The queue size for all plugins
epicsEnvSet("QSIZE",  "20")
# The maximum image width; used to set the maximum size for this driver and for row profiles in the NDPluginStats plugin
epicsEnvSet("XSIZE",  "1024")
# The maximum image height; used to set the maximum size for this driver and for column profiles in the NDPluginStats plugin
epicsEnvSet("YSIZE",  "1024")
# The maximum number of time series points in the NDPluginStats plugin
epicsEnvSet("NCHANS", "2048")
# The maximum number of frames buffered in the NDPluginCircularBuff plugin
epicsEnvSet("CBUFFS", "500")
# The maximum number of threads for plugins which can run in multiple threads
epicsEnvSet("MAX_THREADS", "8")
# The search path for database files
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")

epicsEnvSet("IMM","$(AREA_DETECTOR)/NDPluginFileIMM/NDFileIMMApp/Db")



asynSetMinTimerPeriod(0.001)

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "40000000")

simDetectorConfig("$(PORT)", $(XSIZE), $(YSIZE), 1, 0, 0)
dbLoadRecords("$(ADSIMDETECTOR)/db/simDetector.template","P=$(PREFIX),R=cam1:,PORT=$(PORT),ADDR=0,TIMEOUT=1")


# Create a standard arrays plugin, set it to get data from first simDetector driver.
NDStdArraysConfigure("Image1", 20, 0, "$(PORT)", 0, 0, 0, 0, 0, 5)

# This creates a waveform large enough for 2000x2000x3 (e.g. RGB color) arrays.
# This waveform only allows transporting 8-bit images
dbLoadRecords("NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int8,FTVL=UCHAR,NELEMENTS=12000000")
# This waveform allows transporting 64-bit images, so it can handle any detector data type at the expense of more memory and bandwidth
dbLoadRecords("NDStdArrays.template", "P=$(PREFIX),R=image2:,PORT=Image2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=SIM2,TYPE=Float64,FTVL=DOUBLE,NELEMENTS=12000000")


#IMM plugin
NDFileIMMConfigure("IMM",10000000,300,0,"SCATTER1",50,0)
dbLoadRecords("NDFile.template","P=$(PREFIX),R=IMM:,PORT=IMM,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(IMM)/NDFileIMM.template","P=$(PREFIX),R=IMM:,PORT=IMM,NDARRAY_PORT=SCATTER1,ADDR=0,TIMEOUT=1")


#IMM plugin
NDFileIMMConfigure("IMM2",10000000,300,0,"SCATTER1",50,0)
dbLoadRecords("NDFile.template","P=$(PREFIX),R=IMM2:,PORT=IMM2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(IMM)/NDFileIMM.template","P=$(PREFIX),R=IMM2:,PORT=IMM2,NDARRAY_PORT=SCATTER1,ADDR=0,TIMEOUT=1")


#IMM plugin
NDFileIMMConfigure("IMM3",10000000,300,0,"SCATTER1",50,0)
dbLoadRecords("NDFile.template","P=$(PREFIX),R=IMM3:,PORT=IMM3,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(IMM)/NDFileIMM.template","P=$(PREFIX),R=IMM3:,PORT=IMM3,NDARRAY_PORT=SCATTER1,ADDR=0,TIMEOUT=1")


#IMM plugin
NDFileIMMConfigure("IMM4",10000000,300,0,"GATHER1",50,0)
dbLoadRecords("NDFile.template","P=$(PREFIX),R=IMM4:,PORT=IMM4,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(IMM)/NDFileIMM.template","P=$(PREFIX),R=IMM4:,PORT=IMM4,NDARRAY_PORT=GATHER1,ADDR=0,TIMEOUT=1")



# Create a scatter plugin
NDScatterConfigure("SCATTER1", $(QSIZE), 0, "$(PORT)", 0, 0, 0)
dbLoadRecords("NDScatter.template",   "P=$(PREFIX),R=Scatter1:,  PORT=SCATTER1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")

# Create a gather plugin with 8 ports
NDGatherConfigure("GATHER1", $(QSIZE), 0, 8, 0, 0)
dbLoadRecords("NDGather.template",   "P=$(PREFIX),R=Gather1:, PORT=GATHER1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=IMM")

dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=1, PORT=GATHER1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=IMM1")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=2, PORT=GATHER1,ADDR=1,TIMEOUT=1,NDARRAY_PORT=IMM1)")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=3, PORT=GATHER1,ADDR=2,TIMEOUT=1,NDARRAY_PORT=IMM2")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=4, PORT=GATHER1,ADDR=3,TIMEOUT=1,NDARRAY_PORT=$(PORT)")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=5, PORT=GATHER1,ADDR=4,TIMEOUT=1,NDARRAY_PORT=$(PORT)")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=6, PORT=GATHER1,ADDR=5,TIMEOUT=1,NDARRAY_PORT=$(PORT)")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=7, PORT=GATHER1,ADDR=6,TIMEOUT=1,NDARRAY_PORT=$(PORT)")
dbLoadRecords("NDGatherN.template",   "P=$(PREFIX),R=Gather1:, N=8, PORT=GATHER1,ADDR=7,TIMEOUT=1,NDARRAY_PORT=$(PORT)")


set_requestfile_path("$(ADSIMDETECTOR)/simDetectorApp/Db")

#asynSetTraceIOMask("$(PORT)",0,2)
#asynSetTraceMask("$(PORT)",0,255)
#asynSetTraceIOMask("FileNetCDF",0,2)
#asynSetTraceMask("FileNetCDF",0,255)
#asynSetTraceMask("FileNexus",0,255)
#asynSetTraceMask("SIM2",0,255)

iocInit()

# save things every thirty seconds
create_monitor_set("auto_settings.req", 30, "P=$(PREFIX)")


dbpf 13SIM1:Pva1:EnableCallbacks, 1

dbpf 13SIM1:IMM:NDFileIMM_throw_images, 1
dbpf 13SIM1:IMM2:NDFileIMM_throw_images, 1
dbpf 13SIM1:IMM3:NDFileIMM_throw_images, 1

dbpf 13SIM1:IMM:EnableCallbacks, 1
dbpf 13SIM1:IMM2:EnableCallbacks, 1
dbpf 13SIM1:IMM3:EnableCallbacks, 1
dbpf 13SIM1:IMM4:EnableCallbacks, 1


dbpf 13SIM1:IMM:NDArrayAddress,0
dbpf 13SIM1:IMM2:NDArrayAddress,0
dbpf 13SIM1:IMM3:NDArrayAddress,0

dbpf 13SIM1:IMM:NDArrayPort,"SCATTER1"
dbpf 13SIM1:IMM2:NDArrayPort,"SCATTER1"
dbpf 13SIM1:IMM3:NDArrayPort,"SCATTER1"


dbpf 13SIM1:IMM:FileName,"NULL"
dbpf 13SIM1:IMM2:FileName,"NULL"
dbpf 13SIM1:IMM3:FileName,"NULL"

dbpf 13SIM1:IMM:NumCapture,-1
dbpf 13SIM1:IMM2:NumCapture,-1
dbpf 13SIM1:IMM3:NumCapture,-1

dbpf 13SIM1:IMM:ArrayCallbacks,0
dbpf 13SIM1:IMM2:ArrayCallbacks,0
dbpf 13SIM1:IMM3:ArrayCallbacks,0

dbpf 13SIM1:IMM:FileFormat,1
dbpf 13SIM1:IMM2:FileFormat,1
dbpf 13SIM1:IMM3:FileFormat,1

dbpf 13SIM1:cam1:AcquireTime,0.5

dbpf 13SIM1:Gather1:EnableCallbacks,1
dbpf 13SIM1:Gather1:ArrayCallbacks,1

dbpf 13SIM1:Gather1:NDArrayPort_1,"IMM"
dbpf 13SIM1:Gather1:NDArrayPort_2,"IMM2"
dbpf 13SIM1:Gather1:NDArrayPort_3,"IMM3"


dbpf 13SIM1:Gather1:NDArrayPort_4,""
dbpf 13SIM1:Gather1:NDArrayPort_5,""
dbpf 13SIM1:Gather1:NDArrayPort_6,""
dbpf 13SIM1:Gather1:NDArrayPort_7,""
dbpf 13SIM1:Gather1:NDArrayPort_8,""


dbpf 13SIM1:Scatter1:EnableCallbacks,1
dbpf 13SIM1:Scatter1:ArrayCallbacks,1

dbpf 13SIM1:IMM4:NDArrayPort,"GATHER1"
dbpf 13SIM1:IMM4:NDArrayAddress,0


dbpf 13SIM1:cam1:AcquireTime,0.01



dbpf 13SIM1:IMM:NDFileIMM_Nimg_rst_ts,100
dbpf 13SIM1:IMM2:NDFileIMM_Nimg_rst_ts,100
dbpf 13SIM1:IMM3:NDFileIMM_Nimg_rst_ts,100
dbpf 13SIM1:IMM4:NDFileIMM_Nimg_rst_ts,100



dbpf 13SIM1:cam1:Acquire,1
epicsThreadSleep 2.0

dbpf 13SIM1:cam1:Acquire,0




dbpf 13SIM1:IMM:Capture,1
dbpf 13SIM1:IMM2:Capture,1
dbpf 13SIM1:IMM3:Capture,1



dbpf 13SIM1:cam1:Acquire,1
epicsThreadSleep 2.0

dbpf 13SIM1:cam1:Acquire,0




dbpf 13SIM1:cam1:AcquireTime,0.5



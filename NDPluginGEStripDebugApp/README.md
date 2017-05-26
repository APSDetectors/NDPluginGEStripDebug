# NDPluginPipeWriter

This is a file plugin for [EPICS](http://www.aps.anl.gov/epics/) 
[areaDetector](http://cars.uchicago.edu/software/epics/areaDetector.html) that 
is intended to write data from the NDArray to a Unix Pipe instead of to a
physical file.  The data written to this pipe is intended to feed the 
[detectorMPI](https://github.com/argonnexraydetector/detectorMPI) with the 
intent that detectorMPI will compress the data and then output images in 
the IMM data file format used by XPCS beamlines such as 8-ID-I at the 
Advanced Photon Source.

This plugin, in addition to writing data from the NDArray into compressed IMM
file, also provides links to the detectorMPI program, that allow setting/getting
parameters of that program, via separate command input and output pipes.   
  
  

Running in scatter/gather mode.  
To produce NDArrays, set NDFileIMM_throw_images to 1
To compute IMM but not save files, name filename to NULL.
To run forever, set Capture, or num of frames to capture to -1.




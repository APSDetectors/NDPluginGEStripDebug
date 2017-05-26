/*
 * NDFileTIFF.h
 * Writes NDArrays to TIFF files.
 * John Hammonds
 * April 17, 2009
 */

#ifndef NDPluginGeDebug_H
#define NDPluginGeDebug_H


#ifdef _WIN32
#include <windows.h>

#endif


#ifndef _WIN32
#define _stat stat
#define _chmod chmod

#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
using std::string;
#ifdef _WIN32
#include <direct.h>
#endif




#include "NDPluginDriver.h"





/** Writes NDArrays in the TIFF file format.
    Tagged Image File Format is a file format for storing images.  The format was originally created by Aldus corporation and is
    currently developed by Adobe Systems Incorporated.  This plugin was developed using the libtiff library to write the file.
    The current version is only capable of writes 2D images with 1 image per file.
    */

class NDPluginGeDebug : public NDPluginDriver {
public:
    NDPluginGeDebug(const char *portName,int max_imm_bytes, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int priority, int stackSize);

	virtual void processCallbacks(NDArray *pArray);
    /* The methods that this class implements */

// virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
//                             const char **pptypeName, size_t *psize);





protected:
    //
    // params
    //

    int GePD_messagetype;
    int GePD_num_mess_start;
    int GePD_num_mess_meta;
    int GePD_num_mess_data;
    int GePD_num_mess_fnum;
    int GePD_frame_num;

   //ebd oarans
   //
    
    
    
    
    
    
    
    
    
    int arrayDataId;
    int uniqueIdId;
    int timeStampId;
    int nextRecord;
    int *pAttributeId;
    


	NDAttribute *pAttribute;
    char name[256];
    char description[256];
    char tempString[256];
    NDAttrDataType_t attrDataType;
    size_t attrSize;
    int numAttributes, attrCount;

	int ii0,ii1;

	int threshold, is_imm_comp;
		double acq_time;
	
	int imm_pixels;
	unsigned int fpga_timestamp;
	

    int is_already_imm;
    int pipe_num_shorts;

	int bytesperpix;
	int cam_type;
	int fileformat;

	int last_filenumber;
	bool is_valid_head;


	int last_unique_id;
	double last_timestamp;
	int timestamp_reset_counter;

	// if fpga or coreco attrubute,we use this timestamp.
	bool is_coreco_timestamp;
	double general_timestamp;


	enum which_timestamp {
		xcorecoticks,
		xfpga_ts,
		xarray_timestamp,
		xarray_unique_id

	};

	NDArray *my_array;

	//
	// Params
	//

	enum {num_params=18};



	int getIntParam(int param){
		int output;
		getIntegerParam(param,&output);
		return(output);
	};


	double getDoubParam(int param){
		double output;
		getDoubleParam(param,&output);
		return(output);
	};

};

#endif



/* NDFileIMM.cpp
 * Writes NDArrays to IMM files.
 *
 * Timothy Madden
 * April 17, 2008
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>
#include <math.h>
#include "NDPluginFile.h"
#include "NDFileIMM.h"
#include "drvNDFileIMM.h"
// #include "tiffio.h"
#include "compressed_file2.h"
#include "image_file2.h"
#include "asynNDArrayDriver.h"

#ifdef _WIN32
#include <io.h>
#endif
static const char *driverName = "NDFileIMM";


//
// Section for Custom Parameters
//

/** Sets pasynUser->reason to one of the enum values for the parameters defined for
  * the NDPluginFile class if the drvInfo field matches one the strings defined for it.
  * If the parameter is not recognized by this class then calls NDPluginDriver::drvUserCreate.
  * Uses asynPortDriver::drvUserCreateParam.
  * \param[in] pasynUser pasynUser structure that driver modifies
  * \param[in] drvInfo String containing information about what driver function is being referenced
  * \param[out] pptypeName Location in which driver puts a copy of drvInfo.
  * \param[out] psize Location where driver puts size of param
  * \return Returns asynSuccess if a matching string was found, asynError if not found. */

/*asynStatus NDFileIMM::drvUserCreate(asynUser *pasynUser,
                                       const char *drvInfo,
                                       const char **pptypeName, size_t *psize)
{
    asynStatus status;
    //const char *functionName = "drvUserCreate";

	printf("Calling NDFileIMM::drvUserCreate- %i\n",NUM_ND_FILE_IMM_PARAMS);
    status = this->drvUserCreateParam(pasynUser, drvInfo, pptypeName, psize,
                                      NDFileIMMParamString, NUM_ND_FILE_IMM_PARAMS);

    //If not, then call the base class method, see if it is known there
    if (status) status = NDPluginFile::drvUserCreate(pasynUser, drvInfo, pptypeName, psize);
    return(status);
}


*/

//
//End Section for Custom Params
//



/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, \
                "%s:%s error=%s\n", \
                driverName, functionName, nc_strerror(e)); \
                return(asynError);}

/* NDArray string attributes can be of any length, but netCDF requires a fixed maximum length
 * which we define here. */
#define MAX_ATTRIBUTE_STRING_SIZE 256

/** This is called to open a TIFF file.
*/
asynStatus NDFileIMM::openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray)
{
    /* When we create TIFF variables and dimensions, we get back an
     * ID for each one. */
/*    nc_type ncType=NC_NAT;*/
	char str0[256];
	char str1[256];
	int fn0,fn1;
	int is_update;
    static const char *functionName = "openFile";

    //
    // if filename is NULL, then we don't open a file on filesystem, but we will pretend we are writing to files,. this is for
    // compuyting IMM format without actuallty writing to disk. 
    //


    /* We don't support reading yet */
    if (openMode & NDFileModeRead) return(asynError);

    /* We don't support opening an existing file for appending yet */
    if (openMode & NDFileModeAppend) return(asynError);



	if (getIntParam(NDFileWriteMode) !=NDFileModeStream)
	{
		printf("Only support Stream Mode\n");
		return(asynError);
	}

    /* Set the next record in the file to 0 */

	

//	printf("FileName: %s\n", fileName);
    /* Create the file. */
//!! we shoudl override CDPluginDriver:;, NDPludFile, openBase, Writebase and createFileName functions to take care of filenumbers and filenames.

		// -1 because baseclass autoincs by 1. so we take it off...



				getStringParam(NDFileName,255,str0);
				getStringParam(NDFilePath,255,str1);
				getIntegerParam(NDFileNumber,&fn0);
				getIntegerParam(NDFileNumCapture,&fn1);

	// this is a kludge because base class uincs the NDFileNumber for us. we are negating that so
				// we can keep track ourselves here.
	getIntegerParam(NDAutoIncrement,&is_update);
	if (is_update==1)
	{
		fn0=fn0-1;
		setIntegerParam(NDFileNumber,fn0);
	}

// keep track of filenumber so if base class messes it up we are not consused...
	//			last_filenumber = fn0;

	this->nextRecord = 0;;

	 cf->open_w(
				str0,
				str1,
				fn0,
				fn0+fn1 -1);
//			cf->open_w(fileName,-1,-1);





    if (cf->myfile !=0)
    	setStringParam(NDFullFileName,cf->myfile->file_name);

   

	is_open_good = cf->is_open;

	if (!is_open_good)
	{
		printf("Error- Could not open IMM File\n");
		   setIntegerParam(NDFileWriteStatus, NDFileWriteError);
		   setStringParam(NDFileWriteMessage, "Error: Could not open file. Check path.");
	    return(asynError);
	}
	else
	{
		setIntegerParam(NDFileIMM_fileevent,1);
		printf("Opened IMM File\n");
		   setIntegerParam(NDFileWriteStatus, NDFileWriteOK);
		   setStringParam(NDFileWriteMessage, "");

        int chstat;
        if (cf->myfile != 0)
		    chstat=_chmod(cf->myfile->file_name,0777);

	}
    return(asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus NDFileIMM::writeFile(NDArray *pArray)
{
	unsigned long int stripsize;
    static const char *functionName = "writeFile";
	int sizex, sizey;
	int is_update;
	int fnx;
	int is_throw_frame=0;
	size_t dimSizeOut[ND_ARRAY_MAX_DIMS];
    int i;

	if (!is_open_good)
	{
		printf("Error- Could not open IMM File\n");
		   setIntegerParam(NDFileWriteStatus, NDFileWriteError);
		   setStringParam(NDFileWriteMessage, "Error: Could not open file. Check path.");
	    return(asynError);
	}



	
	
		file_coreco_ts=getIntParam(NDFileIMM_uniqueID);


		file_elapsed_ts=getDoubParam(NDFileIMM_timestamp);
	
		file_systick_ts=getIntParam(NDFileIMM_uniqueID);


		getIntegerParam(NDFileNumber,&fnx);

		getIntegerParam(NDAutoIncrement,&is_update);



	sizex = pArray->dims[0].size;
	sizey = pArray->dims[1].size;

    switch(pArray->dataType) {
        case NDInt8:
            bytesperpix=1;
            break;
        case NDUInt8:
            bytesperpix=1;
            break;
        case NDInt16:
            bytesperpix=2;
            break;
        case NDUInt16:
            bytesperpix=2;
            break;
        case NDInt32:
            bytesperpix=4;
            break;
        case NDUInt32:
            bytesperpix=4;
            break;
        case NDFloat32:
            bytesperpix=4;
            break;
        case NDFloat64:
            bytesperpix=8;
            break;
        default:
            bytesperpix=1;
            break;
    }



		getIntegerParam(NDFileIMM_throw_images,&is_throw_frame);
			//is_throw_frame=0;
		if (is_throw_frame)
		{
			this->lock();
			if (my_array!=0)
			{
				my_array->release();
				my_array = 0;
			}


			//for (i=0; i<pArray->ndims; i++)
			    //	dimSizeOut[i] = pArray->dims[i].size;
			    //have to assume as have 2 dims for image.
			    // we add 2000 for the header.. it is act only 1024, but we get extra room...
		    dimSizeOut[0]=(size_t)1;
		    dimSizeOut[1]= this->max_imm_bytes;
		        my_array = this->pNDArrayPool->alloc(2,dimSizeOut, NDUInt8, (size_t)0, (void*)0);

              int one = 1;

            //add new attr to img, if not already there. if there, it updates values         
            my_array->pAttributeList->add(
                "is_already_imm", 
                "if 1 then this is imm data",
                 NDAttrInt32, 
                 &one);

             my_array->uniqueId = pArray->uniqueId;
             my_array->timeStamp= pArray->timeStamp;


			if (my_array==0)
			{
				printf("ERROR- IMM plugin could not get NDArray\n");
				setIntegerParam(
					NDFileIMM_NmissedTimeStamps,
					getIntParam(NDFileIMM_NmissedTimeStamps)+1);
				is_throw_frame =0;
			}
		//	my_array = pNDArrayPool->copy(pArray, NULL, 1);
			this->unlock();
		}

//NDFileFormat

	getIntegerParam(NDFileFormat,&fileformat);
	// fileformat=0 RAW, 1 for compressed
	//printf("Fileformat %i",fileformat);


    cf->setBufferNumber(this->nextRecord);

	//setIntegerParam(NDFileIMM_threshold,threshold);
// need to determint what type of IMM to wrute:
//raw data, FPGA compressed data, raw data written as compressed
// for now just do raw...

    if (is_already_imm)
    {

		compressed_header *immh = (compressed_header*)(pArray->pData);

		immh->buffer_number = this->nextRecord;

        int imm_bytes = cf->getNumBytes(immh);
        
        if (cf->myfile != 0)
            cf->myfile->write((char*)(pArray->pData),imm_bytes);
    
        this->nextRecord++;

        getIntegerParam(NDFileNumber,&fnx);

    if (is_update==1)
        setIntegerParam(NDFileNumber,fnx+1);
  


    }
    else if (fileformat==0)
	{
		    //printf("saveFileIMMRaw\n");
		cf->saveFileIMMRaw(
		     file_coreco_ts,
	 	    file_elapsed_ts,
	 	    file_systick_ts,//timestamp- ise ndarray param
		    threshold,//thresh
		    sizex,
		    sizey,
		    bytesperpix,// bytes per pix-00 need to get this from NDArray... use short for now
		    pArray->pData,
		    this->nextRecord,//fiklenumber- where do we get it?
		    cam_type,// camtype should be a ndarray param
		    acq_time//acq period- do we read the param or use an ndarray param?
		    );

		    this->nextRecord++;

		    getIntegerParam(NDFileNumber,&fnx);
            
               
        setIntegerParam(NDFileIMM_is_already_imm,0);
        setIntegerParam(NDFileIMM_imm_systicks,file_systick_ts);
        setIntegerParam(NDFileIMM_imm_corecoticks, file_coreco_ts );
        setIntegerParam(NDFileIMM_imm_dlen,cf->last_nbytes );
        setDoubleParam(NDFileIMM_imm_elapsed,file_elapsed_ts);
        
            
            
            

	    if (is_update==1)
		    setIntegerParam(NDFileNumber,fnx+1);


	     if (is_throw_frame)
	     {
		      if (cf->last_nbytes<=my_array->dataSize)
		      {
			      memcpy(my_array->pData, cf->last_data, cf->last_nbytes);
		     //my_array->dims[0].size=cf->last_nbytes;
		     //my_array->dims[1].size=1;
		     //my_array->dataType= NDUInt8;

		    /* Get the attributes for this driver */
	        this->getAttributes(my_array->pAttributeList);


		    doCallbacksGenericPointer(my_array, NDArrayData, 0);
		    }
		    else
		    {
		    printf("Error- Could not throw IMM to plugins- not enough mem\n");
		       setIntegerParam(NDFileWriteStatus, NDFileWriteError);
		       setStringParam(NDFileWriteMessage, "Error- Could not throw IMM to plugins- not enough mem.");
		    }
		    callParamCallbacks();


	     }



	}
	else
	{
		
		
	//	printf("saveFileIMMComp\n");
		cf->saveFileIMMComp(
				 file_coreco_ts,
	 			file_elapsed_ts,
	 			file_systick_ts,
				threshold,//thresh
				sizex,
				sizey,
				bytesperpix,// bytes per pix-00 need to get this from NDArray... use short for now
				pArray->pData,
				this->nextRecord,//fiklenumber- where do we get it?
				cam_type,// camtype should be a ndarray param
				acq_time,//acq period- do we read the param or use an ndarray param?
				&imm_pixels
				);
//			setIntegerParam(NDFileIMM_num_imm_pixels,imm_pixels);



        setIntegerParam(NDFileIMM_is_already_imm,0);
        setIntegerParam(NDFileIMM_imm_systicks,file_systick_ts);
        setIntegerParam(NDFileIMM_imm_corecoticks, file_coreco_ts );
        setIntegerParam(NDFileIMM_imm_dlen,cf->last_nbytes );
        setDoubleParam(NDFileIMM_imm_elapsed,file_elapsed_ts);
        



		this->nextRecord++;
	getIntegerParam(NDFileNumber,&fnx);
		if (is_update==1)
			setIntegerParam(NDFileNumber,fnx+1);

		 if (is_throw_frame)
		 {
			  if (cf->last_nbytes<=my_array->dataSize)
			  {
				 memcpy(my_array->pData, cf->last_data, cf->last_nbytes);
				// my_array->dims[0].size=cf->last_nbytes;
			 //my_array->dims[1].size=1;
			// my_array->dataType= NDUInt8;

					/* Get the attributes for this driver */
					this->getAttributes(my_array->pAttributeList);


			doCallbacksGenericPointer(my_array, NDArrayData, 0);		}
			else
			{
					printf("Error- Could not throw IMM to plugins- not enough mem\n");
					   setIntegerParam(NDFileWriteStatus, NDFileWriteError);
					   setStringParam(NDFileWriteMessage, "Error- Could not throw IMM to plugins- not enough mem.");

			}


			callParamCallbacks();


		 }//if (is_throw_frame)




	}

    setIntegerParam(NDFileIMM_fileevent,0);
	setIntegerParam(NDFileNumCaptured,this->nextRecord);
    getIntegerParam(NDFileNumCapture,&ii0);
    
    // if num to captuer is -1 we wioll run forever.
	if ((this->nextRecord >= ii0) && (ii0 != -1))
	{
		setIntegerParam(NDFileNumCaptured,ii0);
		closeFile();
		setIntegerParam(NDFileCapture, 0);
	}

    callParamCallbacks();
	return(asynSuccess);
}
/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void NDFileIMM::processCallbacks(NDArray *pArray)
{
	bool is_at_msg;
	double fr_prd;
	/*

	,
	
	,
	,

	*/

	//getAttributes(pArray);

	is_at_msg=false;
	threshold = 0;
	is_imm_comp = 0;
	acq_time = 0.0;

    is_already_imm=0;
    pipe_num_shorts=0;

// dbpf "SimMaddog:IMM:EnableCallbacks","Enable"

	getIntegerParam(NDFileIMM_threshold,&threshold);

	setDoubleParam(NDFileIMM_timestamp,pArray->timeStamp);
	setIntegerParam(NDFileIMM_uniqueID,pArray->uniqueId);




	general_timestamp=pArray->timeStamp;
	fr_prd=general_timestamp-last_timestamp;
	if (fr_prd<0.0)
		fr_prd=0.0-fr_prd;


	if ( (pArray->uniqueId - last_unique_id) > 1)
	{
		setIntegerParam(
			NDFileIMM_NmissedIDs,
			getIntParam(NDFileIMM_NmissedIDs)+1);
	}


	if (fr_prd > 1.5*getDoubParam(NDFileIMM_framePeriod))
	{
		setIntegerParam(
			NDFileIMM_NmissedTimeStamps,
			getIntParam(NDFileIMM_NmissedTimeStamps)+1);
	}

	setDoubleParam(NDFileIMM_framePeriod,fr_prd);

	last_unique_id=pArray->uniqueId;

	if (timestamp_reset_counter>getIntParam(NDFileIMM_Nimg_rst_ts))
	{
		timestamp_reset_counter=0;
		setIntegerParam(NDFileIMM_NmissedTimeStamps,0);
		setIntegerParam(NDFileIMM_NmissedIDs,0);

	}

	timestamp_reset_counter++;
	last_timestamp=general_timestamp;

	
	numAttributes = pArray->pAttributeList->count();

//	printf("Num Attributes %i \n", numAttributes);
	pAttribute = pArray->pAttributeList->next(NULL);


	if (getIntParam(NDFileIMM_printAttributes))
		is_at_msg=true;

	for (attrCount=0; attrCount<numAttributes; attrCount++)
	{

        pAttribute->getValueInfo(&attrDataType, &attrSize);
        strcpy(name, pAttribute->getName());
        strcpy(description, pAttribute->getDescription());
                // pAttribute->getDescription(description, sizeof(description));
		//pAttribute->getValue(attrDataType, void *pValue, attrSize);

		// imm_threshold
		// imm_acq_time
		// imm_coreco_ticks

		if (is_at_msg)
		{
			printf("Attr: %s \n",name);


		}



		if (strcmp(name,"imm_threshold")==0)
		{
			pAttribute->getValue(attrDataType, (void*)&threshold, attrSize);
			if (is_at_msg)
				printf("imm_threshold = %i\n", threshold);
		}

	

        if (strcmp(name,"is_already_imm")==0)
        {
            pAttribute->getValue(attrDataType, (void*)&is_already_imm, attrSize);
            if (is_at_msg)
                printf("is_already_imm = %d\n", is_already_imm);
        }
     


	        pAttribute = pArray->pAttributeList->next(pAttribute);
		// pAttribute = pArray->nextAttribute(pAttribute);

	}

	setIntegerParam(NDFileIMM_is_imm_comp,is_imm_comp);
        setIntegerParam(NDFileIMM_threshold,threshold);
	


	imm_pixels=0;


    if (is_already_imm)
    {
        compressed_header *immh = (compressed_header*)(pArray->pData);
        //corecoticks=immh->corecotick;
        //setIntegerParam(NDFileIMM_fileSysticks,immh->systick);
        //setIntegerParam(NDFileIMM_fileElapsed,immh->elapsed);
     
      
        if (immh->compression!=0)
            setIntegerParam(NDFileIMM_is_imm_comp,1);
        else
            setIntegerParam(NDFileIMM_is_imm_comp,0);

        imm_pixels=immh->dlen;
        
        
              
        setIntegerParam(NDFileIMM_is_already_imm,1);
        setIntegerParam(NDFileIMM_imm_systicks,immh->systick);
        setIntegerParam(NDFileIMM_imm_corecoticks, immh->corecotick );
        setIntegerParam(NDFileIMM_imm_dlen,immh->dlen );
        setDoubleParam(NDFileIMM_imm_elapsed,immh->elapsed);
        
        
        
    }
   

	

	
	// call base class function...
	NDPluginFile::processCallbacks(pArray);

	setIntegerParam(NDFileIMM_num_imm_pixels,imm_pixels);
	

    callParamCallbacks();
}

asynStatus NDFileIMM::readFile(NDArray **pArray)
{
    //static const char *functionName = "readFile";

    return asynError;
}


asynStatus NDFileIMM::closeFile()
{
    static const char *functionName = "closeFile";
	int fnx;
	int is_update;

    printf("IMM closeFile\n");
		cf->close();


			return asynSuccess;
}





/** Called when asyn clients call pasynOctet->write().
 * Catch parameter changes.  If the user changes the path or name of the template file
 * load the new template file.
 * \param[in] pasynUser pasynUser structure that encodes the reason and address.
 * \param[in] value Address of the string to write.
 * \param[in] nChars Number of characters to write.
 * \param[out] nActual Number of characters actually written. */
asynStatus NDFileIMM::writeOctet(asynUser *pasynUser, const char *value,
      size_t nChars, size_t *nActual)
{
   int addr = 0;
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   const char *functionName = "writeOctet";
char pathstr[512];

   status = getAddress(pasynUser, &addr);
   if (status != asynSuccess)
      return (status);
   /* Set the parameter in the parameter library. */
   status = (asynStatus) setStringParam(addr, function, (char *) value);

   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
  
    if (function == NDFilePath) 
    {

			int is_makedirs;
			int statx;
			
			getStringParam(NDFilePath, sizeof(pathstr), pathstr);
			
			statx = recursePath(pathstr, true);
			printf("IMM Recurse path: statis = %d\n",statx);

			if (statx==0)
			{
				setIntegerParam(NDFilePathExists, 1);
				setIntegerParam(NDFileWriteStatus, NDFileWriteOK);
				
			}
			else
			{
				setIntegerParam(NDFilePathExists, 0);
				setIntegerParam(NDFileWriteStatus, NDFileWriteError);
			}


    } 
   
  
      /* If this parameter belongs to a base class call its method */
      status = NDPluginFile::writeOctet(pasynUser, value, nChars, nActual);
   

   /* Do callbacks so higher layers see any changes */
   status = (asynStatus) callParamCallbacks(addr, addr);

   if (status)
      epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
            "%s:%s: status=%d, function=%d, value=%s", driverName, functionName,
            status, function, value);
   else
      asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
            "%s:%s: function=%d, value=%s\n", driverName, functionName,
            function, value);
   *nActual = nChars;
   return status;
}




// searches path part by part.
// path is of the form part/part/part/part
// return
// -64 if drive not exist.
// -1 if 1st path not exist, -2 if 2md part not exist.
// -128| part if cannot create dir. ex -128-2 if cant create 2nd part.

int NDFileIMM::recursePath(char *pathstr, bool is_makedirs)
{


	struct _stat buff;
	int status;
	int slash_pos=0;
	int last_slash_pos = 0;
	int start = 0;
	int part_number = 1;

	string part_path, path_word;

	if (strlen(pathstr)==0)
		return(-1);

	string full_path(pathstr);

	// make sure last char is /

	
	if (full_path[full_path.size()-1] != '/')
		full_path.append("/",1);

    //search thru path name part by part. if start w/ / then take next /

    if (full_path[0]!='/')
        slash_pos = full_path.find(string("/"),0);
    else
        slash_pos = full_path.find(string("/"),1);


	while (slash_pos!=string::npos)
	{
		// path up to / not incl /
		//here we are actually calling the copy constructor...of part path. The returned str from substr?
		// prob in memory inside fullpath,,, it will go out of scope and destruct?
		part_path = full_path.substr(start, slash_pos);
	

		// this part of path... say path is D:/aaa/bbb/ccc   then worrd is bbb for eample
        // if loas_slash_pos is 0, then the path starts with /. this is special case. make sure pathword has the 1st /
        if (full_path[last_slash_pos] == '/')
			path_word = full_path.substr(last_slash_pos+1, (slash_pos -last_slash_pos-1));
		else
			path_word = full_path.substr(last_slash_pos, (slash_pos  -last_slash_pos));


		//check if we are looking at a drive, check for : at pos 1
		if (path_word[1] == ':')
		{
			//a drive
			//somehow make sure drive exists.

			// for drive need / at end.
			part_path.append("/",1);
			status = _stat(part_path.c_str(), &buff);
			if (status!=0)
				return(-64-part_number);



			// if not exist return(-64 -part_num);
		}
		else
		{

			status = _stat(part_path.c_str(), &buff);

			setStringParam(NDFileWriteMessage, "INFO-Directory Exists");
			
			if (!is_makedirs)
			{
				if (status!=0)
					return(0-part_number);
			}
			else
			{
				if (status!=0)
				{
					
#ifdef _WIN32					
					status= _mkdir(part_path.c_str());
#else
					umask(0777);	
				    int mode=(int)umask(0777);
					status= mkdir(part_path.c_str(),0777);
					int chstat=_chmod(part_path.c_str(),0777);
					printf("mkdir umask %0o chmod ret %d\n ", mode, chstat);

#endif

					if (status!=0)
					{
						printf("ERROR: Could Not Dir %s\n",part_path.c_str());
						setStringParam(NDFileWriteMessage, "ERROR-Cound not create Directory");
						
						return(-128-part_number);
					}
					else
						printf("Created Dir %s\n",part_path.c_str());
						setStringParam(NDFileWriteMessage, "INFO-Created New Directory");

				}
			}


		}

		//search thru path name part by part
		last_slash_pos = slash_pos;
		slash_pos = full_path.find(string("/"),slash_pos+1);
		part_number++;

	}

	return(0);


}
















/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvNDFileIMMConfigure(const char *portName, int max_imm_bytes, int queueSize, int blockingCallbacks,
                                   const char *NDArrayPort, int NDArrayAddr,
                                   int priority, int stackSize)
{
  NDFileIMM *pPlugin =  new NDFileIMM(portName,max_imm_bytes, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                       priority, stackSize);
  //  return(asynSuccess);
  return pPlugin->start();

}
/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg01 = { "max_imm_bytes",iocshArgInt};
static const iocshArg initArg1 = { "frame queue size",iocshArgInt};
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt};
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString};
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt};
static const iocshArg initArg5 = { "priority",iocshArgInt};
static const iocshArg initArg6 = { "stackSize",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
                                            &initArg01,
                                            &initArg1,                                           
                                            &initArg2,
                                            &initArg3,
                                            &initArg4,
                                            &initArg5,
                                            &initArg6};
static const iocshFuncDef initFuncDef = {"NDFileIMMConfigure",8,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  drvNDFileIMMConfigure(args[0].sval,args[1].ival, args[2].ival, args[3].ival,
                       args[4].sval,args[5].ival, 
                       args[6].ival, args[7].ival);
}

extern "C" void NDFileIMMRegister(void)
{
  iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
  epicsExportRegistrar(NDFileIMMRegister);
}

/* The constructor for this class */
//max_imm_bytes is size of NDArray thrown, 1 x IMMLength bytes
NDFileIMM::NDFileIMM(const char *portName,int max_imm_bytes ,int queueSize, int blockingCallbacks,
                       const char *NDArrayPort, int NDArrayAddr,
                       int priority, int stackSize) :
     NDPluginFile(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, 
                   500, 0, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize,10)



{
	int i;

    this->max_imm_bytes = max_imm_bytes;
	is_open_good=true;

		paramStrings[0]=new param_type_str(&NDFileIMM_threshold,asynParamInt32,"NDFileIMM_threshold");
		paramStrings[1]=new param_type_str(&NDFileIMM_is_imm_comp,asynParamInt32, "NDFileIMM_is_imm_comp");
		paramStrings[2]=new param_type_str(&NDFileIMM_num_imm_pixels,asynParamInt32,"NDFileIMM_num_imm_pixels");
		paramStrings[3]=new param_type_str(&NDFileIMM_timestamp,asynParamFloat64,"NDFileIMM_timestamp");
		paramStrings[4]=new param_type_str(&NDFileIMM_uniqueID,asynParamInt32,"NDFileIMM_uniqueID");
		paramStrings[5]=new param_type_str(&NDFileIMM_printAttributes,asynParamInt32,"NDFileIMM_printAttributes");
		paramStrings[6]=new param_type_str(&NDFileIMM_NmissedTimeStamps,asynParamInt32,"NDFileIMM_NmissedTimeStamps");
		paramStrings[7]=new param_type_str(&NDFileIMM_framePeriod,asynParamFloat64,"NDFileIMM_framePeriod");
		paramStrings[8]=new param_type_str(&NDFileIMM_NmissedIDs,asynParamInt32,"NDFileIMM_NmissedIDs");

		paramStrings[9]=new param_type_str(&NDFileIMM_Nimg_rst_ts,asynParamInt32,"NDFileIMM_Nimg_rst_ts");
		paramStrings[10]=new param_type_str(&NDFileIMM_throw_images,asynParamInt32,"NDFileIMM_throw_images");

        paramStrings[11]=new param_type_str(&NDFileIMM_fileevent,asynParamInt32,"NDFileIMM_fileevent");


         paramStrings[12]=new param_type_str(&NDFileIMM_is_already_imm ,asynParamInt32,"NDFileIMM_is_already_imm");
         paramStrings[13]=new param_type_str(&NDFileIMM_imm_systicks   ,asynParamInt32,"NDFileIMM_imm_systicks");
         paramStrings[14]=new param_type_str(&NDFileIMM_imm_corecoticks,asynParamInt32,"NDFileIMM_imm_corecoticks");
         paramStrings[15]=new param_type_str(&NDFileIMM_imm_elapsed    ,asynParamFloat64,"NDFileIMM_imm_elapsed");
         paramStrings[16]=new param_type_str(&NDFileIMM_imm_dlen       ,asynParamInt32,"NDFileIMM_imm_dlen");


        setIntegerParam(NDFileIMM_is_already_imm , 0);
        setIntegerParam(NDFileIMM_imm_systicks   , 0);
        setIntegerParam(NDFileIMM_imm_corecoticks, 0);
        setDoubleParam(NDFileIMM_imm_elapsed    ,0 );
        setIntegerParam(NDFileIMM_imm_dlen       , 0);


    this->supportsMultipleArrays = 1;
    this->pAttributeId = NULL;

    	cf= new compressed_file();

   	for (i=0;i<num_params;i++)
	{

		  createParam(
		    paramStrings[i]->str_ptr,
		  	(asynParamType)(paramStrings[i]->param_type),
		  	paramStrings[i]->int_ptr);

		  	 printf("Create %d Param: Str %s  , Param %d \n",
		  	 	paramStrings[i]->param_type,
				paramStrings[i]->str_ptr,
		  		*(paramStrings[i]->int_ptr));

	}

		setIntegerParam(NDFileIMM_threshold, 0);

		last_filenumber=0;

		is_coreco_timestamp=false;
		general_timestamp=0.0;



			setIntegerParam(NDFileIMM_printAttributes,0);

			setIntegerParam(NDFileWriteMode,NDFileModeStream);
			setIntegerParam(NDFileIMM_throw_images,0);
	setIntegerParam(NDFileIMM_fileevent,0);
	// fileevent is 0 for no event, 1 for file opened, 1st in series

	last_unique_id=0;
	last_timestamp=0.0;
	timestamp_reset_counter=0;


	my_array = 0;


    /* Set the plugin type string */
    setStringParam(NDPluginDriverPluginType, "NDFileIMM");

    /* Try to connect to the array port */
    connectToArrayPort();

}




/* NDPluginGeDebug.cpp
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
#include "NDPluginGeDebug.h"
#include "drvNDPluginGeDebug.h"
// #include "tiffio.h"
#include "asynNDArrayDriver.h"
#include <ADCoreVersion.h>

#ifdef _WIN32
#include <io.h>
#endif
static const char *driverName = "NDPluginGeDebug";


/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, \
                "%s:%s error=%s\n", \
                driverName, functionName, nc_strerror(e)); \
                return(asynError);}

/* NDArray string attributes can be of any length, but netCDF requires a fixed maximum length
 * which we define here. */
#define MAX_ATTRIBUTE_STRING_SIZE 256

/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void NDPluginGeDebug::processCallbacks(NDArray *pArray)
{
	bool is_at_msg;
	double fr_prd;
	char name[256];
	char description[256];
	int ival;
	NDAttrDataType_t attrDataType;
	
	numAttributes = pArray->pAttributeList->count();

	printf("Num Attributes %i \n", numAttributes);
	pAttribute = pArray->pAttributeList->next(NULL);


	//if (getIntParam(NDPluginGeDebug_printAttributes))
		is_at_msg=true;

	for (attrCount=0; attrCount<numAttributes; attrCount++)
	{

        pAttribute->getValueInfo(&attrDataType, &attrSize);
        strcpy(name, pAttribute->getName());
        strcpy(description, pAttribute->getDescription());
	attrDataType = pAttribute->getDataType();
        pAttribute->getValue(attrDataType, (void*)&ival,0);


                // pAttribute->getDescription(description, sizeof(description));
		//pAttribute->getValue(attrDataType, void *pValue, attrSize);


		if (is_at_msg)
		{
			printf("Attr: %s Val %d \n",name,ival);


	
	}


	        pAttribute = pArray->pAttributeList->next(pAttribute);
		// pAttribute = pArray->nextAttribute(pAttribute);
	}
	// call base class function...
//	NDPluginDriver::processCallbacks(pArray);
    callParamCallbacks();
}



/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvNDPluginGeDebugConfigure(const char *portName, int max_imm_bytes, int queueSize, int blockingCallbacks,
                                   const char *NDArrayPort, int NDArrayAddr,
                                   int priority, int stackSize)
{
  NDPluginGeDebug *pPlugin =  new NDPluginGeDebug(portName,max_imm_bytes, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
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
static const iocshFuncDef initFuncDef = {"NDPluginGeDebugConfigure",8,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  drvNDPluginGeDebugConfigure(args[0].sval,args[1].ival, args[2].ival, args[3].ival,
                       args[4].sval,args[5].ival, 
                       args[6].ival, args[7].ival);
}

extern "C" void NDPluginGeDebugRegister(void)
{
  iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
  epicsExportRegistrar(NDPluginGeDebugRegister);
}

/* The constructor for this class */
//max_imm_bytes is size of NDArray thrown, 1 x IMMLength bytes
#if ADCORE_VERSION>2
NDPluginGeDebug::NDPluginGeDebug(const char *portName,int max_imm_bytes ,int queueSize, int blockingCallbacks,
                       const char *NDArrayPort, int NDArrayAddr,
                       int priority, int stackSize) :
    
       NDPluginDriver(portName, queueSize, blockingCallbacks,
                  NDArrayPort,  NDArrayAddr, 1,
                 500,0,  asynGenericPointerMask, asynGenericPointerMask,
                  ASYN_CANBLOCK, 1, priority, stackSize,1)
    
    
    

#else
NDPluginGeDebug::NDPluginGeDebug(const char *portName,int max_imm_bytes ,int queueSize, int blockingCallbacks,
                       const char *NDArrayPort, int NDArrayAddr,
                       int priority, int stackSize) :
     NDPluginDriver(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, num_params,
                   500, 0, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize)
#endif
{
	int i;

//    this->supportsMultipleArrays = 1;
    this->pAttributeId = NULL;

    /* Try to connect to the array port */
    connectToArrayPort();

}


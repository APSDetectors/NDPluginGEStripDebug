/*
 * drvNDPluginGeDebug.h
 *
 * Asyn driver for callbacks to write data to IMM files for area detectors.
 *
 * Author: Tim Madden (thanks to J. Hammonds, B. Tieman)
 *
 * Created Nov 4, 2009
 */

#ifndef DRV_NDPluginGeDebug_H
#define DRV_NDPluginGeDebug_H

#ifdef __cplusplus
extern "C" {
#endif

int drvNDPluginGeDebugConfigure(const char *portName,int max_imm_bytes, int queueSize, int blockingCallbacks,
                             const char *NDArrayPort, int NDArrayAddr,
                             int priority, int stackSize);

#ifdef __cplusplus
}
#endif
#endif

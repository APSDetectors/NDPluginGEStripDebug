
"""

iname = '/local/maddog_00000-00999.imm'


#How to use
#run ipython

execfile('immcheck.py')

checkFile(iname)


checkFile(iname,is_plot=True)


checkFile(iname,is_image=True)

checkFile(iname)


P='13SIM1:'
Rlist = ['IMM:','IMM2:','IMM3:']

CaMultiPut(P,Rlist,'Capture',1)

CaMultiPut(P,Rlist,'Capture',0)

CaMultiPut(P,Rlist,'FileFormat',0)

CaMultiPut(P,Rlist,'FileFormat',1)


#to read headers of images

fp = open(iname,'r')

h = readHeader(fp)
print h
#call if you are skipping iamge data, and not reading iamge data.
getNextHeaderPos(fp,h)

h = readHeader(fp)
print h
getNextHeaderPos(fp,h)


clf()
h = readHeader(fp)
img = getImage(fp,h)
#do not call getNextHeader. getImage seeks to next header already
print h
#displauy image
figimage(img)


clf()
h = readHeader(fp)
img = getImage(fp,h)
#do not call getNextHeader. getImage seeks to next header already
print h
#displauy image
figimage(img)



#etc ..\
#raw IMM

iname = '/local/testyraw_00100-00199.imm'

checkFile(iname)

fp.close()

"""




import struct


#comment these ouit of not drawing images, but only reading headers"
import numpy as np
import matplotlib.pyplot as plt
import epics






imm_headformat = "ii32s16si16siiiiiiiiiiiiiddiiIiiI40sf40sf40sf40sf40sf40sf40sf40sf40sf40sfffiiifc295s84s12s"

imm_fieldnames = [
'mode',
'compression',
'date',
'prefix',
'number',
'suffix',
'monitor',
'shutter',
'row_beg',
'row_end',
'col_beg',
'col_end',
'row_bin',
'col_bin',
'rows',
'cols',
'bytes',
'kinetics',
'kinwinsize',
'elapsed',
'preset',
'topup',
'inject',
'dlen',
'roi_number',
'buffer_number',
'systick',
'pv1',
'pv1VAL',
'pv2',
'pv2VAL',
'pv3',
'pv3VAL',
'pv4',
'pv4VAL',
'pv5',
'pv5VAL',
'pv6',
'pv6VAL',
'pv7',
'pv7VAL',
'pv8',
'pv8VAL',
'pv9',
'pv9VAL',
'pv10',
'pv10VAL',
'imageserver',
'CPUspeed',
'immversion',
'corecotick',
'cameratype',
'threshhold',
'byte632',
'empty_space',
'ZZZZ',
'FFFF'

]





def checkFile(fname,is_image=False,is_plot=False):
    fp = open(fname,'rb')

    lastcor=-1
    lastbn = -1;
    n_corerror = 0
    n_bnerror = 0

    systicks=[]
    elapseds = []
    buffer_numbers = []
    corecoticks = []
    
    while True:
        h = readHeader(fp)
        if h!='eof':
            print '_________________________________________'
            print 'buffer number %d'%h['buffer_number']
            print 'corecotick %d'%h['corecotick']
            print 'elapsed %f'%h['elapsed']
            print 'systick %f'%h['systick']
            print 'dlen %f'%h['dlen']
            print 'bytes %f'%h['bytes']
            print 'compression %f'%h['compression']

            systicks.append(h['systick'])
            elapseds.append(h['elapsed'])
            buffer_numbers.append(h['buffer_number'])
            corecoticks.append(h['corecotick'])
            
            

            if lastbn==-1: lastbn = h['buffer_number']-1
            if lastcor==-1: lastcor = h['corecotick']-1

            dbn = h['buffer_number'] - lastbn
            dcor = h['corecotick'] - lastcor

            if dbn>1: n_bnerror=n_bnerror+1

            if dcor>1: n_corerror = n_corerror+1


            lastbn = h['buffer_number']
            lastcor = h['corecotick']

        
            if is_image:
                img = getImage(fp,h)
                figure(1)
                clf()
                ion()
                figimage(img)
                pause(0.001)

            else:
                getNextHeaderPos(fp,h)
    
    
        else: break

    print "Skipped Buffer numbers %d"%n_bnerror
    print "Skipped Corecoticks %d"%n_corerror

    fp.close()

    if is_plot:
        figure(2)
        clf()
        subplot(4,1,1)
        plot(systicks)
       
        
        subplot(4,1,2)
        plot(elapseds)
      

        subplot(4,1,3)
        plot(buffer_numbers)
       

        subplot(4,1,4)
        plot(corecoticks)
      
        
        
        figure(3)
        clf()
        subplot(4,1,1)
       
        plot(diff(systicks))
        
        subplot(4,1,2)
      
        plot(diff(elapseds))

        subplot(4,1,3)
       
        plot(diff(buffer_numbers))

        subplot(4,1,4)
       
        plot(diff(corecoticks))
        
    

def readHeader(fp):
    bindata = fp.read(1024)
    if bindata=='':
        return('eof')

    imm_headerdat = struct.unpack(imm_headformat,bindata)
    imm_header ={}
    for k in range(len(imm_headerdat)):
        imm_header[imm_fieldnames[k]]=imm_headerdat[k]
    return(imm_header)


def getNextHeaderPos(fp,header):
    dlen = header['dlen']
    bytes = header['bytes']
    
    if header['compression']==6:
        fp.seek((dlen*4) + (dlen*bytes),1)
    else:
        fp.seek(dlen*bytes,1)




#getImage requres numpy, comment out of no numpy


def getImage(fp,h):
    dlen = h['dlen']

    if h['compression']==6:
        loc_b = fp.read(4*dlen)
        pixloc = struct.unpack('%di'%dlen,loc_b)

        if h['bytes']==2:
            val_b = fp.read(2*dlen)
            pixval = struct.unpack('%dH'%dlen,val_b)

        if h['bytes']==1:
            val_b = fp.read(1*dlen)
            pixval = struct.unpack('%dB'%dlen,val_b)

        imgdata = np.array( [0] * (h['rows'] * h['cols']))

        for k in range(dlen):
            imgdata[ pixloc[k] ] = pixval[k]

    else:
        pixdat=fp.read(h['bytes']*dlen)
        if h['bytes']==2:       
            pixvals=struct.unpack('%dH'%dlen,pixdat)
    
        if h['bytes']==1:        
            pixvals=struct.unpack('%dB'%dlen,pixdat)

        imgdata=np.array(pixvals)



    imgdata = imgdata.reshape(h['rows'], h['cols'])

    return(imgdata)



# CaMultiPut(P,Rlist,'Capture',1)
# CaMultiPut(P,Rlist,'FileFormat',0)

def CaMultiPut(P,Rlist,Pname, val):
    for R in Rlist:
        PV = P + R + Pname
        epics.caput(PV,val)
        
        
        


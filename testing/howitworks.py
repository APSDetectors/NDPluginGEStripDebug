

def main():
    global scatterQ,imm0Q,imm1Q,imm2Q,gatherQ
    
    scatterQ = queue()
    imm0Q = queue()
    imm1Q = queue()
    imm2Q = queue()
    imm3Q = queue()
    gatherQ = queue()
    

    runonthread(driver,None,scatterQ)
    runonthread(scatter,scatterQ,[imm0Q,imm1Q,imm2Q])

    runonthread(immplugin,imm0Q,gattherQ)
    runonthread(immplugin,imm1Q,gatherQ)
    runonthread(immplugin,imm2Q,gatherQ)


    runonthread(gather,gatherQ,imm3Q)    
    runonthread(immplugin,imm3Q,None)


    
    if is_sort:
        mylist = linkedlist()
        runonthread(sorter,mylist,imm3Q)
        
        
   
    
    while(1): pass
    
    
    
    

def driver(Qin,Qout):
    while(1):
        img=makeimage()
        Qout.enqueue(img)
        





def scatter(Qin,Qlistout):
    n = 0
    while(1):
        #block until image is there
        img = Qin.dequeue() 
        
        Qlistout[n].enqueue(img)
        n = (n+1)%Qlistout.len()
        
        
        


def immplugin(Qin, Qout):
    while(1):
        img1 = Qin.dequeue()
        img2 = createIMMFormat(img)
        if is_save: save(img2)
        if is_throwimages:
            Qout.enqueue(img2)
            
            

def gather(Qin,Qout)        
    while(1):
        img = Qin.dequeue()
        if is_sort:
            linkedlist.insert(img)
            sorterThread.notify()
            
        else:
            Qout.enqueue(img)
            
            
    
    
    
    
def sorter(listin,Qout):
    toc = 0
    
    while(1):
        sleep_until_notify()
        tic = current_time()
        
        
        if listin.len()>0:
            
            listin.sort()
            
            for img in listin:
                if listin.isInOrder(img):
                    Qout.enqueue(img)
                    listin.remove(img)
                elif tic-toc>sorttime:
                    Qout.enqueue(img)
                    listin.remove(img)
                    
                    
        toc = tic
        
                      
    
    
    
    
    
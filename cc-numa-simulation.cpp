#include <stdio.h>
#include <iostream>
#include <cmath>
using namespace std;

//Clock cycle count variable
int clockCycles = 0;

//Function to add clock cycles
void addClockCycles(int x) {
    clockCycles += x;
}

//Convert decimal to binary
int* decToBin(int n) {
   int m = 5;
   static int binaryNumber[6] = {0,0,0,0,0,0};
   int num = n;
   int i = 0;
   while (m >= 0) {
      binaryNumber[m] = n % 2;
      n = n / 2;
      m--;
   }

   return binaryNumber;
}


/*
Class Name: Memory
Class Description: This represents one memory line. It consists of memory address and content.
*/
class Memory {
    int mem_addr;
    int content;
    int tag = 0;
    int index = 0;
    int *binMemAddr;

    public:
        Memory() {}
        Memory(int n) {
            int i = 0, j = 5, a;
            mem_addr = n;
            content = n + 5;
            binMemAddr = decToBin(n);

            while(i < 2) {
                a = *(binMemAddr + j--);
                index = index + a*pow(2,i++);
            }

            i = 0;
            while(i < 4) {
                a = *(binMemAddr + j--);
                tag = tag + a*pow(2,i++);
            }
        }

        //Check if memory tag is equal to argument passed
        bool verifyMemTag(int t) {
            if(t == tag) {
                return true;
            } else {
                return false;
            }
        }

        //Return content of the memory
        int getMemoryData() {
            return content;
        }

        //Update content of the memory
        void putMemoryData(int data) {
            content = data;
        }

        //Display address and content of the memory
        void displayMemoryData() {
            cout<<"Memory: "<<mem_addr<<", "<<content<<endl;
        }
};


//Global variable memory
Memory mem[64];


/*
Class Name: Directory
Class Description: This represents one directory line. It consists of two status field followed by 4 bits representing each node.
*/
class Directory {
    int status_field[2];
    int p[4];
    int name;

    public:
        Directory() {}
        Directory(int n) {
            int i;
            name = n;
            status_field[0] = 0;
            status_field[1] = 0;
            for(i=0;i<4;i++) {
                p[i] = 0;
            }
        }

        //Check the status field of the directory. Return true if uncached or shared
        bool checkStatusField() {
            if( ( (status_field[0] == 0) && (status_field[1] == 0) ) || ( (status_field[0] == 0) && (status_field[1] == 1) ) ) {
                return true;
            } else {
                return false;
            }
        }

        //Set the status field of the directory
        void setStatusField(int a, int b) {
            status_field[0] = a;
            status_field[1] = b;
        }
        
        //Set the status of each node in the directory
        void setStatus(int node) {
            int i;
            p[node] = 1;
        }

        //Get status of the directory
        string getStatus() {
            if( (status_field[0] == 0) && (status_field[1] == 1) ) {
                return "shared";
            } else if((status_field[0] == 1) && (status_field[1] == 1)) {
                return "dirty";
            } else if((status_field[0] == 0) && (status_field[1] == 0)) {
                return "uncached";
            }
        }

        //Get nodes if shared
        int* getSharedNodes() {
            if( (status_field[0] == 0) && (status_field[1] == 1) ) {
                return p;
            }
        }

        //Get nodes if dirty
        int* getDirtyNodes() {
            if( (status_field[0] == 1) && (status_field[1] == 1) ) {
                return p;
            }
        }

        //Display the directory bits
        void displayDirectoryData() {
            cout<<"Directory: "<<status_field[0]<<", "<<status_field[1]<<", "<<p[0]<<", "<<p[1]<<", "<<p[2]<<", "<<p[3]<<endl;
        }
};


/*
Class Name: Cache
Class Description: This represents one cache line. It consists of a valid bit, tag and data.
*/
class Cache {
    int valid_bit;
    int boolTag[4];
    int tag;
    int data = -1;
    int name;
    
    public: 
        Cache() {}
        Cache(int n) {
            name = n; 
            valid_bit = 0;
        }

        //Check if tag is equal to the argument passed.
        bool isCache(int t) {
            if((tag == t) && (valid_bit == 1)) {
                return true;
            } else {
                return false;
            }
        }

        //Return cache data
        int getCacheData() {
            return data;
        }

        //Set cache data. Change valid bit to 1.
        void setCacheData(int d) {
            valid_bit = 1;
            data = d;
        }

        //Return cache valid bit
        int getValidBit() {
            return valid_bit;
        }

        //Update cache valid bit if tag is equal to argument passed.
        void updateValidBit(int vb, int t) {
            if(tag == t) {
               valid_bit = 0;
            }
        }

        //Check if data is updated and available
        bool isAvail() {
            if(data == -1) {
                return true;
            } else {
                return false;
            }
        }

        //Update cache data, tag and valid bit.
        void putCacheData(int d, int t) {
            data = d;
            valid_bit = 1;
            tag = t;
        }

        //Display cache data
        void displayCacheData() {
            cout<<"Cache: "<<valid_bit<<", "<<tag<<", "<<data<<endl;
        }
};


/*
Class Name: ScalarProc
Class Description: This represents one Scalar Processor. It consists of two registers s1 and s2 along with a local cache.
*/
class ScalarProc { 
    int s1;
    int s2;
    int name;
    Cache cache[4];

    public:
        ScalarProc() {}
        ScalarProc(int n) {
            int i;
            name = n; 
            s1 = 0;
            s2 = 0;
            
            for(i=0;i<4;i++) {
                cache[i] = Cache(i);
            }
        }

        //Return cache position if tag is found
        int searchCache(int tag) {
            int i;
            for(i=0;i<4;i++) {
                if(cache[i].isCache(tag)) {
                    return i;
                }
            }
            return -1;
        }

        //Get cache data
        int retrieveCache(int id) {
            return cache[id].getCacheData();
        }

        //Update register and cache with data argument
        void putRegData(int data, string reg, int tag) {
            int i, flag = 0;
            if(reg == "s1") {
                s1 = data;
            } else if(reg == "s2") {
                s2 = data;
            }

            for(i=0;i<4;i++) {
                if(cache[i].isAvail()) {
                    cache[i].putCacheData(data, tag);
                    flag = 1;
                    break;
                }
                if(flag == 0) {
                    //replace
                }
            }
        }

        //Update cache with data parameter
        void updateCache(int id, int data) {
            cache[id].setCacheData(data);
        }

        //Retrieve cache data with valid bit
        int getValidCache() {
            int i;
            for(i=0;i<4;i++) {
                if(cache[i].getValidBit() == 1) {
                    return cache[i].getCacheData();
                }
            }
        }

        //Set cache that matches with tag as invalidate
        void invalidateCache(int tag) {
            int i;

            for(i=0;i<4;i++) {
                cache[i].updateValidBit(0,tag);
            }
        }

        //Display register and cache data in the processor
        void displayProcessorData() {
            int i;
            cout<<"S1: "<<s1<<endl;
            cout<<"S2: "<<s2<<endl;

            for(i=0;i<4;i++) {
                cache[i].displayCacheData();
            }
        }

        //Get register content
        int getData(string reg) {
            if(reg == "s1") {
                return s1;
            } else if(reg == "s2") {
                return s2;
            }
        }
};


/*
Class Name: SMPNodes
Class Description: This represents one SMP Node. It consists of two Scalar Processors, a Directory and Memory.
*/
class SMPNodes {        
    int setNumber, current;
    ScalarProc sp[2];
    Directory dir[16];
    public:
        SMPNodes() {}
        SMPNodes(int n) {
            int i;
            setNumber = n; 
            
            for(i=0;i<2;i++) {
                sp[i] = ScalarProc(i);
            }

            current = setNumber*16;
            for(i=current;i<(current+16);i++) {
                mem[i] = Memory(i);
            }

            for(i=0;i<16;i++) {
                dir[i] = Directory(i);
            }
        }

        //Check node set number
        int verifySetNumber(int setN) {
            if(setN == setNumber) {
                return true;
            } else {
                return false;
            }
        }

        //Check if cache containing data is present or not using tag and processor number. If yes, return cache data.
        int lookupCache(int tag, int cpu) {
            int id1, id2, ret = -1, flag = 0;

            id1 = lookupLocalCache(tag,cpu);
            if(id1 == -1) {
                cout<<"Not found in local cache. Going to secondary cache."<<endl;
                if(cpu == 0) {
                    id2 = lookupLocalCache(tag,++cpu);
                } else {
                    id2 = lookupLocalCache(tag,--cpu);
                }
            }
            if(id2 != -1) {
                ret = sp[cpu].retrieveCache(id2);
            } else {
                cout<<"Not found in secondary cache. Going to Memory."<<endl;
            }

            if(id1 != -1) {
                addClockCycles(1);
            } else if(id2 != -1) {
                addClockCycles(30);
            }

            return ret;
        }

        //Update cache with data
        void updateLocalCache(int id, int cpu, int data) {
            sp[cpu].updateCache(id, data);
        }

        //Updat valid bit to 0 for all cache that matches tag argument
        void invalidateAllCache(int tag) {
            int i = 0;

            for(i=0;i<2;i++) {
                sp[i].invalidateCache(tag);
            }
        }

        //Search for cache that matches tag argument 
        int lookupLocalCache(int tag, int cpu) {
            return sp[cpu].searchCache(tag);
        }

        //Check directory for uncached and shared nodes
        bool checkDir(int word_addr, int currentNode) {
            int dir_no, i;
            //dir_no = (word_addr/setNumber) + 1;
            dir_no = word_addr - (16*setNumber);
            return dir[dir_no].checkStatusField();
        }

        //Check memory for data
        int lookupMemory(int word_addr) {
            addClockCycles(100);
            return mem[word_addr].getMemoryData();
        }

        //Update content of memory with data argument
        void updateMemory(int word_addr, int data) {
            mem[word_addr].putMemoryData(data);
        }

        //Update register with data
        void loadRegNode(int data, int cpu, string reg, int tag) {
            sp[cpu].putRegData(data, reg, tag); 
        }

        //Update directory status and status fields
        void updateDirectory(int dir_no, int currentNode, string status) {
            int i;

            if(status == "shared") {
                dir[dir_no].setStatusField(0,1);
            } else if(status == "uncached") {
                dir[dir_no].setStatusField(0,1);
            } else if(status == "dirty") {
                dir[dir_no].setStatusField(1,1);
            }
            dir[dir_no].setStatus(currentNode);
        }

        //Update directory and cache
        void updateDirNcache(int dir_no, int tag) {
            int* sNodes;
            int i;
            string status;
            status = dir[dir_no].getStatus();
            if(status == "shared") {
                sNodes = dir[dir_no].getSharedNodes();
                for(i=0;i<4;i++) {
                    if(*(sNodes + i) == 1) {
                        sp[i].invalidateCache(tag);
                    }
                }
            } else if(status == "dirty") {
                sNodes = dir[dir_no].getDirtyNodes();
                for(i=0;i<4;i++) {
                    if(*(sNodes + i) == 1) {
                        sp[i].invalidateCache(tag);
                    }
                }
                dir[dir_no].setStatusField(0,1);
            }
        }

        //Get all dirty nodes using id passed as argument
        int* retrieveDirtyNodes(int nid) {
            int* sNodes;
            int i;

            sNodes = dir[nid].getDirtyNodes();
            return sNodes;
        }

        //Change status of directory from dirty to shared
        void dirtyToShared(int nid) {
            dir[nid].setStatusField(0,1);
        }

        //Get ID of dirty nodes
        int getDirtyNodesID() {
            int* sNodes;
            int i;

            for(i=0;i<16;i++) {
                if(dir[i].getStatus() == "dirty") {
                    return i;
                }
            }

            return -1;
        }

        //Retrieve data from valid cache
        int getValidCacheData() {
            int i, data;
            for(i=0;i<2;i++) {
                data = sp[i].getValidCache();
                if(data != -1) {
                    return data;
                }
            }
        }

        //Display all data present in the node
        void displayAllNodeData() {
            int i;
            for(i=0;i<16;i++) {
                dir[i].displayDirectoryData();
            }
            for(i=0;i<2;i++) {
                sp[i].displayProcessorData();
            }
            for(i=current;i<(current+16);i++) {
                mem[i].displayMemoryData();
            }
        }

        //Get data in the register
        int getRegData(int cpu, string reg) {
            return sp[cpu].getData(reg);
        }
};


/*
Class Name: CCNuma
Class Description: This represents cc-Numa machine. It processes the inputs and executes the operation on the assigned node.
*/
class CCNuma {
    SMPNodes smpn[4];
    int offset = 0;
    int word_addr;
    int node_set;
    int tag;
    int currentNode;
    int currentCPU;
    string cmd;
    string rs;
    string rt;

    public:
        int i = 0;

        CCNuma() {
            for(i=0;i<4;i++) {
                smpn[i] = SMPNodes(i);
                //cout<<i<<endl;
            }
        }

        //Return register name accoring to the argument code
        string getRegister(int reg) {
            switch(reg) {
                case 17 :
                    return "s1"; 
                    break;
                case 18 :
                    return "s2";
                    break;
                case 0 :
                    return "zero";
                    break;
                default :
                    return "Invalid register";
            }
        }

        //Return command accoring to opcode
        string getCommand(int opcode) {
            switch(opcode) {
                case 35 :
                    return "lw";
                    break;
                case 43 :
                    return "sw";
                    break;
                default :
                    return "Invalid opcode";
            }
        }

        //Return the node accorgint to the argument
        void selectNode(int node[], int cpu) {
            int len = 2, a = 0, i = 0;
            currentNode = 0;
            while(i < 2) {
                a = node[--len];
                currentNode = currentNode + a*pow(2,i++);
            }
            currentCPU = cpu;
        }

        //Find out registers, command, tag, address, node and word address from input bits passed as argument
        void processInput(int ip[]) {
            int len = 31;
            int a;
            int reg_s = 0;
            int reg_t = 0;
            int opcode = 0;
            offset = 0;

            i = 0;
            while(i < 16) {
                a = ip[len--];
                offset = offset + a*pow(2,i++);
            }
            
            word_addr = offset/4;
            node_set = word_addr%4;
            tag = word_addr/4;

            i = 0;
            while(i < 5) {
                a = ip[len--];
                reg_s = reg_s + a*pow(2,i++);
            }
            rs = getRegister(reg_s);

            i = 0;
            while(i < 5) {
                a = ip[len--];
                reg_t = reg_t + a*pow(2,i++);
            }
            rt = getRegister(reg_t);

            i = 0;
            while(i < 6) {
                a = ip[len--];
                opcode = opcode + a*pow(2,i++);
            }
            cmd = getCommand(opcode);
        }

        //Execute the command in the selected node and processor.
        void execute() {
            //search in local cache for each processor
            int data, tempNode, dir_no, i, nid, x, wa;
            int *dn;

            if(cmd == "lw") {
                data = smpn[currentNode].lookupCache(tag, currentCPU);

                //if not found, search for home node memory
                if(data == -1) {
                    tempNode = word_addr/16;
                    if(smpn[tempNode].checkDir(word_addr,currentNode)) {
                        data = smpn[tempNode].lookupMemory(word_addr);
                        cout<<"Search in Memory."<<endl;
                    } else {
                        cout<<"Memory marked dirty so search in Dirty Node."<<endl;
                        //Get dirty node
                        for(i=0;i<4;i++) {
                            nid = smpn[i].getDirtyNodesID();
                            if(nid != -1) {
                                x = i;
                                break;
                            }
                        }


                        //Update home memory and send data to reg
                        for(i=0;i<4;i++) {
                            if(*(dn + i) == 1) {
                                data = smpn[*(dn + i)].getValidCacheData();
                                break;
                            }
                        }

                        dn = smpn[x].retrieveDirtyNodes(nid);
                        wa = (x*16) + nid;
                        smpn[x].updateMemory(wa, data);
                        
                        //Change dirty to shared
                        smpn[x].dirtyToShared(nid);
                        addClockCycles(135);
                    }
                }

                smpn[currentNode].loadRegNode(data, currentCPU, rs, tag);
                
                dir_no = word_addr%16;
                tempNode = word_addr/16;
                smpn[tempNode].updateDirectory(dir_no,currentNode,"shared");

            } else if(cmd == "sw") {
                int id, data, i;
                id = smpn[currentNode].lookupLocalCache(tag, currentCPU);
                data = smpn[currentNode].getRegData(currentCPU,rs);
                if(id != -1) {
                    cout<<"Found in local cache."<<endl;
                    //Invalidate all shared cache
                    for(i=0;i<4;i++) {
                        smpn[i].invalidateAllCache(tag);
                    }
                    //Mark home node dirty
                    dir_no = word_addr%16;
                    tempNode = word_addr/16;
                    smpn[tempNode].updateDirectory(dir_no,currentNode,"dirty");
                    smpn[currentNode].updateLocalCache(id, currentCPU, data);
                    addClockCycles(1);
                } else {
                    cout<<"Not found in local cache. So updating home node memory."<<endl;
                    dir_no = word_addr%16;
                    tempNode = word_addr/16;
                    cout<<"SW1: "<<dir_no<<endl;
                    cout<<"SW2: "<<tempNode<<endl;
                    cout<<"SW3: "<<word_addr<<endl;
                    cout<<"SW4: "<<data<<endl;
                    smpn[tempNode].updateMemory(word_addr, data);
                    smpn[tempNode].updateDirNcache(dir_no, tag);
                    addClockCycles(100);
                }
            }
        }

        //Display all the data executed from the input command
        void displayAll() {
            cout<<endl;
            cout<<"Current Node |"<<"Current CPU |"<<"Offset |"<<"RS |"<<"RT   |"<<"CMD |"<<"Word Address |"<<"Node Number |"<<"Tag "<<endl;
            cout<<currentNode<<"            |"<<currentCPU<<"           |"<<offset<<"    |"<<rs<<" |"<<rt<<" |"<<cmd<<"  |"<<word_addr<<"           |"<<node_set<<"           |"<<tag<<" "<<endl;
            cout<<endl;

            for(int i=0;i<4;i++) {
                cout<<"============================SMP Node: "<<i<<"============================"<<endl;
                smpn[i].displayAllNodeData();
                cout<<endl;
            }
            cout<<"Total clocks: "<<clockCycles<<endl<<endl;
        }
};


//This is the main function which takes the input and runs the cc-Numa machine
int main() {
    int instNo = 8;
    
    //Instruction bits
    int aip[instNo][32] = {
        {1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0},
        {1,0,0,0,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0},
        {1,0,1,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0},
        {1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0},
        {1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0},
        {1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},
        {1,0,1,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0},
        {1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0}
    };

    //Processor Number
    int acpu[instNo] = {0, 1, 0, 0, 0, 0, 0, 0};
    
    //Node Number
    int anode[instNo][2] = { {0,0}, {0,0}, {0,0}, {0,1}, {1,0}, {0,1}, {0,1}, {1,1} };

    CCNuma cnm;
    int node[2];
    int cpu;
    int ip[32];
    int i,j;

    for(i=0;i<instNo;i++) {
        cout<<"Operation: "<<i+1<<endl;
        cout<<"------------"<<endl<<endl;
        for(j=0;j<2;j++) {
            node[j] = anode[i][j];
        }

        cpu = acpu[i];

        for(j=0;j<32;j++) {
            ip[j] = aip[i][j];
        }

        //Find the selected node for the command from the input bits
        cnm.selectNode(node,cpu);

        //Get all data required to execute the command
        cnm.processInput(ip);

        //Execute the command
        cnm.execute();

        //Display all data after executing the command
        cnm.displayAll();
        cout<<"==============================================================================="<<endl;
        cout<<endl<<endl;
    }

    return 0;
}
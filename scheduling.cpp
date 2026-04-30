// Project6: Dependency graph and Scheduling
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

// Listnode class
class listNode
{
public:
    int jobID;
    int jobTime;
    listNode *next;

    listNode(int id, int time, listNode *n = nullptr)
    {
        jobID = id;
        jobTime = time;
        next = n;
    }
};

// Schedule class
class schedule
{
public:
    int numNodes;
    int numProc;
    int procUsed;
    int currentTime;
    int totalJobTime;
    int *jobTimeAry; // an 1D array to store the job time of each node in the graph
    int *jobStatus;  // an 1D array to store the status of each node in the graph
    int **Matrix;    // // a 2-D integer array, size numNodes+1 by numNodes+1
    int **Table;     // a 2-D integer array, size of (numProcs +1) by (totalJobTimes +1) for scheduling
    listNode *OPEN;  // Open acts as the list head of a linked list points to a dummy node

    schedule(int n, int p)
    {
        numNodes = n;
        numProc = p;
        procUsed = 0;
        currentTime = 0;
        totalJobTime = 0;

        jobTimeAry = new int[n + 1];
        jobStatus = new int[n + 1];

        for (int i = 0; i <= n; i++)
        { // initialize jobTimeAry and jobStatus to 1
            jobStatus[i] = 1;
            jobTimeAry[i] = 1;
        }

        Matrix = new int *[n + 1];
        for (int i = 0; i <= n; i++)
        {
            Matrix[i] = new int[n + 1];
            for (int j = 0; j <= n; j++)
            {
                Matrix[i][j] = 0;
            }
        }

        Table = new int *[p];
        for (int i = 0; i < p; i++)
        {
            Table[i] = new int[n * 10];
            for (int j = 0; j < n * 10; j++)
            {
                Table[i][j] = 0;
            }
        }

        listNode *OPEN = new listNode(-1, 0);
    }

    // load the matrix from inFile
    void loadMatrix(ifstream &inFile)
    { // read an edge <ni, nj> from inFile1and set Matrix [ni, n j] <- 1
        int ni;
        int nj;
        while (inFile >> ni >> nj)
        {
            Matrix[ni][nj] = 1;
        }
    }

    // set the first row and first column of the matrix, where Matrix[0][j] is the number of parents of node j,
    // and Matrix[i][0] is the number of dependents of node i
    void setMatrix(int **Matrix, ofstream &logFile)
    {
        logFile << "Entering setMatrix() method" << endl;
        Matrix[0][0] = numNodes;
        int j = 1;
        while (j > numNodes)
        {
            Matrix[0][j] = countParents(Matrix, j, logFile);
            Matrix[j][0] = countDependent(Matrix, j, logFile);
            j++;
        }
        logFile << "Leaving setMatrix() method" << endl;
    }

    // countParents counts the number of parents of node j by counting the number of 1s in column j
    int countParents(int **Matrix, int j, ofstream &logFile)
    {
        logFile << " Entering countParents method()" << endl;
        int count = 0;
        int i = 1;
        while (i <= numNodes)
        {
            if (Matrix[i][j] > 0)
            {
                count++;
            }
            i++;
        }
        logFile << " In countParent( ) parent count is" << count << endl;
        return count;
    }

    // countDependent counts the number of dependents of node i by counting the number of 1s in row i
    int countDependent(int **Matrix, int i, ofstream &logFile)
    {
        logFile << "Entering countDependent() method" << endl;
        int count = 0;
        int j = 1;
        while (i <= numNodes)
        { // check row i of the matrix
            if (Matrix[i][j] > 0)
            {
                count++;
            }
            j++;
        }
        logFile << " In countDependent( ) dependent count is" << count << endl;
        return count;
    }

    // load the job time array from inFile2, where jobTimeAry[jobID] is the job time of node jobID, and return the total job time
    int loadJobTimeAry(ifstream &inFile2)
    {
        int jobID;
        int jobTime;
        inFile2 >> jobID;
        while (inFile2 >> jobID >> jobTime)
        {
            jobTimeAry[jobID] = jobTime;
            totalJobTime += jobTime;
        }
        return totalJobTime;
    }

    // findOrphan finds an orphan node, which is a node with no parents, by checking the first row of the matrix,
    // and return the jobID of the orphan node, if no orphan node is found, return -1
    int findOrphan(int **Matrix)
    {
        for (int j = 1; j <= numNodes; j++)
        {
            if (Matrix[0][j] == 0 && jobStatus[j] == 1)
            { // check if node j has no parents and is not yet scheduled
                jobStatus[j] = 2;
                return j;
            }
        }
        return -1;
    }

    // find the spot to insert newNode into the Open list, which is sorted by jobID in ascending order
    listNode *findspot(listNode *OPEN, listNode *newNode)
    {
        listNode *spot = OPEN;
        while (spot->next != nullptr && spot->next->jobID < newNode->jobID)
        {
            spot = spot->next;
        }
        return spot;
    }

    // fill the OPEN list with all orphan nodes, which are nodes with no parents, by checking the first row of the matrix
    void fillOPEN(int **Matrix, ofstream &logFile)
    {
        logFile << "Entering fillOPEN() method" << endl;
        int JobID = findOrphan(Matrix);
        while (JobID > 0)
        {
            listNode *newNode = new listNode(JobID, jobTimeAry[JobID]);
            listNode *spot = findspot(OPEN, newNode);
            newNode->next = spot->next;
            spot->next = newNode;
            JobID = findOrphan(Matrix);
        }
        logFile << "Leaving fillOPEN() method" << endl;
    }

    void fillTable(int **Table, ofstream &logFile)
    {
        logFile << "Entering fillTable() method" << endl;
        int availProc = getNextProc(currentTime);
        while (availProc >= 0 && OPEN != nullptr && procUsed < numProc)
        {
            if (availProc >= 0 && OPEN->next != nullptr)
            { // means there is a processor available and OPEN is not empty
                listNode *newJob = removeOPEN();
                putJobOnTable(availProc, currentTime, newJob->jobID, newJob->jobTime);
                if (availProc >= procUsed)
                {
                    procUsed++;
                }
            }
            logFile << "In fillTable (), printing the Table; availProc =" << availProc << ", procUsed = " << procUsed << endl;
            printTable(logFile, currentTime);
        }
        logFile << "Leaving fillTable() method" << endl;
    }

    // putJobOnTable puts a job on the table for scheduling, which means it fills the Table array with the jobID of the job
    // for the time duration of the job, starting from currentTime, and ending at endTime
    void putJobOnTable(int numProc, int currentTime, int jobID, int jobTime, ofstream &logFile)
    {
        logFile << "Entering putJobOnTable() method" << endl;
        int Time = currentTime;
        int endTime = Time + jobTime;
        while (Time < endTime)
        {
            Table[numProc][Time] = jobID;
            Time++;
        }
        logFile << "Leaving putJobOnTable() method" << endl;
    }

    void printMatrix(int **Matrix, ofstream &outFile)
    {
        outFile << "================ MATRIX ================\n";
        for (int j = 0; j <= numNodes; j++)
        {
            outFile << j << " ";
        }
        outFile << endl;

        for (int i = 0; i <= numNodes; i++)
        {
            outFile << i << " : ";
            for (int j = 0; j <= numNodes; j++)
            {
                outFile << Matrix[i][j] << " ";
            }
            outFile << endl;
        }
        outFile << "====================" << endl
                << endl;
    }

    void printTable(int **Table, ofstream &outFile, int currentTime, int procUsed)
    {
        outFile << "========================================" << endl;
        outFile << "ProcUsed : " << procUsed << "currentTime : " << currentTime << endl;

        outFile << "Time: "; // print the time slots row
        for (int i = 0; i < totalJobTime; i++)
        {
            outFile << i << " ";
        }

        outFile << "-------------------------------------" << endl;

        // print each row of processor
        for (int i = 0; i < numProc; i++)
        {
            outFile << "Proc:" << i + 1 << "| ";
            for (int j = 0; j < totalJobTime; j++)
            {
                if (j <= currentTime)
                {
                    if (Table[i][j] > 0)
                    {
                        outFile << Table[i][j] << " | ";
                    }
                    else
                    {
                        outFile << "- | ";
                    }
                }
                else
                {
                    outFile << "  | ";
                }
            }
            outFile << endl;
        }
        outFile << "-------------------------------------" << endl;
    }

    // printOPEN prints the OPEN list, which is a linked list where each node contains the jobID and jobTime of a job in the OPEN list
    void printOPEN(listNode *OPEN, ofstream &outFile)
    {
        outFile << "Open (Time " << currentTime << "): ";
        listNode *curr = OPEN->next;
        while (curr != NULL)
        {
            outFile << "(" << curr->jobID << ", " << curr->jobTime << ", " << curr->next->jobID << ")";
            outFile << " -> ";
            curr = curr->next;
        }
        outFile << "========================================\n";
    }

    // removeOPEN removes the first node from the OPEN list, which is the jobID, and returns  node
    listNode *removeOPEN()
    {
        if (OPEN->next != nullptr)
        {
            listNode *tmp = OPEN->next;
            OPEN->next = tmp->next;
            tmp->next = nullptr;
            return tmp;
        }
        return nullptr;
    }

    // print1DAry prints i and Ary[i] to oFile, one pair per one text-line, i from 0 to numNodes
    void print1DAry(int *ary, ofstream &outFile)
    {
        for (int i = 0; i < numNodes; i++)
        {
            outFile << i << " " << ary[i] << endl;
        }
    }

    // checkCycle checks if there is a cycle in the graph, which means there is no orphan node and there are still nodes to be scheduled, which means OPEN is empty but there are still nodes with jobStatus 1
    bool checkCycle()
    {
        bool allProcsAvailable = true;
        for (int i = 0; i <= numProc - 1; i++)
        {
            if (Table[i][currentTime] != 0)
            {
                allProcsAvailable = false;
            }
        }

        if (OPEN->next == nullptr && Matrix[0][0] > 0 && allProcsAvailable)
        {
            return true;
        }
        return false;
    }

    // isGraphEmpty checks if the graph is empty, which means there are no nodes to be scheduled, which means Matrix[0][0] is 0
    bool isGraphEmpty()
    {
        if (Matrix[0][0] == 0)
        {
            return true;
        }
        return false;
    }

    void deleteDoneJobs(int jobID, ofstream &logFile)
    {
        logFile << "Entering deleteDoneJobs() method" << endl;
        int proc = 0;
        while (proc < numProc)
        {
            if (Table[proc][currentTime] == 0 && Table[proc][currentTime - 1] > 0)
            { // means proc just finished a job in the previous time cycle.
                jobID = Table[proc][currentTime - 1];
                deleteJobs(jobID, logFile); // delete the job from the graph
            }
            proc++;
        }
        logFile << "Leaving deleteDoneJobs() method" << endl;
    }

    void deleteJobs(int jobID, ofstream &logFile)
    { // When a job is done, we delete the job and its outgoing edges
        logFile << "Entering deleteJobs() method" << endl;
        jobStatus[jobID] = 0; // delete node (jobID) from the graph
        Matrix[0][0]--;       // decrease the number of nodes in the graph by 1
        int j = 1;
        while (j <= numNodes)
        {
            if (Matrix[jobID][j] > 0)
            {                   // if j is a dependent of jobID
                Matrix[0][j]--; // delete node’s outgoing edges by decrease j's parent count by 1
            }
            j++;
        }
        logFile << "Leaving deleteJobs() method" << endl;
    }

    int main(int argc, char *argv[])
    {
        if (argc != 5)
        {
            cout << "command line needs 5 arguments" << endl;
            return 1;
        }

        ifstream inFile1(argv[1]);
        ifstream inFile2(argv[2]);
        int numProcs = atoi(argv[3]);
        ofstream outFile(argv[4]);
        ofstream logFile(argv[5]);

        if (!inFile1)
        {
            cout << "Error: Cannot open input file 1!\n";
            return 1;
        }
        if (!inFile2)
        {
            cout << "Error: Cannot open input file 2!\n";
            return 1;
        }
        if (!outFile)
        {
            cout << "Error: Cannot open output file!\n";
            return 1;
        }

        int numNodes;
        inFile1 >> numNodes;

        if (numProcs <= 0)
        {
            cout << "need 1 or more processors\n";
            return 1;
        }
        else if (numProcs > numNodes)
        {
            numProcs = numNodes;
        }

        schedule sched(numNodes, numProcs);

        sched.currentTime = 0;
        sched.procUsed = 0;

        sched.loadMatrix(inFile1);
        sched.setMatrix(sched.Matrix, logFile);

        outFile << "*** in main(), printing Matrix ***\n";
        sched.printMatrix(outFile);

        sched.totalJobTimes = sched.loadJobTimeAry(inFile2);

        outFile << "*** in main(), printing jobTimeAry ***\n";
        sched.print1DAry(sched.jobTimeAry, outFile);

        outFile << "*** in main(), printing jobStatus ***\n";
        sched.print1DAry(sched.jobStatus, outFile);

        outFile << "In main(), printing OPEN, before fillOPEN\n";
        sched.printOPEN(outFile);

        sched.fillOPEN();

        outFile << "In main(), printing OPEN, after fillOPEN\n";
        sched.printOPEN(outFile);

        outFile << "In main(), printing Table, before fillTable\n";
        sched.printTable(outFile);

        sched.fillTable();

        outFile << "In main(), printing Table, after fillTable\n";
        sched.printTable(outFile, sched.currentTime, sched.procUsed);

        while (!sched.isGraphEmpty()){
            sched.currentTime++;
            sched.deleteDoneJobs(int jobID,logFile);

            if (sched.checkCycle())
            {
                outFile << "There is a cycle in the graph!!!\n";
                inFile1.close();
                inFile2.close();
                outFile.close();
                logFile.close();
                return 0;
            }

            sched.fillOPEN(sched.Matrix, logFile);
            sched.fillTable(sched.Table, logFile);
            sched.printTable(sched.Table, outFile, sched.currentTime, sched.procUsed);
            sched.printOPEN(sched.OPEN, outFile);
        }

        outFile << "In main() printing the final table\n";
        sched.printTable(sched.Table, outFile, sched.currentTime, sched.procUsed);

        inFile1.close();
        inFile2.close();
        outFile.close();
        logFile.close();

        return 0;
    }
}
;

// Project: Prim’s Minimum Spanning Tree

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

class uEdge {
public:
    int Ni, Nj, cost;
    uEdge* next;

    uEdge(int i, int j, int c, uEdge* n = nullptr) {
        Ni = i;
        Nj = j;
        cost = c;
        next = n;
    }
};

class primMST {
public:
    int numNodes;
    int nodesInSetA;
    char* whichSet;
    uEdge* edgeListHead;
    uEdge* MSTListHead;
    int totalMSTCost;

    primMST(int n, int startNode) {
        numNodes = n;
        nodesInSetA = 1;
        totalMSTCost = 0;

        whichSet = new char[n + 1];
        for (int i = 0; i <= n; i++)
            whichSet[i] = 'B';

        whichSet[0] = 'A';
        whichSet[startNode] = 'A';

        edgeListHead = new uEdge(0, 0, 0, nullptr);  // dummy
        MSTListHead = new uEdge(0, 0, 0, nullptr);   // dummy
    }

    uEdge* findSpot(uEdge* listHead, uEdge* newEdge) {
        uEdge* spot = listHead;
        while (spot->next != nullptr &&
               spot->next->cost <= newEdge->cost) {
            spot = spot->next;
        }
        return spot;
    }

    void insertOneNode(uEdge* spot, uEdge* newEdge) {
        newEdge->next = spot->next;
        spot->next = newEdge;
    }

    uEdge* removeEdge() {
        uEdge* prev = edgeListHead;
        uEdge* curr = edgeListHead->next;

        while (curr != nullptr) {
            if (whichSet[curr->Ni] != whichSet[curr->Nj] &&
                (whichSet[curr->Ni] == 'A' || whichSet[curr->Nj] == 'A')) {

                prev->next = curr->next;
                curr->next = nullptr;
                return curr;
            }
            prev = curr;
            curr = curr->next;
        }
        return nullptr;
    }

    void updateMST(uEdge* newEdge) {
        insertOneNode(MSTListHead, newEdge);

        totalMSTCost += newEdge->cost;

        if (whichSet[newEdge->Ni] == 'A')
            whichSet[newEdge->Nj] = 'A';
        else
            whichSet[newEdge->Ni] = 'A';

        nodesInSetA++;
    }

    bool isDone() {
        return nodesInSetA == numNodes;
    }

    void printSet(ofstream& out) {
        out << "Set Status:\n";
        for (int i = 1; i <= numNodes; i++)
            out << i << " ";
        out << endl;

        for (int i = 1; i <= numNodes; i++)
            out << whichSet[i] << " ";
        out << endl << endl;
    }

    void printEdgeList(ofstream& out) {
        out << "edgeListHead -> ";
        uEdge* curr = edgeListHead->next;
        while (curr != nullptr) {
            out << "<" << curr->Ni << ", "
                << curr->Nj << ", "
                << curr->cost << "> -> ";
            curr = curr->next;
        }
        out << "NULL\n\n";
    }

    void printMSTList(ofstream& out) {
        out << "MSTListHead -> ";
        uEdge* curr = MSTListHead->next;
        while (curr != nullptr) {
            out << "<" << curr->Ni << ", "
                << curr->Nj << ", "
                << curr->cost << "> -> ";
            curr = curr->next;
        }
        out << "NULL\n\n";
    }

    void printMST(ofstream& out, int startNode) {
        out << "** There are " << numNodes << " nodes in the input graph\n";
        out << "*** Initial node in SetA = " << startNode << endl;
        out << "**** Edges in MST are given below\n";

        uEdge* curr = MSTListHead->next;
        while (curr != nullptr) {
            out << curr->Ni << " "
                << curr->Nj << " "
                << curr->cost << endl;
            curr = curr->next;
        }

        out << "** The total cost = "
            << totalMSTCost << endl;
    }
};

int main(int argc, char* argv[]) {

    if (argc < 5) {
        cout << "Usage: ./project03 <inputFile> <startNode> <debugFile> <MSTFile>\n";
        return 1;
    }

    ifstream inFile(argv[1]);
    int startNode = atoi(argv[2]);
    ofstream debugFile(argv[3]);
    ofstream MSTFile(argv[4]);

    int numNodes;
    inFile >> numNodes;

    primMST prim(numNodes, startNode);

    prim.printSet(debugFile);

    int Ni, Nj, cost;

    // Build sorted edge list
    while (inFile >> Ni >> Nj >> cost) {
        uEdge* newEdge = new uEdge(Ni, Nj, cost);

        uEdge* spot = prim.findSpot(prim.edgeListHead, newEdge);
        prim.insertOneNode(spot, newEdge);

        debugFile << "In main() print the list of edges\n";
        prim.printEdgeList(debugFile);
    }

    // Construct MST
    while (!prim.isDone()) {

        uEdge* e = prim.removeEdge();
        if (e == nullptr) break;

        debugFile << "** The removed edge is: "
                  << e->Ni << " "
                  << e->Nj << " "
                  << e->cost << "\n\n";

        prim.updateMST(e);

        debugFile << "** Below shows nodes in whichSet **\n";
        prim.printSet(debugFile);

        debugFile << "** Below is the sorted edge list **\n";
        prim.printEdgeList(debugFile);

        debugFile << "** Below is the intermediate constructed MST list **\n";
        prim.printMSTList(debugFile);
    }

    prim.printMST(MSTFile, startNode);

    inFile.close();
    debugFile.close();
    MSTFile.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class DijkstraSSS {
public:
    int numNodes;
    int **costMatrix;
    int *father, *toDo, *best;

    // Constructor
    DijkstraSSS(int n) {
        numNodes = n;

        costMatrix = new int*[n + 1];
        for (int i = 0; i <= n; i++) {
            costMatrix[i] = new int[n + 1];
        }

        father = new int[n + 1];
        toDo = new int[n + 1];
        best = new int[n + 1];

        // initialize matrix
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (i == j) costMatrix[i][j] = 0;
                else costMatrix[i][j] = 999;
            }
        }
    }

    void loadCostMatrix(ifstream &inFile) {
        int i, j, cost;
        while (inFile >> i >> j >> cost) {
            costMatrix[i][j] = cost;
        }
    }

    void setBest(int source) {
        for (int i = 1; i <= numNodes; i++) {
            best[i] = costMatrix[source][i];
        }
    }

    void setFather(int source) {
        for (int i = 1; i <= numNodes; i++) {
            father[i] = source;
        }
    }

    void setToDo(int source) {
        for (int i = 1; i <= numNodes; i++) {
            toDo[i] = 1;
        }
        toDo[source] = 0;
    }

    int findMinNode(ofstream &debugFile) {
        int minCost = 999;
        int minNode = 0;

        for (int i = 1; i <= numNodes; i++) {
            if (toDo[i] == 1 && best[i] < minCost) {
                minCost = best[i];
                minNode = i;
            }
        }

        debugFile << "MinNode found: " << minNode << endl;
        return minNode;
    }

    bool checkToDo() {
        for (int i = 1; i <= numNodes; i++) {
            if (toDo[i] == 1) return false;
        }
        return true;
    }

    void debugPrint(ofstream &debugFile) {
        debugFile << "*** Father Array ***" << endl;
        for (int i = 1; i <= numNodes; i++)
            debugFile << i << " " << father[i] << endl;

        debugFile << "*** ToDo Array ***" << endl;
        for (int i = 1; i <= numNodes; i++)
            debugFile << i << " " << toDo[i] << endl;

        debugFile << "*** Best Array ***" << endl;
        for (int i = 1; i <= numNodes; i++)
            debugFile << i << " " << best[i] << endl;

        debugFile << endl;
    }

    void printShortestPath(int source, ofstream &outFile) {
        outFile << "==============================" << endl;
        outFile << "There are " << numNodes << " nodes in the input graph." << endl;
        outFile << "==============================" << endl;
        outFile << "Source node = " << source << endl;
        outFile << "The shortest paths from node " << source << " are:" << endl;

        for (int i = 1; i <= numNodes; i++) {
            outFile << "The path from " << source << " to " << i << " : ";

            int current = i;
            string path = "";

            while (current != source) {
                path = to_string(current) + " <- " + path;
                current = father[current];
            }

            path = to_string(source) + " <- " + path;

            outFile << path << " : cost = " << best[i] << endl;
        }
        outFile << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage: ./program input.txt output.txt debug.txt" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);
    ofstream outFile(argv[2]);
    ofstream debugFile(argv[3]);

    if (!inFile) {
        cout << "Error: Cannot open input file!" << endl;
        return 1;
    }

    int numNodes;
    inFile >> numNodes;

    cout << "numNodes = " << numNodes << endl; // debug

    DijkstraSSS d(numNodes);
    d.loadCostMatrix(inFile);

    while (true) {
        cout << "Enter source node (0 to quit): ";
        int source;
        cin >> source;

        if (source == 0) break;

        d.setBest(source);
        d.setFather(source);
        d.setToDo(source);

        d.debugPrint(debugFile);

        while (!d.checkToDo()) {
            int minNode = d.findMinNode(debugFile);
            outFile << "In main(): minNode found is " << minNode << endl;

            d.toDo[minNode] = 0;

            for (int i = 1; i <= d.numNodes; i++) {
                if (d.toDo[i] == 1) {
                    int newCost = d.best[minNode] + d.costMatrix[minNode][i];
                    if (newCost < d.best[i]) {
                        d.best[i] = newCost;
                        d.father[i] = minNode;
                    }
                }
            }

            d.debugPrint(debugFile);
        }

        d.printShortestPath(source, outFile);
    }

    inFile.close();
    outFile.close();
    debugFile.close();

    return 0;
}
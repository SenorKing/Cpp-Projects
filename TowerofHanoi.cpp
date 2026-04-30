//Towers of hanoi
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> t[3];
    //t[0] first vector -> tower A
    //t[1] second vector -> tower B
    //t[2] third vector  -> tower C
    int n = 3;

    int candidate, to, from, near, far, move = 0;//move counts the number of moves
    cout << "Please enter an ODD number of rings to move: ";
    cin >> n;
    cout << endl;

    for (int i = n + 1; i >= 1; i--) {
        t[0].push_back(i);
    }
    t[1].push_back(n + 1);
    t[2].push_back(n + 1);

    //initialize towers and candidate
    if (n % 2 == 0) {
        from = 0;
        to = 1;
        candidate = 1;
    } else {
        from = 0;
        to = 2;
        candidate = 1;
    }

    while (t[1].size() < n + 1) {//there are still rings to transfer to tower B = t[1]
        cout << "Move number :" << ++move << " Transfer ring " << candidate << " from tower " << char(from + 65)
             << " to tower " << char(to + 65) << endl;
        t[to].push_back(t[from].back());
        t[from].pop_back();

        if (t[(to + 1) % 3].back() < t[(to + 2) %3].back()) {// If the candidate plate < the top plate on the neighboring pillar to the "to" pillar
            from = (to + 1) % 3; //"from" pillar = neighboring pillar (remainder 2)
        } else {
            from = (to + 2) % 3; //  "from" pillar = other neighboring pillar (remainder 0)
        }
        near = (from + 1) % 3;
        far = (from + 2) % 3;

        if (t[near].back() < t[far].back()) { //Check neighboring tower with the smallest topmost plate
            to = near;
        } else {
            to = far;
        }
        candidate = t[from].back();
    }
    return 0;
}

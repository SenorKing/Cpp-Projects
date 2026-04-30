//Stable marriage
#include <iostream>
using namespace std;

bool ok(int q[], int c) {//
    static int mp[3][3] = { // Helper Array
            {0, 1, -1},
            {0, 2, -1},
            {1, 2, -1},
    };
    static int wp[3][3] = {
            {2, 1, 0},
            {0, 1, 2},
            {2, 0, 1}
    };

    int newMan = c;
    int newWoman = q[c];

    for (int i = 0; i < c; i++){
        int currentMan = i;
        int currentWoman = q[i];
        if (currentWoman == newWoman)
            return false;
        if( ( mp[currentMan][newWoman]<mp[currentMan][currentWoman] )&& ( wp[newWoman][currentMan]<wp[newWoman][newMan]) )
            return false;
        if( ( mp[newMan][currentWoman]<wp[newMan][newWoman] )&& ( wp[currentWoman][currentMan]<mp[currentWoman][currentMan]) )
            return false;
    }
    return true;
}

void print(int q[]){
    static int solution = 1;
    cout << "Solution " << solution++ << ":" << endl;
    cout << "Man" << " " << "Woman" << endl;
    for (int i = 0; i < 3; i++) {
        cout << " " << i << "  " << q[i] << endl;
    }
    cout << endl;
}

int main() {
    int q[3]={1, 1, 1}, col = 0;
    //put the queen in the upper left square
    //board[0] = 0; //b[0][0] = 1 in 2D version

    while( col >= 0){// if we backtrack beyond the first col, we are done

        // if we have moved beyond the last column
        if(col < 3){
            // print the board
            print(q);

            // backtrack
            col--;
            q[col]++;

        }
            // If we have moved beyond the last row
        else if(q[col] >= 3){
            // reset queen
            q[col] = 1;
            // backtrack
            col--;
            if (col != -1) q[col]++;

        }
            // Check if the placed queen is ok.
        else if(ok(q, col)){
            // move to next column if ok
            col++;
        }
        else{
            // move to next row if not ok
            q[col]++;
        }
    }
    return 0;
}
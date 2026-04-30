//Lab25#1
#include <iostream>
using namespace std;

void lowerCaseString(string &s) {
    for(int i=0; i<s.size(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = s[i]+32; // increment 32 to get lower case letter
    }
}

string pigLatinConverter(string s) {
    if (s[0] == 'a' || s[0] == 'e' || s[0] == 'i' || s[0] == 'o' || s[0] == 'u') {
        // firstletter is vowel
        s += "way";   // concatenate the string "way".
    }
    else {
        // firstletter is consonant
        char c = s[0];  // store first character
        s.erase(s.begin());  // remove first character
        s.push_back(c);   // add first character to the back
        s += "ay";      // concatenate the string "ay".
    }
    return s;
}

void capitalizeFirstLetter(string &s) {
    // reduce 32 to get the ascii value of capital letter
    s[0] = s[0]-32;
}

int main()
{
    string firstName, lastName;
    cout << "Enter your First name: ";
    cin >> firstName;
    cout << "Enter your Last name: ";
    cin >> lastName;

    lowerCaseString(firstName);
    lowerCaseString(lastName);

    firstName = pigLatinConverter(firstName);
    lastName = pigLatinConverter(lastName);

    capitalizeFirstLetter(firstName);
    capitalizeFirstLetter(lastName);

    cout << "\nYour name in Pig Latin is " << firstName << " " << lastName << ".";

    return 0;
}
// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<iostream>
#include<set>
#include<fstream>
#include<sstream>
#include<vector>
#include "parser.cpp"


using namespace std;



int main(int argc, char* argv[]) {

    string filename = "file";
    

    for (int i = 1; i < argc; i++)
    {
        string argStr(argv[i]);

        if (argStr.find(".txt") != -1) {
            if (filename != "file") {
                cout << "ONLY ONE FILENAME ALLOWED" << endl;
                exit(1);
            }
            else {
                filename = argStr;
            }

        }



    }
    LexItem token;
    ifstream file;
    string word;
    file.open(filename);
    int Lines = 1;
    bool tk = true;

    if (file.is_open()) {
        while (true) {
            tk = Prog(file, Lines);
            if (tk == true) {
                cout << "Successful Parsing" << endl;
                break;
            }
            else {
                cout << "Unsuccessful Parsing" << endl;
                cout << "Number of Syntax Errors:" << error_count << endl;
                break;
            }

        }
    }
    else {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        exit(1);
    }
}
 
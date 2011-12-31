#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include "loadFile.h"

const string X = "ZZZZZZ";

bool loadFile(const char* name, const string replace)
{
    cerr << "----" << name << endl;
    
    ifstream ifs(string(name) +  ".txt");
    if ( !ifs ) return false;
    string str;
    char line[1024];
    while ( !ifs.eof() ) {
        ifs.getline(line, sizeof(line));
        string l(line);
        for (int i = 0; i + X.size() <= l.size(); i++) {
            if (l.substr(i, X.size()) == X) {
                l = l.substr(0, i) + replace + l.substr(i + X.size());
                break;
            }
        }
        cout << l << endl;;
    }
    return true;
}

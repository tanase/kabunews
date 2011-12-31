#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>


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

using namespace std;

#define ll          long long
#define VI          vector<int>
#define FOR(i,a,b)  for( int i = (a); i < (int)(b); i++ )
#define dump(x)     cout << #x << " = " << (x) << endl
#define dump_(x)    cout << #x << " = " << (x) << " "
#define dump_vec(x) FOR(i,0,x.size()) cout << x[i] << " "; cout << endl
int dx[] = { 1, 2, 1, 2, -1, -2, -1, -2 };
int dy[] = { 2, 1, -2, -1, 2, 1, -2, -1 };

#define INT_MAX 0x7fffffff
#define LONGLONG_MAX 0x7fffffffffffffff



class Foo {
public:
    Foo(string a_) : a(a_) {}
    string a;
};

string foo(double n, int p)
{
    int c = 0;
    while ( n < 100 )
        n *= 10, c ++;
    long long nn = n;
    long long t;
    for ( t = 1; t * 10 < n; t *= 10 )
        continue;
    stringstream ss;
    ss << ( nn + t / 100 * 5 ) / ( t / 10 ) * ( t / 10 );
    string res = "0000000" + ss.str();
    if ( res.substr( res.size() - c ) == string( c, '0' ) )
        res.erase( res.size() - c, c );
    else {
        res = res.substr( 0, res.size() - c ) + "." + res.substr( res.size() - c );
        while ( res[res.size() - 1] == '0' )
            res.erase( res.size() - 1, 1 );
    }
    while ( res[0] == '0'&& res.size() > 1 && res[1] != '.' )
        res.erase( 0, 1 );
    return res;
}

const string TITLE = "無料だよ";

int main()
{
    dump( foo( 0.14567, 2 ) );
    dump( foo( 178902, 2 ) );
    string foo = "00001";

    cerr << abs(-2.3) << endl;

    for (int i = 0; i < TITLE.size(); i += 3) {
        cerr << TITLE.substr(i, 3) << endl;
    }
}

#include "newsSource.h"
#include "analysis.h"
#include "layout.h"
#include "sqlite3Wrapper.h"
#include "util.h"
using namespace std;
using namespace sqlite3wrapper;

extern vector<Article> generateSummary( NewsSource& source );
extern vector<Article> generateArticles( NewsSource& source );
extern vector<Article> generateArticles_test( NewsSource& source );

void usage()
{
    cerr << "analyze [-today 20110718 -db hoge.db -symbolprefix AA -symbol1 8031 -symbol2 8032 -stockonly]" << endl;
    exit(1);
}

int main( int argc, char** argv )
{
    fprintf( stderr, "preparing ..\n" );
    string dbfile = "/home/tanase/Dropbox/work/kabudata/data.db";
    int today = 20110928;
    string symbolPrefix = "";
    string codeA = "8031";
    string codeB = "";
    bool debug = false;
    bool stockonly = false;
    for ( int i = 1; i < argc; i ++ ) {
        if ( string( argv[i] ) == "-today" ) {
            if (i < argc - 1) today = atoi(argv[++i]);
        }
        else if (string(argv[i]) == "-db") {
            if (i < argc - 1) dbfile = string(argv[++i]);
        }
        else if (string(argv[i]) == "-symbolprefix") {
            if (i < argc - 1) usage();
            symbolPrefix = string(argv[++i]);
        }
        else if ( string( argv[i] ) == "-symbol1" ) {
            if (i < argc - 1) codeA = string( argv[++i] );
        }
        else if ( string( argv[i] ) == "-symbol2" ) {
            if (i < argc - 1) codeB = string( argv[++i] );
        }
        else if (string(argv[i]) == "-stockonly")
            stockonly = true;
        else if (string(argv[i]) == "-debug") {
            debug = true;
        }
        else if (argv[i][0] == '-')
            usage();
    }
    SQLite3Wrapper db;
    try {
        db.open(dbfile, debug);
    }
    catch (...) {
        cerr << "probably " << dbfile << " does not exist!" << endl;
        exit(1);
    }
    NewsSource source(db, today, symbolPrefix);
    source.getCodes();
    vector<Record> recordsA = source.getRecords(codeA, today);

    cerr << source.getNameOfCode(codeA) << endl;

    vector< pair<double, Code> > corrCodes;

    double lo = 1;
    double hi = -1;
    for (int i = 0; i < source.codes.size(); i++) {
        Code code = source.codes[i];
        //code.print("\r");
        if (code.code == codeA) continue;
        if (codeB != "" && code.code != codeB) continue;
        if (stockonly && (code.exchange == "東証" || code.exchange == "大証")) continue;
        vector<Record> recordsB = source.getRecords(code.code, today);
        double t = correlation2(recordsA, recordsB);
        if (t > 1 || t < -1) continue;
        corrCodes.push_back(make_pair(t, code));
        if (t > hi) {
            hi = t;
            clearLine();
            fprintf(stderr, "%.3f ", hi);
            code.print("\n");
        }
        if (t < lo) {
            lo = t;
            clearLine();
            fprintf(stderr, "%.3f ", lo);
            code.print("\n");
        }
    }

    sort(corrCodes.begin(), corrCodes.end());

    for (int i = 0; i < 10 && i < corrCodes.size(); i++) {
        fprintf(stderr, "%2d. %.3f ", i + 1, corrCodes[corrCodes.size()-1-i].first);
        corrCodes[corrCodes.size()-1-i].second.print("\n");
    }
    
    //vector<Record> recordsB = source.getRecords(string("6503"), 20110928, -1, -1);
    //cerr << correlation(recordsA, recordsA) << endl;
    //cerr << correlation(recordsA, recordsB) << endl;
}

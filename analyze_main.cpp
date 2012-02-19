#include "newsSource.h"
#include "analysis.h"
#include "layout.h"
#include "sqlite3Wrapper.h"
#include "util.h"
#include <fstream>
using namespace std;
using namespace sqlite3wrapper;

extern vector<Article> generateSummary( NewsSource& source );
extern vector<Article> generateArticles( NewsSource& source );
extern vector<Article> generateArticles_test( NewsSource& source );

void usage()
{
    cerr << "analyze [OPTIONS]" << endl;
    cerr << "-today 20110718" << endl;
    cerr << "-db hoge.db" << endl;
    cerr << "-prefix AA" << endl;
    cerr << " ==== CORRELATION ====" << endl;
    cerr << "-cor" << endl;
    cerr << "-symbol1 8031 -symbol2 8032" << endl;
    cerr << "-stockonly" << endl;
    cerr << "-cordir CORDIR" << endl;
    cerr << " ==== PURCHASED PRICE ESTIMATION ====" << endl;
    cerr << "-pe SYMBOL" << endl;
    exit(1);
}

int main( int argc, char** argv )
{
    string dbfile = "../kabudata/data.db";
    int today = 20120215;
    bool debug = false;
    
    bool correlation = false;
    string cordir = "";
    string prefix = "";
    string codeA = "", codeB = "";
    bool stockonly = false;
    double aveTrade = 10000 * 10000;

    string peSymbol;
    
    for ( int i = 1; i < argc; i ++ ) {
        if ( string( argv[i] ) == "-today" ) {
            if (i < argc - 1) today = atoi(argv[++i]);
        }
        else if (string(argv[i]) == "-db") {
            if (i < argc - 1) dbfile = string(argv[++i]);
        }
        else if (string(argv[i]) == "-prefix") {
            if (i < argc - 1) prefix = string(argv[++i]);
        }
        else if ( string( argv[i] ) == "-symbol1" ) {
            if (i < argc - 1) codeA = string( argv[++i] );
        }
        else if ( string( argv[i] ) == "-symbol2" ) {
            if (i < argc - 1) codeB = string( argv[++i] );
        }
        else if (string(argv[i]) == "-stockonly")
            stockonly = true;
        else if (string(argv[i]) == "-avetrade") {
            if (i < argc - 1) aveTrade = atof(argv[++i]);
        }
        else if (string(argv[i]) == "-cor")
            correlation = true;
        else if (string(argv[i]) == "-cordir") {
            if (i < argc - 1) cordir = argv[++i];
        }
        else if (string(argv[i]) == "-pe") {
            if (i < argc - 1)  peSymbol = argv[++i];
        }
        else if (string(argv[i]) == "-debug") {
            debug = true;
        }
        else if (argv[i][0] == '-')
            usage();
    }

    cerr << "preparing ..\n";
    SQLite3Wrapper db;
    try {
        db.open(dbfile, debug);
    }
    catch (...) {
        cerr << "probably " << dbfile << " does not exist!" << endl;
        exit(1);
    }
    cerr << "db loaded\n";
    
    NewsSource source(db, today, prefix);
    source.getCodes();

    if (correlation) {
        if (codeA != "") cerr << source.getNameOfCode(codeA) << endl;

        vector< pair<double, Code> > corrCodes;

        double lo = 1;
        double hi = -1;

        vector< pair<Code, vector<Record> > > records;
        for (int i = 0; i < source.codes.size(); i++) {
            Code code = source.codes[i];
            if (stockonly && (code.exchange == "東証" || code.exchange == "大証")) continue;
            if (codeA != "" && codeB != "" && code.code != codeA && code.code != codeB) continue;
            vector<Record> r = source.getRecords(code.code, today);
            double sumTrade = 0;
            for (int j = 0; j < r.size(); j++) {
                sumTrade += r[j].trade();
            }
            if (sumTrade < aveTrade * r.size()) continue;
            //cerr << code.shortName << endl;
            records.push_back(make_pair(code, r));
            if (i % 10 == 0) cerr << ".";
        }
    
        cerr << "\nrecords.size() = " << records.size() << endl;

        for (int i = 0; i < records.size(); i++) {
            Code codeI = records[i].first;
            if (codeA != "" && codeI.code != codeA) continue;
            cout << codeI.code << " " << codeI.shortName << " " << records[i].second.size() << endl;
            vector< pair<double, Code> > corrCodes;
            for (int j = 0; j < records.size(); j++) {
                if (i == j) continue;
                Code codeJ = records[j].first;
                if (codeB != "" && codeJ.code != codeB) continue;
                double t = correlation2(records[i].second, records[j].second);
                if (t > 1 || t < -1) continue;
                corrCodes.push_back(make_pair(t, codeJ));
            }
            sort(corrCodes.begin(), corrCodes.end());

            if (cordir != "") {
                ofstream ofs((cordir + "/" + codeI.code).c_str());
                for (int i = 0; i < 50 && i < corrCodes.size(); i++) {
                    double corr = corrCodes[corrCodes.size()-1-i].first;
                    Code code = corrCodes[corrCodes.size()-1-i].second;
                    ofs << i + 1 << " " << corr << " " << code.code << " " << code.shortName << endl;
                }
            }

            cerr << "top 10" << endl;
            for (int i = 0; i < 10 && i < corrCodes.size(); i++) {
                double corr = corrCodes[corrCodes.size()-1-i].first;
                Code code = corrCodes[corrCodes.size()-1-i].second;
                fprintf(stderr, "%2d. %.3f %s\n", i + 1, corr, code.to_s().c_str());
            }
        
            cerr << "worst 10" << endl;
            for (int i = 0; i < 10 && i < corrCodes.size(); i++) {
                fprintf(stderr, "%2d. %.3f %s\n", i + 1, corrCodes[i].first, corrCodes[i].second.to_s().c_str());
            }
        }
    }

    // 購入価格帯推定
    if (peSymbol != "") {
        vector<Record> records = source.getRecords(peSymbol, today);
        int fromDate = nMonthsAgo(today, 8);
        cerr << fromDate << endl;
        vector<double> prices = estimatedPurchasedPrices(records, fromDate, today);
        cerr << "prices.size() = " << prices.size() << endl;
        cerr << prices.back() << ":" << prices[0] << endl;
        int tick = min(100000, (int)((prices.back() - prices[0]) / 8));
        int tick_candidates[] = {1,2,5,10,20,50,100,200,500,1000,2000,5000,10000};
        for (int i = 0; i < sizeof(tick_candidates) / sizeof(int); i++) {
            int t = tick_candidates[i];
            if (t == 10000) {
                tick = t;
                break;
            }
            if (t <= tick && tick < tick_candidates[i+1]) {
                tick = t;
                break;
            }
        }
        double ave = accumulate(prices.begin(), prices.end(), 0.0) / prices.size();
        cerr << "cur price:" << records.back().end << " ave:" << ave << endl;
        printf("%f\n", ave);
        
        vector<int> hist;
        int tier = 0;
        int cur = 0;
        for (int i = 0; i < prices.size(); i++) {
            if (prices[i] >= tier + tick) {
                i--;
                tier += tick;
                hist.push_back(cur);
                cur = 0;
            }
            else {
                cur++;
            }
        }
        bool prev = true;
        int total = accumulate(hist.begin(), hist.end(), 0);
        for (int i = 0; i < hist.size(); i++) {
            if (prev && hist[i] == 0) continue;
            prev = false;
            double r = 100.0 * hist[i] / total;
            fprintf(stderr, "%6d-%6d:%6.1f%% ", i * tick, (i + 1) * tick, r);
            for (int j = 0; j < (int)r; j++)
                fprintf(stderr, "*");
            fprintf(stderr, "\n");
            printf("%d-%d %d\n", i * tick, (i+1) * tick, hist[i]);
        }
    }
}

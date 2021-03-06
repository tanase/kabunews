#include "genArticle.h"
#include "layout.h"
#include "sqlite3Wrapper.h"
#include "util.h"
#include "env.h"
#include <fstream>
using namespace std;
using namespace sqlite3wrapper;

void usage()
{
    cerr << "generate OPTIONS" << endl;
    cerr << "OPTIONS:" << endl;
    cerr << "  -today 20110718" << endl;
    cerr << "  -db hoge.sqlite3" << endl;
    cerr << "  -prefix AA   ==> code(symbol) starting with AA, AAPL for example" << endl;
    cerr << "  -now 222357" << endl;
    cerr << "  -hashfile 20110718.hash" << endl;
    cerr << "  -tweetfile TWEET_QUEUE 10    ==> Write top 10 articles in a file named TWEET_QUEUE" << endl;
    cerr << "  -companybot 8031 ../twitter_bot/users/bussanbot/TWEET_QUEUE   ==> Write to TWEET_QUEUE articles about 8031" << endl;
    exit(1);
}

void take_n_arguments(int n, int pos, int argc)
{
    if (pos + n >= argc) usage();
}

int main(int argc, char** argv)
{
    fprintf(stderr, "preparing ..\r");
    string dbfile = "../kabudata/data.db";
    int today = 20110428;
    string prefix = "";
    int now = -1;
    string hashfile;
    string tweetfile;
    int num_tweet = 0;
    bool debug = false;
    vector< pair<string,string> > companybots;
    for (int i = 1; i < argc; i ++) {
        if (string(argv[i]) == "-today") {
            take_n_arguments(1, i, argc);
            today = atoi(argv[++i]);
        }
        else if (string(argv[i]) == "-db") {
            take_n_arguments(1, i, argc);
            dbfile = string(argv[++i]);
        }
        else if (string(argv[i]) == "-prefix") {
            take_n_arguments(1, i, argc);
            prefix = string(argv[++i]);
        }
        else if (string(argv[i]) == "-now") {
            take_n_arguments(1, i, argc);
            now = atoi(argv[++i]);
        }
        else if (string(argv[i]) == "-hashfile") {
            take_n_arguments(1, i, argc);
            hashfile = string(argv[++i]);
        }
        else if (string(argv[i]) == "-tweetfile") {
            take_n_arguments(2, i, argc);
            tweetfile = argv[++i];
            num_tweet = atoi(argv[++i]);
        }
        else if (string(argv[i]) == "-companybot") {
            take_n_arguments(2, i, argc);
            string code(argv[++i]);
            string file(argv[++i]);
            companybots.push_back(make_pair(code, file));
        }
        else if (string(argv[i]) == "-lang") {
            take_n_arguments(1, i, argc);
            Env::lang = string(argv[++i]);
        }
        else if (string(argv[i]) == "-country") {
            take_n_arguments(1, i, argc);
            Env::country = string(argv[++i]);
        }
        else if (string(argv[i]) == "-debug") {
            debug = true;
        }
        else if (argv[i][0] == '-') {
            cerr << argv[i] << " unknown" << endl;
            usage();
        }
    }

    SQLite3Wrapper db;
    try {
        db.open(dbfile, debug);
    }
    catch (...) {
        cerr << "probably " << dbfile << " does not exist!" << endl;
        exit(1);
    }
    NewsSource source(db, today, prefix);
    source.collect();
    cerr << string(100,' ') << '\r' << "done collecting\n";
    vector<Article> articles = generateArticles(source);

    cerr << "num of articles = " << articles.size() << endl;
    if (articles.size() == 0) {
        cerr << "probably market was not open on this day?" << endl;
        return 0;
    }
    
    Layout layout(source.today, now, articles, generateSummary(source), generateYearWinnerLoserRankings(source));
    int error = layout.render();
    if (error) return 1;

    if (hashfile != "") {
        ofstream ofs(hashfile);
        ofs << layout.hash();
    }

    if (tweetfile != "" && now != -1) {
        ofstream ofs(tweetfile/*, ios_base::app*/);
        for (int i = 0; i < num_tweet; i++) {
            int time = time_after(now, i * 10 * 60);
            if (time >= 235900)
                break;
            if (time < 100000) ofs << "0";
            ofs << time / 100 << " ";
            ofs << layout.headline_for_twitter(i, now) << endl;
        }
#if 0 // これちょっとうざいかも
        // 15時以降に更新したニュースは後でもう一度反復
        if (now > 150000) {
            int from = 210000;
            for (int i = 0; i < num_tweet; i++, from = time_after(from, 10 * 60)) {
                ofs << from / 100 << " ";
                ofs << layout.headline_for_twitter(i, from) << endl;
            }
        }
#endif
    }

    //
    for (int i = 0; i < companybots.size(); i++) {
        string code = companybots[i].first;
        string tweetfile = companybots[i].second;
        cerr << code << " " << tweetfile << endl;
        for (int j = 0; j < articles.size(); j++) {
            if (code == articles[j].code.code) {
                ofstream ofs(tweetfile);
                // まあ手抜きだけど
                for (int k = 0; k < 2; k++) {
                    ofs << (k == 0 ? now / 100 : 2230) << " ";
                    ofs << layout.headline_for_twitter(j, now) << endl;
                }
            }
        }
    }

    // 記事をdatabaseに保存
    for (int i = 0; i < articles.size(); i++) {
        int error = 0;
        {
            stringstream ss;
            ss << "UPDATE articles SET title='" << articles[i].heading << "', subtitle='" << articles[i].sub_heading << "', content='" <<  articles[i].content << "', score=" << articles[i].score << ", change_today=" << articles[i].changeToday << " WHERE code='" << articles[i].code.code << "' AND date=" << today;
            error = db.execute(ss.str());
        }
        if (error) {
            cerr << "CAUGHT!!!\n";
            stringstream ss;
            ss << "INSERT INTO articles VALUES ('" << articles[i].code.code << "'," << today << ",'" << articles[i].heading << "','" << articles[i].sub_heading << "','" << articles[i].content << "', " << articles[i].score << "," << articles[i].changeToday << ")";
            error = db.execute(ss.str());
            if (error) {
                cerr << "NOW WHAT??\n";
            }
        }
    }

    return 0;
}

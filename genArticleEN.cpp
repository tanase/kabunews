#include "genArticleEN.h"

#include "genArticleHelper.h"
#include "env.h"
#include <assert.h>

static const double ENTROPY_THRES = log(8);
static const string COMMA = ", ";
static const string END = ". ";
static const string BY = "で";
static const string IN = "で";
static const string DID_AND = "し、";

static string yen2String(Price yen)
{
    stringstream ss;
    ss << commaNumeralF(yen) << (Env::country == "us" ? "$" : "Yen");
    return ss.str();
}

// n円{高,安}
static string nYenUp2String(Price n)
{
    // !! 乱数で表現変える？
    if (Env::country == "us") {
        char buf[256];
        sprintf(buf, "%s%.2f", n > 0 ? "+" : "", n);
        return buf;
    }
    else {
        stringstream ss;
        if (n > 0) {
            ss << yen2String(n);
            ss << "高";
        }
        else {
            ss << yen2String(-n);
            ss << "安";
        }
        return ss.str();
    }
}

// (見出し用)
static string ratio2headingString(double ratio, int streak)
{
    stringstream ss;
    if (ratio >= 1.12)
        ss << "暴騰！";
    else if (ratio >= 1.05)
        ss << "大幅" << (streak > 1 ? "続伸" : "上昇");
    return ss.str();
}

static string gainString(string type)
{
    if (type == "updown") return "up";
    /*if (type == "gainloss")*/ return "gain";
}

static string lossString(string type)
{
    if (type == "updown") return "down";
    /*if (type == "gainloss")*/ return "loss";
}

// n%{高低}
static string nPercentUp2String(double n, string type = "updown")
{
    stringstream ss;
    ss << formatWithPrecision(100 * fabs(n - 1), 2) << "% "
       << (n > 1 ? gainString(type) : lossString(type));
    return ss.str();
}

// gaining N% .. みたいな付加的なやつ
static string gainingNPercent2String(double n)
{
    stringstream ss;
    ss << (n > 1 ? "gaining" : "losing") << " " << formatWithPrecision(100 * fabs(n - 1), 2) << "%";
    return ss.str();
}

static string nYenPercentUp2String(Price n, double r)
{
    stringstream ss;
    ss << (r > 1 ? "rose" : "fell") << " " << yen2String(abs(n));
    ss << " or " << formatWithPrecision(100 * fabs(r - 1), 2) << "%";
    return ss.str();
}

static string daysPassed2String(int n, bool heading = false)
{
    double month = 365.0/12;
    double half_month = 365.0/24;
    
    stringstream ss;
    if (n <= 10)
        ss << n << " days";
    else if (n < 30) {
        // 2週間, 20日, 3週間のいずれか近いものに
        if (n == 14) ss << "2 weeks";
        else if (n == 20) ss << "20 days";
        else if (n == 21) ss << "3 weeks";
        else {
            if (!heading)
                ss << "about ";
            if (n <= 12) ss << n << " days";
            else if (n <= 17) ss << "2 weeks";
            else if (n <= 24) ss << "3 weeks";
            else ss << "1 month";
        }
    }
    else {
        int t = (int)(n / month + 0.5);
        if (t < 12) {
            t = (int)(n / half_month + 0.5);
            ss << t / 2;
            if (t & 1)
                ss << ".5";
            ss << " month";
            if (t / 2 > 1) ss << "s";
        }
        else {
            ss << t / 12 << " year" << (t / 12 > 1 ? "s" : "");
            if (t % 12 > 0)
                ss << " " << t % 12 << " months";
        }
    }
    return ss.str();
}

// n日連続{上昇,下落}
// !! 適当に「実に10日連続の」とつけたり、連騰とか
// !! 実に、とかは見出しにはふさわしくないか。
static string nDaysUpStreak2String(int n)
{
    stringstream ss;
    //ss << (n > 0 ? "gaining" : "losing") << " " << abs(n) << " days in a row";
    if (abs(n) >= 10)
        ss << "Whopping ";
    ss << abs(n) << " days" << " " << (n > 0 ? "up" : "down") << " streak";
    return ss.str();
}


// 年初来高値
static string yearHigh2String(int today, Stock& stock)
{
    stringstream ss;
    int prev = UNDEF;
    if (stock.ratioToday() > 1) {
        if (stock.yearHigh[0].prevDate != -1)
            prev = stock.yearHigh[0].prevDate;
    }
    else {
        if (stock.yearLow[1].prevDate != -1)
            prev = stock.yearLow[1].prevDate;
    }
    if (prev != UNDEF) {
        int days1 = daysBetween(today / 10000 * 10000 + 101, prev);
        int days2 = daysBetween(prev, today);
        if (days2 == 1) {
            ss << "連日の";
        }
        else if (days1 > 15 && days2 > 15) {
            ss << prev%10000/100 << "月" << prev%100 << "日" << "以来の";
        }
    }
    if (stock.ratioToday() > 1)
        ss << "年初来高値更新";
    else
        ss << "年初来安値更新";
    return ss.str();
}

// n%{高い,安い}水準
static string nPercentHigher2String(double n)
{
    stringstream ss;
    if (n > 1)
        ss << formatWithPrecision(100 * (n - 1), 2) << "%高い水準";
    else
        ss << formatWithPrecision(100 * (1 - n), 2) << "%安い水準";
    return ss.str();
}

static string bounceLevel2String(int level)
{
    if (level >= +3) return "急反発";
    if (level == +2) return "大幅反発";
    if (level == +1) return "反発";
    if (level == -1) return "反落";
    if (level == -2) return "大幅反落";
    if (level <= -3) return "急反落";
    return "";
}

Article genArticleEN(NewsSource& source, Stock& stock, double entropy_yearHigh, double entropy_yearLow)
{
    stringstream ss_heading;
    stringstream ss_subHeading;
    stringstream ss_content;

    long long trade = stock.valueToday.trade();
    Price changeToday = stock.changeToday();
    double ratioToday = stock.ratioToday();
    double ratioToday_modified = stock.ratioToday_modified();
    double ratioYesterday = stock.ratioYesterday();
    int bounce_level = bounceLevel(ratioToday, ratioYesterday);
    double volumeChangeToday = stock.volumeChangeToday();
    // !! stop高とかも含むと思うのでいずれ処理しないといけないんだろう
    if (changeToday == UNDEF || volumeChangeToday == UNDEF)
        return Article();

    int num_trade = source.numSatisfy_trade(trade);
    ; assert(num_trade > 0);
    double entropy_trade = calcEntropy(num_trade, source.stocks.size()) * 1.3;
    
    // 年初来高値(含昨年来高値)、連続上昇率、連続上昇日、前日比上昇率
    // 各entropyソート、上位から文章を構成
    // entropyが基準値に満たない場合は言及しない。
    // 例えば1月10日の年初来高値などは恐らくentropyが低いことが予想される。
    // ex)連続上昇日＞年初来高値＞前日比上昇率
    // h1,h2両方使う場合はh1に日立とつける。
    // 両方使うかどうかは記事間の相対的な順位により決まるのでlayout時点で決まる。
    //    <h1>7日連続の上げ</h1>
    //    <h2>3月12日以来の年初来高値</h2>
    //    <p>日立(6501,東証1部)7日連続の上昇で年初来高値を更新した。</p>
    // 文章の構成:
    //   年初来高値を言及する場合：
    //     日立が{7連騰で、前日比20円上げ、}4月22日以来の年初来高値634円をつけた。
    //     {7日間の上昇率は46%に上る、前日比+13.4%と大きく上昇した}
    //   年初来高値を言及しない場合：
    //     日立が7連騰。この間の上昇率は46%に上る。

    // ストップ高判定いるなあ。

    if (ratioToday == 1) {
        return Article();
    }

    enum {
        CHANGE_TODAY,
        STREAK_INCREASE,
        STREAK_DAYS,
        LAST_PEAK_DATE,
        VOLUME_CHANGE_TODAY,
        YEAR_HIGH,
        RATIO_THIS_YEAR,
        
        NUM_TYPES,
    };

    int satisfy[NUM_TYPES];
    double entropy[NUM_TYPES];

    satisfy[CHANGE_TODAY] = source.numSatisfy_ratioToday_modified(ratioToday_modified);
    entropy[CHANGE_TODAY] = calcEntropy(satisfy[CHANGE_TODAY], source.stocks.size());

    satisfy[STREAK_INCREASE] = source.numSatisfy_streakIncrease_modified(stock.streakIncrease_modified);
    // tweak:
    // 1日で29%上昇、2日で30%上昇というようなのを繰り返し重要ニュースとして流すのを避けるため、
    // その期間の平均上昇率の1/8以下だったら記事の価値を1/10に補正。かなり適当でへぼいけどまあ無いよりはマシだろうということで。
    if (abs(stock.streakDays) > 1) {
        double ave = pow(stock.streakIncrease, 1.0 / abs(stock.streakDays));
        if ((ave > 1 && (ave - 1) > (ratioToday - 1) * 8) || (ave < 1 && (1 - ave) > (1 - ratioToday) * 8)) {
            //cerr << stock.code.shortName << " " << stock.streakIncrease << " ave = " << ave << "(" << stock.streakDays << ")" << " > " << "ratioToday = " << ratioToday << " * 8" << endl;
            satisfy[STREAK_INCREASE] = min(satisfy[STREAK_INCREASE] * 10, (int)source.stocks.size());
        }
    }
    entropy[STREAK_INCREASE] = calcEntropy(satisfy[STREAK_INCREASE], source.stocks.size());

    satisfy[STREAK_DAYS] = source.numSatisfy_streakDays(stock.streakDays);
    entropy[STREAK_DAYS] = calcEntropy(satisfy[STREAK_DAYS], source.stocks.size());

    satisfy[LAST_PEAK_DATE] = source.numSatisfy_lastPeak(stock.lastPeakDate);
    entropy[LAST_PEAK_DATE] = calcEntropy(satisfy[LAST_PEAK_DATE], source.stocks.size());
    
    satisfy[VOLUME_CHANGE_TODAY] = source.numSatisfy_volumeChangeToday(volumeChangeToday);
    entropy[VOLUME_CHANGE_TODAY] = calcEntropy(satisfy[VOLUME_CHANGE_TODAY], source.stocks.size());

    double maxEntropy = 0;
    int mostInteresting = -1;

    if (entropy[CHANGE_TODAY] > maxEntropy) {
        maxEntropy = entropy[CHANGE_TODAY];
        mostInteresting = CHANGE_TODAY;
    }
    if (entropy[STREAK_INCREASE] > maxEntropy && stock.streakDays > 1) {
        maxEntropy = entropy[STREAK_INCREASE];
        mostInteresting = STREAK_INCREASE;
    }
    if (entropy[STREAK_DAYS] > maxEntropy && abs(stock.streakDays) > 1) {
        maxEntropy = entropy[STREAK_DAYS];
        mostInteresting = STREAK_DAYS;
    }
    if (entropy[LAST_PEAK_DATE] > maxEntropy && stock.lastPeakDate != UNDEF) {
        maxEntropy = entropy[LAST_PEAK_DATE];
        mostInteresting = LAST_PEAK_DATE;
    }
    if (ratioToday > 1 && stock.yearHigh[0].date == source.today && entropy_yearHigh > ENTROPY_THRES) {
        maxEntropy = entropy_yearHigh;
        mostInteresting = YEAR_HIGH;
    }
    if (ratioToday < 1 && stock.yearLow[1].date ==  source.today && entropy_yearLow > ENTROPY_THRES) {
        maxEntropy = entropy_yearLow;
        mostInteresting = YEAR_HIGH;
    }

    if (mostInteresting == CHANGE_TODAY) {
        ss_heading << nPercentUp2String(ratioToday);
    }
    else if (mostInteresting == STREAK_INCREASE) {
        ss_heading << nPercentUp2String(stock.streakIncrease) << " in " << stock.streakDays << " days";
    }
    else if (mostInteresting == STREAK_DAYS) {
        ss_heading << nDaysUpStreak2String(stock.streakDays);
    }
    else if (mostInteresting == LAST_PEAK_DATE) {
        ss_heading << daysPassed2String(daysBetween(abs(stock.lastPeakDate), source.today), true) << " " << (ratioToday > 1 ? "high" : "low");
    }

    ss_content << nYenPercentUp2String(changeToday, ratioToday);
    // TODO: after losing 4 days.
    if (abs(bounce_level) > 0) {
        ss_content << " after previous " << nPercentUp2String(ratioYesterday, "gainloss");
    }
    if (entropy[LAST_PEAK_DATE] > ENTROPY_THRES && stock.lastPeakDate != UNDEF) {
        ss_content << ", trading at " << daysPassed2String(daysBetween(abs(stock.lastPeakDate), source.today)) << " " << (ratioToday > 1 ? "high" : "low");
        ss_content << ", at ";
    }
    else {
        ss_content << ", trading at ";
    }
    ss_content << yen2String(stock.valueToday.end);
    ss_content << END;
    if (abs(stock.streakDays) > 1) {
        ss_content << nDaysUpStreak2String(stock.streakDays);
        // ５連騰＆その間にN％上昇
        if (abs(stock.streakDays) > 1 && entropy[STREAK_INCREASE] > ENTROPY_THRES && fabs(stock.streakIncrease - 1) > fabs(ratioToday - 1) * 1.5) {
            ss_content << COMMA;
            ss_content << gainingNPercent2String(stock.streakIncrease);
            ss_content << " meanwhile" << END;
        }
        else {
            ss_content << END;
        }
    }

    return Article(stock.code, ss_heading.str(), ss_subHeading.str(), ss_content.str(), entropy_trade + maxEntropy, changeToday);    
}

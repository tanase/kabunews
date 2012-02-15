#include "env.h"
#include "strings.h"
#include <sstream>

string I19_date2str(int year, int month, int day)
{
    stringstream ss;
    if (Env::lang == "en") {
        const char* monStr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        ss << monStr[month-1] << " " << day << ", " << year;
    }
    else {
        ss << year << "年" << month << "月" << day << "日";
    }
    return ss.str();
}

string I19_winnersRanking(int year)
{
    stringstream ss;
    if (Env::lang == "en") {
        ss << year << " Winners";
        return ss.str();
    }
    // yearくっつけると長くなりすぎる
    return "勝ち組ランキング";
}

string I19_losersRanking(int year)
{
    stringstream ss;
    if (Env::lang == "en") {
        ss << year << " Losers";
        return ss.str();
    }
    return "負け組ランキング";
}

string I19_about()
{
    if (Env::country == "us") {
        if (Env::lang == "en") {
            return "Great summary of what happened in the stock market today.<br />";
        }
        else {
            return "日本語で読む<br /><strong>米国株ニュース</strong><br />";
        }
    }
    else {
        return "<strong>個人投資家</strong>のための<br /><strong>無料株式ニュース</strong><br />"        ;
    }
}

string I19_detailedAbout()
{
    if (Env::country == "us") {
        if (Env::lang == "en") {
            return "";
        }
        else {
            return "全ての米国株の値動きを元に新聞風の記事を作り、重要そうな順に並べています。銘柄選びの参考にしていただければ幸いです。<br />";
        }
    }
    else {
        return "全上場銘柄の値動きを元に新聞風の記事を作り、重要そうな順に並べています。銘柄選びの参考にしていただければ幸いです。<br />";
    }
}

string I19_latestIssue()
{
    if (Env::lang == "en") return "latest";
    return "最新号";
}

string I19_backnumber()
{
    if (Env::lang == "en") return "back issues";
    return "バックナンバー";
}

string I19_updateTwice()
{
    if (Env::country == "us") {
        if (Env::lang == "en") {
            return "Updated about once per hour!";
        }
        else {
            return "大体1時間に1回更新します。";
        }
    }
    else {
        return "1日2回(前・後場終了後)更新！";
    }
}

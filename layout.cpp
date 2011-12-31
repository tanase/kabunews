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
#include "layout.h"
#include "util.h"
#include "env.h"

using namespace layout;

extern double utf8len2(const string &s);

static int date2year(int date) { return date / 10000; }
static int date2month(int date) { return date % 10000 / 100; }
static int date2day(int date) { return date % 100; }

static string css_gradient(bool vertical, int from, int to)
{
    char buf[200];
    sprintf(buf, "background: -moz-linear-gradient(top, #%06x, #%06x);background: -webkit-gradient(linear, left top, left bottom, from(#%06x), to(#%06x));", from, to, from, to);
    return buf;
}

static string css_textWithShadow(int color, int shadow_margin, int shadow_color)
{
    char buf[200];
    sprintf(buf, "color:#%06x; text-shadow:%dpx %dpx %dpx #%06x;", color, shadow_margin, shadow_margin, shadow_margin, shadow_color);
    return buf;
}

static int randFromStr(string str)
{
    int r = 0;
    for (int i = 0; i < str.size(); i++)
        r += str[i];
    return r;
}

static string date2str1(int date)
{
    stringstream ss;
    ss << date2year(date) << "年" << date2month(date) << "月" << date2day(date) << "日";
    return ss.str();
}

static string date2str2(int date)
{
    stringstream ss;
    ss << date2year(date) << "." << date2month(date) << "." << date2day(date);
    return ss.str();
}

static string now2str(int now)
{
    char buf[100];
    sprintf(buf, "%d時%d分", now / 10000, now % 10000 / 100);
    return string(buf);
}

string code2string(int code)
{
    stringstream ss;
    ss << code;
    return ss.str();
}

void html_out(string str) {
    cout << str << endl;
}

template<typename T>
void html_tag(string tag, T content, string klass="", string style="") {
    cout << "<" << tag;
    if (klass != "") cout << " class=\"" << klass << "\"";
    if (style != "") cout << " style=\"" << style << "\"";
    cout << ">";
    cout << content;
    cout << "</" << tag << ">";
}

template<typename T>
void html_out_type(T n) {
    cout << n << endl;
}

// strが日本語を仮定
void html_out_vertical(string str) {
    for (int i = 0; i < str.size(); i+=3) {
        cout << str.substr(i, 3) << "<br />" << endl;
    }
}

void html_p(string str, int font_size = 13) {
    cout << "<p";
//    if (font_size != 13)
//        cout << " style=\"font-size:" << font_size << "px;\"";
    cout << ">" << str << "</p>" << endl;
}

void html_htmlStart() {
    cout << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"" << endl;
    cout << "       \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
    cout << "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">" << endl;
}
void html_htmlStop() {
    cout << "</html>" << endl;
}

void html_headStart() {
    cout << "<head>" << endl;
}
void html_headStop() {
    cout << "</head>" << endl;
}

void html_meta() {
    cout << "<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\" />" << endl;
    cout << "<meta name=\"format-detection\" content=\"telephone=no\" />" << endl;
}
void html_loadCSS() {
    cout << "<link href=\"reset.css\" media=\"screen\" rel=\"stylesheet\" type=\"text/css\" />" << endl;
    cout << "<link href=\"main.css\" media=\"screen\" rel=\"stylesheet\" type=\"text/css\" />" << endl;
}
void html_title(int date) {
    cout << "<title>" << TITLE << " " << date2str1(date) << " 〜 " << SUBTITLE << "</title>" << endl;
}

void html_bodyStart() {
    cout << "<body>" << endl;
}
void html_bodyStop() {
    cout << "</body>" << endl;
}

void html_divIdStart(string id, int* info) {
    cout << "<div id=\"" << id << "\" style=\"width:" << info[3] << "px;border:" << info[1] << "px solid #000;padding:" << info[2] << "px;"
         << (/*Env::country == "us" ? css_gradient(true, 0x80AAEE, 0xCCCCFF) : */"background-color: #FEFDFD;")
//         << Env::country == "us" ? "color:#0000CC;" : ""
         << "\">" << endl;
}
void html_divIdStop() {
    cout << "</div>" << endl;
}

void html_topTitle(int date) {
    cout << "<div class=\"bg\" style=\"text-align:center;font-weight:bold;font-size:12px;letter-spacing:1.5em;margin-top:-10px;\">" << endl;
    cout << date2str1(date) << " " << TITLE << endl;
    cout << "</div>" << endl;
}

void html_header(int date, int now) {
    html_out("<div id=\"header\" style=\"background-color:transparent;\">");
    int from = 0xFFFFFF, to = 0xDDDDFF;
    int color = 0x000000;
    /*
    if (Env::country == "us")
        color = 0xFF0000;
    */
    html_out(string("<div id=\"logo\" style=\"font-size:50px;padding:20px 0;") + css_textWithShadow(0x000000, 4, 0xAAAAAA) + css_gradient(true, from, to) + "\">");
    html_out_vertical(TITLE);
    html_out("</div>");
    html_out("<p style=\"margin-bottom:4px;\">");
    html_out("<span style=\"letter-spacing:-0.05em\">" + date2str1(date) + "</span>" + "版" + "<br />");
    if (now >= 0)
        cout << "最終更新:" << now2str(now) << "<br />" << endl;
    if (Env::country == "us")
        html_out("日本語で読む<br /><strong>米国株ニュース</strong><br />");
    else
        html_out("<strong>個人投資家</strong>のための<br /><strong>無料株式ニュース</strong><br />");
    html_out("<a href=\"today.html\">最新号</a><br />");
    html_out("<a href=\"back.html\">バックナンバー</a><br />");
    html_out("</p>");

    html_out("<p style=\"margin-bottom:4px;width:120px;text-align:left;font-size:10px;color:#333;\">");
    html_out("全上場銘柄の値動きを元に新聞風の記事を作り、重要そうな順に並べています。銘柄選びの参考にしていただければ幸いです。<br />");
    html_out("1日2回(前・後場終了後)更新！");
    html_out("</p>");
    
    loadFile("ad_120x600");

    html_out("</div>");
}

void html_copyright() {
    html_out("<div class=\"bg\" style=\"clear:both;text-align:right;font-size:12px;margin-bottom:-10px;\">");
    html_out(string("Copyright © 2011 ") + TITLE + "社(ご意見・ご要望は<a href=\"http://twitter.com/kabushinbun\">@kabushinbun</a>へお願いします)");
    html_out("</div>");
}

int html_rowStart(int W, int margin = 0) {
    cout << "<div class=\"row\" style=\"width:" << W << "px;";
    if (margin > 0) cout << "padding-bottom:" << margin << "px;";
    cout << "\">" << endl;
    return W;
}
void html_rowStop() {
    cout << "</div>" << endl;
}

int html_colStart(int W, int margin = 0) {
    W -= margin;
    cout << "<div class=\"column\" style=\"width:" << W << "px;";
    if (margin > 0) cout << "padding-right:" << margin << "px;";
    cout << "\">" << endl;
    return W;
}
void html_colStop() {
    cout << "</div>" << endl;
}

void html_h1(string str, int W) {
    double len = utf8len2(str);
    int pad = min(10, W / 20);
    int font_size = (int)((W - pad * 2) / len);
    cout << "<h1 style=\"font-size:" << font_size << "px;line-height:1;padding:" << pad << "px;\">" << str << "</h1>" << endl;
}

void html_h2(string str, int W, int level, string extra_style="") {
    int len = utf8len2(str);
    int pad = min(2, W / 20);
    int fsmin = 12, fsmax = 16;
    if (level <= 1) extra_style = ""; // 新聞らしさを残す
    if (level == 2) {
        fsmin = 12;
        fsmax = 18;
    }
    // firefoxのline-spacingが大きいので適当に調節
    int font_size = max(fsmin, min(fsmax, (W - pad * 2 - 2/* =border */) / len * 8 / 9));
    cout << "<h2 style=\"" << (extra_style != "" ? extra_style + ";" : "") << "font-size:" << font_size << "px;line-height:1;padding:" << pad << "px;" << "\">" << str << "</h2>" << endl;
}

void html_tableStart() {
    html_out("<table>");
}

void html_tableStop() {
    html_out("</table>");
}

void html_trStart(int font_size = 12) {
    stringstream ss;
    ss << "<tr style=\"font-size:" << font_size << "px;\">";
    html_out(ss.str());
}

void html_trStop() {
    html_out("</tr>");
}

void html_td(string str, string style) {
    html_out(string("") + "<td style=\"" + style + "\">" + str + "</td>");
}

void html_th(string str, string style) {
    html_out(string("") + "<th style=\"" + style + "\">" + str + "</th>");
}

void html_divClassStart(string str) {
    cout << "<div class=\"" << str << "\">" << endl;
}
void html_divClassStop() {
    cout << "</div>" << endl;
}

void html_articleStart(int level) {
    cout << "<div class=\"article lv" << level << "\">" << endl;
}
void html_articleStop() {
    cout << "</div>" << endl;
}


Layout::Layout() {}

Layout::Layout(int today_, int now_, vector<Article> articles_, vector<Summary> summary_, pair<Ranking, Ranking> yearWinnerLoserRankings_)
    : today(today_), now(now_),
      articles(articles_.begin(), articles_.end()),
      summary(summary_.begin(), summary_.end()),
      yearWinnerLoserRankings(yearWinnerLoserRankings_),
      used(0)
{
    elements[WRAPPER_WIDTH]      = 1000;
    elements[WRAPPER_BORDER]     = 1;
    elements[WRAPPER_PADDING]    = 20;
    elements[WRAPPER_HEIGHT]     = 1000;
    elements[FOOTER_PADDING]     = 3;
    elements[HEADER_INNER_WIDTH] = 120/*75*/ + 2 * 2;
    elements[HEADER_BORDER]      = 0;
    elements[HEADER_PADDING]     = 8;
    elements[CONTAINER_BORDER]   = 1;
    elements[CONTAINER_PADDING]  = 8;
    elements[PRIMARY_BORDER]     = 0;
    elements[PRIMARY_PADDING]    = 0;
    elements[SECONDARY_BORDER]   = 0;
    elements[SECONDARY_PADDING]  = 0;
    calc();
}

void Layout::calc()
{
    elements[WRAPPER_INNER_WIDTH]    = elements[WRAPPER_WIDTH] - (elements[WRAPPER_BORDER] + elements[WRAPPER_PADDING]) * 2;
    elements[WRAPPER_INNER_HEIGHT]   = elements[WRAPPER_HEIGHT] - (elements[WRAPPER_BORDER] + elements[WRAPPER_PADDING]) * 2;
    elements[FOOTER_HEIGHT]          = elements[WRAPPER_PADDING];
    elements[FOOTER_INNER_HEIGHT]    = elements[FOOTER_HEIGHT] - elements[FOOTER_PADDING] * 2;
    elements[HEADER_WIDTH]           = elements[HEADER_INNER_WIDTH] + (elements[HEADER_BORDER] + elements[HEADER_PADDING]) * 2;
    elements[CONTAINER_WIDTH]        = elements[WRAPPER_INNER_WIDTH] - elements[HEADER_WIDTH];
    elements[CONTAINER_INNER_WIDTH]  = elements[CONTAINER_WIDTH] - (elements[CONTAINER_BORDER] + elements[CONTAINER_PADDING]) * 2;
    elements[CONTAINER_HEIGHT]       = elements[WRAPPER_INNER_HEIGHT];
    elements[CONTAINER_INNER_HEIGHT] = elements[CONTAINER_HEIGHT] - (elements[CONTAINER_BORDER] + elements[CONTAINER_PADDING]) * 2;
    elements[PRIMARY_WIDTH]          = (int)(elements[CONTAINER_INNER_WIDTH] * 0.67);
    elements[PRIMARY_INNER_WIDTH]    = elements[PRIMARY_WIDTH] - (elements[PRIMARY_BORDER] + elements[PRIMARY_PADDING]) * 2;
    elements[SECONDARY_WIDTH]        = elements[CONTAINER_INNER_WIDTH] - elements[PRIMARY_WIDTH];
    elements[SECONDARY_INNER_WIDTH]  = elements[SECONDARY_WIDTH] - (elements[SECONDARY_BORDER] + elements[SECONDARY_PADDING]) * 2;
}

void Layout::renderHeadline(int W)
{
    html_rowStart(W);
    html_articleStart(1);
    html_h1(articles[0].prefixSimple() + articles[0].heading, W);
    if (articles[0].sub_heading != "") {
        html_divClassStart("subh");
        html_out(articles[0].sub_heading);
        html_divClassStop();
    }
    html_p(articles[0].prefix() + articles[0].content, 20);
    html_articleStop();
    html_rowStop();
    used = 1;
}

void Layout::renderArticle(int W, int level)
{
    if (used >= articles.size()) return;
    html_rowStart(W);
    html_articleStart(level);
    string style;
    if (level == 2) {
        string border = "border:1px solid #CCC; border-right: 1px solid #999; border-bottom: 1px solid #999;";
        string background = "background-color:#FCFCFC;";
        style = border + background;
        if (articles[used].changeToday >= 0)
            style += "color:#080;";
        else
            style += "color:#800;";
    }    
    else if (level == 3) {
        if (articles[used].changeToday >= 0)
            style = "background-color:#080; color:#FFF";
        else
            style = "background-color:#800; color:#FFF";
    }
    html_h2(articles[used].prefixSimple() + articles[used].heading, W, level, style);
    html_p(articles[used].prefix() + articles[used].content);
    html_articleStop();
    html_rowStop();
    used ++;
}

void Layout::renderLevel2Articles(int numRows, int numCols)
{
    html_rowStart(elements[CONTAINER_INNER_WIDTH], 10);
    loadFile("link_728x15");
    html_rowStop();
    
    for (int i = 0; i < numRows; i++) {
        html_rowStart(elements[CONTAINER_INNER_WIDTH], elements[CONTAINER_PADDING]);
        int width = (elements[CONTAINER_INNER_WIDTH] + elements[CONTAINER_PADDING]) / numCols;
        for (int j = 0; j < numCols; j++) {
            int W;
            if (j == numCols - 1)
                W = html_colStart(width - elements[CONTAINER_PADDING], 0);
            else
                W = html_colStart(width, elements[CONTAINER_PADDING]);
            renderArticle(W, 2);
            html_colStop();
        }
        html_rowStop();
    }
}

void Layout::renderLevel3Articles(int numRows, int numCols)
{
    html_rowStart(elements[CONTAINER_INNER_WIDTH], 10);
    loadFile("link_728x15");
    html_rowStop();
    
    int adRow = 3 + randFromStr(articles[0].code.code) % 8;
    for (int i = 0; i < numRows; i ++) {
        
        if (i == adRow) {
            html_rowStart(elements[CONTAINER_INNER_WIDTH], elements[CONTAINER_PADDING]);
            loadFile("ad_728x90");
            html_rowStop();
        }

        html_rowStart(elements[CONTAINER_INNER_WIDTH]);

        int width = (elements[CONTAINER_INNER_WIDTH] + elements[CONTAINER_PADDING]) / numCols;

        for (int j = 0; j < numCols; j++) {
            int W;
            if (j == numCols - 1)
                W = html_colStart(width - elements[CONTAINER_PADDING], 0);
            else
                W = html_colStart(width, elements[CONTAINER_PADDING]);
            try {
                renderArticle(W, 3);
            }
            catch (...) {}
            html_colStop();
        }
        
        html_rowStop();
    }

    html_rowStart(elements[CONTAINER_INNER_WIDTH]);
    loadFile("link_728x15");
    html_rowStop();
}

string Layout::url() const
{
    char buf[100];
    sprintf(buf, "%d%02d%02d", date2year(today), date2month(today), date2day(today));
    return string("http://sugyoku.jp/") + buf + ".html";
}

int Layout::render()
{
    cerr << "aricles.size() = " << articles.size() << endl;
    if (articles.size() == 0) {
        cerr << "no article to be rendered" << endl;
        return 1;
    }
    
    int W;
    
    html_htmlStart();
    html_headStart();
    html_meta();
    html_loadCSS();
    html_title(today);
    loadFile("analytics");
    html_headStop();

    html_bodyStart();
    
    html_divIdStart("wrapper", elements + WRAPPER);
    html_topTitle(today);

    html_out("<div style=\"float:left;width:120px;\">");
        
    html_header(today, now);

    for (int group = 0; group < 2; group++) {
        html_out("<div style=\"float:left; margin-top:10px;\">");
        html_tag("h3", group == 0 ? "勝ち組ランキング" : "負け組ランキング");
        html_out("<table style=\"font-size:11px;\">");
        Ranking& stocks = (group == 0 ? yearWinnerLoserRankings.first : yearWinnerLoserRankings.second);
        char buf[200];
        for (int i = 0; i < stocks.codes.size(); i++) {
            html_out("<tr>");
            if ((group == 0 && stocks.codes[i].first <= 1) || (group == 1 && stocks.codes[i].first >= 1)) break;
            int ratio = (stocks.codes[i].first) * 100 + 0.5;
            string code = stocks.codes[i].second.code;
            string name = stocks.codes[i].second.shortName;
            sprintf(buf, "%d.", i + 1);
            html_tag("td", buf, "", "text-align:right;padding-right:2px;");
            sprintf(buf, "%d%%", ratio);
            html_tag("td", buf, group == 0 ? "up" : "down", "padding-right: 2px;");
            html_tag("td", urlwrap(yahooURL(code), name));
            html_out("</tr>");
        }
        html_out("</table>");
        html_out("</div>");
    }

    html_out("</div>");

    

    html_divIdStart("container", elements + CONTAINER);
    html_rowStart(elements[CONTAINER_INNER_WIDTH], 10);
    W = html_colStart(elements[PRIMARY_INNER_WIDTH], elements[CONTAINER_PADDING]);
    renderHeadline(W);
    html_colStop();

    W = html_colStart(elements[SECONDARY_INNER_WIDTH]);
    html_rowStart(W);
    loadFile("twitter", url());
    loadFile("facebook", url());
    html_rowStop();
    html_rowStart(W);
    loadFile("ad_234x60");
    html_rowStop();
    html_tableStart();
    for (int i = 0; i < summary.size(); i ++) {
        html_trStart(15);
        html_th(summary[i].code.shortName, "font-weight:bold;");
        html_td(summary[i].content, "padding-left:6px;");
        html_trStop();
    }
    html_tableStop();
    html_colStop();
    html_rowStop();

    renderLevel2Articles(2, 2);

    renderLevel3Articles(49, 4);

    html_divIdStop();           // container

    html_copyright();
    
    html_divIdStop();           // wrapper
    
    html_bodyStop();
    html_htmlStop();
    return 0;
}

unsigned int Layout::hash() const {
    unsigned int res = 0;
    for (vector<Article>::const_iterator it = articles.begin(); it != articles.end(); it++)
        res ^= it->hash();
    for (vector<Summary>::const_iterator it = summary.begin(); it != summary.end(); it++)
        res ^= it->hash();
    return res;
}

string Layout::headline(int idx) const {
    return articles[idx].prefixSimple() + articles[idx].heading;
}

static bool time_just_updated(int now) {
    if (now < 170000) return true;
    return false;
}

static bool time_is_morning(int now) {
    if (now != -1 && now < 150000) return true;
    return false;
}

string Layout::headline_for_twitter(int i, int now) const {
    stringstream ss;
    if (i == 0) {
        ss << (time_just_updated(now) ? string(TITLE) + "を更新しました！ " : "")
           << (time_is_morning(now) ? "前場の" : "今日の")
           << "トップニュースは"
           << "「" << headline(0) << "」"
           << (time_just_updated(now) ? "です。" : "でした。")
           << url();
    }
    else {
        ss << (time_is_morning(now) ? "前場の" : "今日の")
           << "重要ニュース 第" << i + 1 << "位は"
           << "「" << headline(i) << "」"
           << (time_just_updated(now) ? "です。" : "でした。")
           << url();
    }
    return ss.str();
}

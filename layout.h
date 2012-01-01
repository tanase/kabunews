#ifndef LAYOUT_H
#define LAYOUT_H

#include <string>
#include "article.h"

#define TITLE (Env::country == "us" ? "アメリカ株新聞" : "株色新聞")
#define SUBTITLE (Env::country == "us" ? "アメリカ株情報を日本語で" : "株式投資のお供に")
#define URL (string("http://") + (Env::country == "us" ? (string("us.sugyoku.com") + (Env::lang == "jp" ? "/jp" : "")) : string("jp.sugyoku.com")))

using namespace std;

namespace layout {
    typedef enum {
        WRAPPER,
        WRAPPER_WIDTH = WRAPPER,
        WRAPPER_BORDER,
        WRAPPER_PADDING,
        WRAPPER_INNER_WIDTH,
        WRAPPER_HEIGHT,
        WRAPPER_INNER_HEIGHT,
        FOOTER_HEIGHT,
        FOOTER_PADDING,
        FOOTER_INNER_HEIGHT,
        HEADER_WIDTH,
        HEADER_BORDER,
        HEADER_PADDING,
        HEADER_INNER_WIDTH,
        CONTAINER,
        CONTAINER_WIDTH = CONTAINER,
        CONTAINER_BORDER,
        CONTAINER_PADDING,
        CONTAINER_INNER_WIDTH,
        CONTAINER_HEIGHT,
        CONTAINER_INNER_HEIGHT,
        PRIMARY_WIDTH,
        PRIMARY_BORDER,
        PRIMARY_PADDING,
        PRIMARY_INNER_WIDTH,
        PRIMARY_HEIGHT,
        PRIMARY_INNER_HEIGHT,
        SECONDARY_WIDTH,
        SECONDARY_BORDER,
        SECONDARY_PADDING,
        SECONDARY_INNER_WIDTH,
        
        NUM_ELEMENTS
    } Element;
}

class Layout {
    int today;
    int now;
    vector<Article> articles;
    vector<Summary> summary;
    pair<Ranking, Ranking> yearWinnerLoserRankings;
    void calc();
    int elements[layout::NUM_ELEMENTS];
    int used;
public:
    int size( layout::Element type ) const {
        return elements[type];
    }
    int setSize( layout::Element type, int size ) {
        elements[type] = size;
        calc();
    }
    
    Layout();
    Layout(int today_, int now_, vector<Article> articles_, vector<Summary> summary_, pair<Ranking, Ranking> yearWinnerLoserRankings_);
    int render();
    void renderHeadline( int W );
    void renderArticle( int W, int level );
    void renderLevel2Articles(int numRows, int numCols);
    void renderLevel3Articles(int numRows, int numCols);
    string url() const;
    unsigned int hash() const;
    string headline(int i) const;
    string headline_for_twitter(int i, int now) const;
};

#endif  /* LAYOUT_H */

#ifndef HTML_H
#define HTML_H

class HTML {
public:
    void render() = 0;
};

class HTML_htmlStart : public HTML {
    void render();
};

#endif  /* HTML_H */

#ifndef RETUYUURO_HTML_HPP
#define RETUYUURO_HTML_HPP

#include <retuyuuro/HtmlElement.hpp>

namespace retuyuuro {

struct HtmlPage : HtmlElement {
  HtmlPage() : HtmlElement("html", false) {}

  void open(String &out) override {
    out += "<!doctype html>";
    HtmlElement::open(out);
  }
};

#define RYR_HTML_DEF(name, tag, is_void)                                       \
  struct name : HtmlElement {                                                  \
    name() : HtmlElement(tag, is_void) {}                                      \
  }

RYR_HTML_DEF(HtmlHead, "head", false);
RYR_HTML_DEF(HtmlBody, "body", false);
RYR_HTML_DEF(HtmlParagraph, "p", false);
RYR_HTML_DEF(HtmlBold, "b", false);
RYR_HTML_DEF(HtmlMeta, "meta", true);

#undef RYR_HTML_DEF

struct RawText : HtmlElement {
  String text;

  explicit RawText(String text)
      : HtmlElement("", false), text(std::move(text)) {}

  void render(String &out) override { out += text; }
};


}

#endif


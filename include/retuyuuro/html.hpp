#ifndef RETUYUURO_HTML_HPP
#define RETUYUURO_HTML_HPP

#include <retuyuuro/HtmlElement.hpp>

namespace retuyuuro {

#define RYR_HTML_DEF(name, tag, is_void)                                       \
  struct name : HtmlElement {                                                  \
    name(Attributes attributes = {}) : HtmlElement(tag, is_void, attributes) {}     \
  }

RYR_HTML_DEF(HtmlHead, "head", false);
RYR_HTML_DEF(HtmlBody, "body", false);
RYR_HTML_DEF(HtmlParagraph, "p", false);
RYR_HTML_DEF(HtmlDiv, "div", false);
RYR_HTML_DEF(HtmlImage, "img", true);
RYR_HTML_DEF(HtmlHeader, "header", false);
RYR_HTML_DEF(HtmlFooter, "footer", false);
RYR_HTML_DEF(HtmlNav, "nav", false);
RYR_HTML_DEF(HtmlBold, "b", false);
RYR_HTML_DEF(HtmlMeta, "meta", true);
RYR_HTML_DEF(HtmlTitle, "title", false);

RYR_HTML_DEF(HtmlHeading1, "h1", false);
RYR_HTML_DEF(HtmlHeading2, "h2", false);
RYR_HTML_DEF(HtmlHeading3, "h3", false);

#undef RYR_HTML_DEF

struct HtmlPage : HtmlElement {
  HtmlHead *head;
  HtmlBody *body;

  HtmlPage() : HtmlElement("html", false) {
    head = new HtmlHead();
    this->children.push_back(head);

    body = new HtmlBody();
    this->children.push_back(body);
  }

  void open(String &out) override {
    out += "<!doctype html>";
    HtmlElement::open(out);
  }
};

struct HtmlAnchor : HtmlElement {
  HtmlAnchor(String href) : HtmlElement("a", false) {
    this->set_attribute("href", href);
  }
};

struct HtmlStylesheet : HtmlElement {
  HtmlStylesheet(String href) : HtmlElement("link", true) {
    this->set_attribute("rel", "stylesheet");
    this->set_attribute("href", href);
  }
};

struct RawText : HtmlElement {
  String text;

  explicit RawText(String text)
      : HtmlElement("", false), text(std::move(text)) {}

  void render(String &out, String lang) override {
    out += text;
    for (auto *child : children)
      child->render(out, lang);
  }
};

} // namespace retuyuuro

#endif

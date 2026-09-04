#include <print>

#include <retuyuuro/URLNode.hpp>

using namespace retuyuuro;


struct WebElement {
  String tag = "";
  bool is_void = false;

  Vector<WebElement *> children;

  WebElement(String tag, bool is_void)
    : tag(std::move(tag)), is_void(is_void) {}

  virtual void open(String &out) {
    out += "<" + tag;

    if (this->is_void) out += " /";
    out += ">";
  }

  virtual void close(String &out) {
    if (!this->is_void)
      out += "</" + tag + ">";
  }

  virtual void render(String &out) {
    this->open(out);

    for (auto *child : this->children)
      child->render(out);

    this->close(out);
  }
};

struct HtmlPage : WebElement {
};

int main() {

  WebElement div("div", false);
  WebElement img("img", true);

  div.children.push_back(&img);

  String out;
  div.render(out);

  std::println("{}", out);

  return 0;
}

#ifndef RETUYUURO_HTMLELEMENT_HPP
#define RETUYUURO_HTMLELEMENT_HPP

#include <retuyuuro/URLNode.hpp>

namespace retuyuuro {

using Attributes = OrderedMap<String, String>;

struct HtmlElement {
  String tag;
  bool is_void = false;

  Vector<HtmlElement *> children;

  Attributes attributes;

  Vector<String> boolean_attributes;

  HtmlElement(String tag, bool is_void = false, Attributes attributes = {})
      : tag(std::move(tag)), is_void(is_void), attributes(attributes) {}

  bool owns_children = true;

  HtmlElement(const HtmlElement &other)
      : tag(other.tag), is_void(other.is_void), children(other.children),
        attributes(other.attributes),
        boolean_attributes(other.boolean_attributes), owns_children(false) {}

  HtmlElement &operator=(const HtmlElement &other) {
    if (this == &other)
      return *this;

    if (owns_children) {
      for (auto *child : children)
        delete child;
    }

    tag = other.tag;
    is_void = other.is_void;
    children = other.children;
    attributes = other.attributes;
    boolean_attributes = other.boolean_attributes;
    owns_children = false;

    return *this;
  }

  virtual ~HtmlElement() {
    if (!owns_children)
      return;

    for (auto *child : children)
      delete child;
  }

  static void escape_attribute(const String &value, String &out) {
    for (const auto c : value) {
      switch (c) {
      case '&':
        out += "&amp;";
        break;

      case '"':
        out += "&quot;";
        break;

      case '<':
        out += "&lt;";
        break;

      case '>':
        out += "&gt;";
        break;

      default:
        out += c;
        break;
      }
    }
  }

  HtmlElement &set_attribute(String name, String value) {
    attributes[std::move(name)] = std::move(value);
    return *this;
  }

  HtmlElement &set_boolean_attribute(String name) {
    boolean_attributes.push_back(std::move(name));
    return *this;
  }

  virtual void open(String &out) {
    out += '<';
    out += tag;

    for (const auto &[name, value] : attributes) {
      out += ' ';
      out += name;
      out += "=\"";

      escape_attribute(value, out);

      out += '"';
    }

    for (const auto &name : boolean_attributes) {
      out += ' ';
      out += name;
    }

    out += '>';
  }

  virtual void close(String &out) {
    if (!is_void)
      out += "</" + tag + ">";
  }

  template <typename T, typename... Args> T *add(Args &&...args) {
    T *element = new T(std::forward<Args>(args)...);
    children.push_back(element);
    return element;
  }

  virtual void render(String &out, String lang = "en") {
    open(out);

    for (auto *child : children)
      child->render(out, lang);

    close(out);
  }
};

} // namespace retuyuuro

#endif

#ifndef RETUYUURO_HTMLELEMENT_HPP
#define RETUYUURO_HTMLELEMENT_HPP

#include <retuyuuro/URLNode.hpp>

namespace retuyuuro {

struct HtmlElement {
  String tag;
  bool is_void = false;

  Vector<HtmlElement *> children;

  OrderedMap<String, String> attributes;

  Vector<String> boolean_attributes;

  HtmlElement(String tag, bool is_void = false)
      : tag(std::move(tag)), is_void(is_void) {}

  HtmlElement(const HtmlElement &) = delete;
  HtmlElement &operator=(const HtmlElement &) = delete;

  HtmlElement(HtmlElement &&other) noexcept
      : tag(std::move(other.tag)), is_void(other.is_void),
        children(std::move(other.children)),
        attributes(std::move(other.attributes)),
        boolean_attributes(std::move(other.boolean_attributes)) {
    other.children.clear();
  }

  HtmlElement &operator=(HtmlElement &&other) noexcept {
    if (this == &other)
      return *this;

    for (auto *child : children)
      delete child;

    tag = std::move(other.tag);
    is_void = other.is_void;
    children = std::move(other.children);
    attributes = std::move(other.attributes);
    boolean_attributes = std::move(other.boolean_attributes);

    other.children.clear();

    return *this;
  }

  virtual ~HtmlElement() {
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

    // Normal attributes.
    for (const auto &[name, value] : attributes) {
      out += ' ';
      out += name;
      out += "=\"";

      escape_attribute(value, out);

      out += '"';
    }

    // Boolean attributes
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

  virtual void render(String &out) {
    open(out);

    for (auto *child : children)
      child->render(out);

    close(out);
  }
};

} // namespace retuyuuro
#endif

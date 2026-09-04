#ifndef RAIREN_RETUYUURO_URLNODE_HPP
#define RAIREN_RETUYUURO_URLNODE_HPP

#include <ranges>

#include <rairen/types.hpp>

namespace retuyuuro {

using namespace rairen;

struct URLNode {
  String name;
  URLNode *parent = nullptr;
  Vector<URLNode *> children;

  virtual ~URLNode() = default;

  virtual void handle(String &out) {}

  URLNode *add(URLNode *child) {
    child->parent = this;
    children.push_back(child);
    return child;
  }

  URLNode *find(StringView name) {
    for (auto *child : children) {
      if (child->name == name)
        return child;
    }

    return nullptr;
  }

  URLNode *resolve(StringView path) {
    URLNode *current = this;

    for (auto &&part : path | std::views::split('/')) {
      if (part.begin() == part.end())
        continue;

      String component(part.begin(), part.end());

      if (component == ".")
        continue;

      if (component == "..") {
        if (current->parent)
          current = current->parent;
        continue;
      }

      current = current->find(component);

      if (!current)
        return nullptr;
    }

    return current;
  }
};

#define RAIREN_HANDLE void handle(String &out) override

}

#endif

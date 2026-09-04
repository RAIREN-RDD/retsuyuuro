#ifndef RETUYUURO_LOCALIZED_TEXT_HPP
#define RETUYUURO_LOCALIZED_TEXT_HPP

#include <stdexcept>
#include <unordered_set>
#include <utility>

#include "retuyuuro/HtmlElement.hpp"

namespace retuyuuro {

struct LocalizedText : HtmlElement {
  using Dataset = UnorderedMap<String, UnorderedMap<String, String>>;

  String key;
  String current_locale = "en";

  const Dataset *dataset = nullptr;

  LocalizedText(String key, const Dataset *dataset)
      : HtmlElement("", false), key(std::move(key)), dataset(dataset) {}

private:
  String resolve(const String &key,
                 std::unordered_set<String> &resolving) const {
    if (dataset == nullptr)
      throw std::runtime_error("LocalizedText: dataset is null");

    if (!resolving.insert(key).second) {
      throw std::runtime_error(
          "LocalizedText: recursive localization reference involving '" + key +
          "'");
    }

    const auto key_it = dataset->find(key);
    if (key_it == dataset->end()) {
      throw std::runtime_error("LocalizedText: unknown localization key '" +
                               key + "'");
    }

    const auto &locales = key_it->second;
    const auto locale_it = locales.find(current_locale);

    if (locale_it == locales.end()) {
      throw std::runtime_error("LocalizedText: localization key '" + key +
                               "' has no locale '" + current_locale + "'");
    }

    const String &text = locale_it->second;
    String result;
    result.reserve(text.size());

    std::size_t pos = 0;

    while (pos < text.size()) {
      const std::size_t open = text.find('{', pos);

      if (open == String::npos) {
        result.append(text, pos, String::npos);
        break;
      }

      result.append(text, pos, open - pos);

      const std::size_t close = text.find('}', open + 1);

      if (close == String::npos) {
        throw std::runtime_error(
            "LocalizedText: unmatched '{' in localization key '" + key + "'");
      }

      const String parameter = text.substr(open + 1, close - open - 1);

      if (parameter.empty()) {
        throw std::runtime_error(
            "LocalizedText: empty localization parameter in key '" + key + "'");
      }

      result += resolve(parameter, resolving);

      pos = close + 1;
    }

    resolving.erase(key);
    return result;
  }

public:
  void render(String &out) override {
    std::unordered_set<String> resolving;
    out += resolve(key, resolving);
  }
};

} // namespace retuyuuro

#endif

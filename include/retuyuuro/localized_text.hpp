#ifndef RETUYUURO_LOCALIZED_TEXT_HPP
#define RETUYUURO_LOCALIZED_TEXT_HPP

#include <stdexcept>
#include <unordered_set>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "retuyuuro/HtmlElement.hpp"

namespace retuyuuro {

using Dataset = UnorderedMap<String, UnorderedMap<String, String>>;

inline Dataset dataset_load_yml(String path) {
  YAML::Node yaml_dataset = YAML::LoadFile(path);
  Dataset dataset;

  for (const auto &key : yaml_dataset) {
    for (const auto &lang : key.second) {
      dataset[key.first.as<String>()][lang.first.as<String>()] =
          lang.second.as<String>();
    }
  }

  return dataset;
}


struct LocalizedText : HtmlElement {

  static String resolve(
      const Dataset &dataset,
      const String &key,
      std::unordered_set<String> &resolving,
      const String &current_locale) {

    if (!resolving.insert(key).second) {
      throw std::runtime_error(
          "LocalizedText: recursive localization reference involving '" +
          key + "'");
    }

    const auto key_it = dataset.find(key);

    if (key_it == dataset.end()) {
      resolving.erase(key);
      return key + " NOT FOUND";
    }

    const auto &locales = key_it->second;

    auto locale_it = locales.find(current_locale);

    if (locale_it == locales.end()) {
      locale_it = locales.find("en");

      if (locale_it == locales.end()) {
        resolving.erase(key);

        throw std::runtime_error(
            "LocalizedText: localization key '" + key +
            "' has neither locale '" + current_locale +
            "' nor fallback locale 'en'");
      }
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
        resolving.erase(key);

        throw std::runtime_error(
            "LocalizedText: unmatched '{' in localization key '" +
            key + "'");
      }

      const String parameter =
          text.substr(open + 1, close - open - 1);

      if (parameter.empty()) {
        resolving.erase(key);

        throw std::runtime_error(
            "LocalizedText: empty localization parameter in key '" +
            key + "'");
      }

      result += resolve(
          dataset,
          parameter,
          resolving,
          current_locale);

      pos = close + 1;
    }

    resolving.erase(key);
    return result;
  }

public:

  static String resolve(
      const Dataset &dataset,
      const String &key,
      const String &lang) {

    std::unordered_set<String> resolving;

    return resolve(dataset, key, resolving, lang);
  }


  LocalizedText(String key, const Dataset *dataset)
      : HtmlElement("", false), key(std::move(key)), dataset(dataset) {}


  String key;
  const Dataset *dataset = nullptr;


  void render(String &out, String lang) override {
    if (dataset == nullptr)
      throw std::runtime_error("LocalizedText: dataset is null");

    out += resolve(*dataset, key, lang);
  }
};

} // namespace retuyuuro

#endif


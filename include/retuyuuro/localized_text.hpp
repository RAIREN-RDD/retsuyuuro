#ifndef RETUYUURO_LOCALIZED_TEXT_HPP
#define RETUYUURO_LOCALIZED_TEXT_HPP

#include <algorithm>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <utility>

#include "retuyuuro/HtmlElement.hpp"

namespace retuyuuro {

struct LocalizedText : HtmlElement {
  using LocaleTable = UnorderedMap<String, String>;

  using TranslationTable = UnorderedMap<String, LocaleTable>;

  static constexpr StringView fallback_locale = "en";

  TranslationTable translations;

  String key;
  String selected_locale;

  LocalizedText(std::ifstream &csv, String key, String selected_locale = "en")
      : HtmlElement("", false), key(std::move(key)),
        selected_locale(std::move(selected_locale)) {
    load(csv);
  }

  void render(String &out) override {
    out += resolve(selected_locale, key, {});
  }

  
  String get(StringView locale, StringView localization_key) const {
    return resolve(locale, localization_key, {});
  }

private:
  
  static Vector<String> parse_csv_line(StringView line) {
    Vector<String> fields;
    String field;

    bool quoted = false;

    for (std::size_t i = 0; i < line.size(); ++i) {

      const char c = line[i];

      if (c == '"') {
        if (quoted && i + 1 < line.size() && line[i + 1] == '"') {

          field += '"';
          ++i;
        } else {
          quoted = !quoted;
        }
      }

      else if (c == ',' && !quoted) {
        fields.push_back(std::move(field));
        field.clear();
      }

      else {
        field += c;
      }
    }

    if (quoted) {
      throw std::runtime_error("Unterminated quoted CSV field");
    }

    fields.push_back(std::move(field));

    return fields;
  }

  static void remove_utf8_bom(String &value) {
    constexpr StringView bom = "\xEF\xBB\xBF";

    if (value.size() >= bom.size() &&
        StringView(value).substr(0, bom.size()) == bom) {

      value.erase(0, bom.size());
    }
  }

  void load(std::ifstream &file) {
    if (!file) {
      throw std::runtime_error("Invalid localization CSV stream");
    }

    String line;

    if (!std::getline(file, line)) {
      throw std::runtime_error("Localization CSV is empty");
    }

    remove_utf8_bom(line);

    const auto header = parse_csv_line(line);

    if (header.empty() || header.front() != "key") {

      throw std::runtime_error("First CSV column must be 'key'");
    }

    if (header.size() < 2) {
      throw std::runtime_error("CSV must contain at least one locale");
    }

    for (std::size_t i = 1; i < header.size(); ++i) {

      const String &locale = header[i];

      if (locale.empty()) {
        throw std::runtime_error("Locale column cannot be empty");
      }

      if (translations.contains(locale)) {
        throw std::runtime_error("Duplicate locale column: " + locale);
      }

      translations.emplace(locale, LocaleTable{});
    }

    std::size_t line_number = 1;

    while (std::getline(file, line)) {
      ++line_number;

      if (line.empty())
        continue;

      const auto fields = parse_csv_line(line);

      if (fields.size() != header.size()) {
        throw std::runtime_error("Invalid number of columns on CSV line " +
                                 std::to_string(line_number) + ": expected " +
                                 std::to_string(header.size()) + ", got " +
                                 std::to_string(fields.size()));
      }

      const String &localization_key = fields[0];

      if (localization_key.empty()) {
        throw std::runtime_error("Empty key on CSV line " +
                                 std::to_string(line_number));
      }

      for (std::size_t i = 1; i < header.size(); ++i) {

        auto &table = translations.at(header[i]);

        if (table.contains(localization_key)) {
          throw std::runtime_error(
              "Duplicate key '" + localization_key + "' for locale '" +
              header[i] + "' on CSV line " + std::to_string(line_number));
        }

        table.emplace(localization_key, fields[i]);
      }
    }

    if (!translations.contains(String(fallback_locale))) {

      throw std::runtime_error("Fallback locale '" + String(fallback_locale) +
                               "' is not present in CSV");
    }
  }

  
  StringView lookup(StringView locale, StringView localization_key) const {

    if (const auto locale_it = translations.find(String(locale));
        locale_it != translations.end()) {

      const auto &table = locale_it->second;

      if (const auto key_it = table.find(String(localization_key));
          key_it != table.end() && !key_it->second.empty()) {

        return key_it->second;
      }
    }

    const auto fallback_it = translations.find(String(fallback_locale));

    if (fallback_it != translations.end()) {
      const auto &table = fallback_it->second;

      if (const auto key_it = table.find(String(localization_key));
          key_it != table.end() && !key_it->second.empty()) {

        return key_it->second;
      }
    }

    throw std::runtime_error("Unknown localization key '" +
                             String(localization_key) + "' for locale '" +
                             String(locale) + "'");
  }

  
  String resolve(StringView locale, StringView localization_key,
                 Vector<String> stack) const {
    const StringView value = lookup(locale, localization_key);

    if (std::find(stack.begin(), stack.end(), localization_key) !=
        stack.end()) {

      String message = "Circular localization reference: ";

      for (const auto &item : stack) {
        message += item;
        message += " -> ";
      }

      message += localization_key;

      throw std::runtime_error(message);
    }

    stack.push_back(String(localization_key));

    static const std::regex parameter_regex(R"(\{([^{}]+)\})");

    const String value_string(value);

    String output;

    output.reserve(value_string.size());

    std::size_t last = 0;

    for (std::sregex_iterator it(value_string.begin(), value_string.end(),
                                 parameter_regex);
         it != std::sregex_iterator(); ++it) {

      const std::smatch &match = *it;
      const std::size_t position = static_cast<std::size_t>(match.position());
      const std::size_t length = static_cast<std::size_t>(match.length());

      output.append(value_string, last, position - last);

      const String parameter_key = match[1].str();

      output += resolve(locale, parameter_key, stack);

      last = position + length;
    }

    output.append(value_string, last, std::string::npos);

    return output;
  }
};
}

#endif


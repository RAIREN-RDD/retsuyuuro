#include <fstream>

#include <retuyuuro/html.hpp>
#include <retuyuuro/localized_text.hpp>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <print>

using namespace retuyuuro;

int main() {
  using Dataset = UnorderedMap<String, UnorderedMap<String, String>>;

  YAML::Node yaml_dataset = YAML::LoadFile("../dataset.yml");
  Dataset dataset;

  for (const auto &key : yaml_dataset) {
    for (const auto &lang : key.second) {
      dataset[key.first.as<String>()][lang.first.as<String>()] =
          lang.second.as<String>();
    }
  }

  std::println("{}", dataset);

  Vector<String> languages = {"en", "ja"};

  HtmlPage page;

  auto *head = page.add<HtmlHead>();
  auto *body = page.add<HtmlBody>();
  auto *text = body->add<HtmlParagraph>()->add<LocalizedText>(
      "cli_tool_dev_pricing", &dataset);

  head->add<HtmlMeta>()->set_attribute("charset", "utf-8");

  for (auto &lang : languages) {
    if (!fs::exists(lang))
      fs::create_directories(lang);

    text->current_locale = lang;
    String out;

    page.render(out);
    std::ofstream(lang + "/" + "index.html") << out;
  }

  return 0;
}

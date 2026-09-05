#include <fstream>

#include <retuyuuro/html.hpp>
#include <retuyuuro/localized_text.hpp>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <print>

using namespace retuyuuro;

int main() {
  Dataset dataset = retuyuuro::dataset_load_yml("dataset.yml");

  Vector<String> languages = {"en", "ja"};

  HtmlPage page;

  auto *head = page.head;
  auto *body = page.body;

  body->add<HtmlParagraph>()->add<LocalizedText>("msg", &dataset);

  head->add<HtmlMeta>()->set_attribute("charset", "utf-8");

  String out = "";

  page.render(out, "en");
  std::println("EN: {}", out);

  out = "";
  page.render(out, "ja");
  std::println("JA: {}", out);

  return 0;
}

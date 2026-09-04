#include <fstream>

#include <retuyuuro/html.hpp>
#include <retuyuuro/localized_text.hpp>

using namespace retuyuuro;

int main() {
  std::ifstream csv("../translation.csv");

  Vector<String> languages = {"en", "ja"};

  HtmlPage page;

  auto *head = page.add<HtmlHead>();
  auto *body = page.add<HtmlBody>();
  auto *text = body->add<HtmlParagraph>()->add<LocalizedText>(
      csv, "cli_tool_dev_pricing");

  head->add<HtmlMeta>()->set_attribute("charset", "utf-8");

  for (auto &lang : languages) {
    if (!fs::exists(lang))
      fs::create_directories(lang);

    text->selected_locale = lang;
    String out;

    page.render(out);
    std::ofstream(lang + "/" + "index.html") << out;
  }

  return 0;
}

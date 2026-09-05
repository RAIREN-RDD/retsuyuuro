# Retuyuuro/裂融炉 - Structured synthesis.
A C++26 framework for webserver creation and serving.

## ❖ Features
- Webpage generation
- String templates from YAML

### ❖ Coming soon
- String pointers (propagate changes)
- Arbitrary component ownership (currently callers own them)
- Fine-grained HTTP support through uWebsockets C++
- Database support through libpqxx
- Caching support through Redis

## ❖ Example

**main.cpp**:  
```cpp
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
```

**dataset.yml**:  
```yml
msg:
  en: "This is an English string. And this got injected: {currency}"
  ja: "此れは日本語の句です。そして此れを入らせり：{currency}"

currency:
  en: "EUR"
  ja: "円"
```

```bash
$ ./scripts/build.sh 
ccache g++ -std=c++26 -Wall -Iinclude -isystem /home/kai/rairen/sdk/cpp23/include/nlohmann/v3.12.0-55f9368 -isystem /home/kai/rairen/sdk/cpp23/include/uWebSockets/v29.79.0-fe7c01a -isystem /home/kai/rairen/sdk/c11/include/uSockets/v0.8.8-17-g86097c4/uSockets -isystem /home/kai/rairen/sdk/cpp26/include/rairen/v1.0.0 -c src/main.cpp -o build/obj/main.o
ccache g++ -o build/entry build/obj/main.o -L/home/kai/rairen/sdk/c11/lib/uSockets/v0.8.8-17-g86097c4 -luSockets -lyaml-cpp -Wl,-rpath,/home/kai/rairen/sdk/c11/lib/uSockets/v0.8.8-17-g86097c4
$ ./build/entry 
EN: <!doctype html><html><head><meta charset="utf-8"></head><body><p>This is an English string. And this got injected: EUR</p></body></html>
JA: <!doctype html><html><head><meta charset="utf-8"></head><body><p>此れは日本語の句です。そして此れを入らせり：円</p></body></html>
```

## ❖ Contact
Website: https://rairen.net/contact


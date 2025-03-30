#include <iostream>
#include <sstream>
#include <processing.h>
#include  <algorithm>
/*
Программа считает частоту слов, встречающихся во всех файлах в директории 
и выводит итоговую статистику в консоль
*/

int main(int argc, char **argv) {
  if(argc != 2)
    return 1;
    // std::vector<std::stringstream> files(3);
    // files[0] << "1\n2\n3\n4\n5, AAA; BBB";
    // files[1] << "6\n7\n8\n9\n10";
    // auto result = AsDataFlow(files) | Split("\n ,;") | Out(std::cout);
  Dir(argv[1], true) 
    | Filter([](const std::filesystem::path& p){ return p.extension() == ".txt"; })
    | OpenFiles()
    | Split("\n ,.;")
    | Transform(
        [](std::string& token) { 
            std::transform(token.begin(), token.end(), token.begin(), [](char c){return std::tolower(c);});
            return token;
        })
    | AggregateByKey(
        0uz, 
        [](const std::string&, size_t& count) { ++count;},
        [](const std::string& token) { return token;}
      )
    | Transform([](const std::pair<std::string, size_t>& stat) { return std::format("{} - {}", stat.first, stat.second);})
    | Out(std::cout);

  return 0;
}

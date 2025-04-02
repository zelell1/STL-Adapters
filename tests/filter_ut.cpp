#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <numeric>

TEST(FilterTest, FilterEven) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(FilterTest, FilterUpperCase) {
    std::vector<std::string> input = {"hello", "world", "HELLO", "WORLD"};
    auto result =
        AsDataFlow(input)
            | Filter([](const std::string& x) { return std::all_of(x.begin(), x.end(), [](char c) { return std::isupper(c); }); })
            | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD"));
}

TEST(FilterTest, FilterEvenNumbersAndLongStrings) {
    std::vector<std::string> input = {"apple", "banana", "cherry", "kiwi", "2", "4", "6", "8", "12"};
    
    auto result = AsDataFlow(input)
        | Filter([](const std::string& x) { return (x.length() > 5 && std::all_of(x.begin(), x.end(), ::isalpha)); })
        | Filter([](const std::string& x) { return std::all_of(x.begin(), x.end(), ::isdigit); }) 
        | AsVector();
    
    ASSERT_THAT(result, testing::ElementsAre());
}

TEST(FilterTest, FilterAndTransformEvenNumbers) {
    std::vector<int> input = {1, 2, 3, 4, 5, 6};
    auto result = AsDataFlow(input)
        | Filter([](int x) { return x % 2 == 0; })
        | Transform([](int x) { return x * 2; })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(4, 8, 12));
}

TEST(FilterTest, FilterNonEmptyAndToUpper) {
    std::vector<std::string> input = {"hello", "", "world", "!"};
    auto result = AsDataFlow(input)
        | Filter([](const std::string& s) { return !s.empty(); })
        | Transform([](const std::string& s) {
             std::string upper = s;
             std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
             return upper;
          })
        | AsVector();

    ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD", "!"));
}

TEST(FilterTest, SplitFilterAndTransform) {
    std::vector<std::stringstream> input(1);
    input[0] << "123||456|789|";
    auto result = AsDataFlow(input)
        | Split("|")
        | Filter([](const std::string& s) { return !s.empty(); })
        | Transform([](const std::string& s) { return std::to_string(s.size()); })
        | AsVector();

    ASSERT_THAT(result, testing::ElementsAre("3", "3", "3"));
}

TEST(FilterTest, ComplexFilter) {
    std::vector<int> input;
    for (int i = 1; i <= 20; ++i) {
        input.push_back(i);
    }
    auto result = AsDataFlow(input)
        | Filter([](int x) { return x % 3 == 0; }) 
        | Filter([](int x) { return x % 2 == 0; }) 
        | Transform([](int x) { return x + 1; })
        | AsVector();

    ASSERT_THAT(result, testing::ElementsAre(7, 13, 19));
}


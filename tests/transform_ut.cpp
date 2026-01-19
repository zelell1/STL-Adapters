#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(TransformTest, PowerOfTwo) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 4, 9, 16, 25));
}

TEST(TransformTest, FromStringToInt) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ") | Transform([](const std::string& str) { return std::stoi(str); }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
}

TEST(TransformTest, EmptyInput) {
    std::vector<int> input = {};

    auto result = AsDataFlow(input) | Transform([](int x) { return x * 2; }) | AsVector();
    
    ASSERT_TRUE(result.empty());
}

TEST(TransformTest, SingleElementInput) {
    std::vector<int> input = {5};
    std::vector<int> output;
    
    auto result = AsDataFlow(input) | Transform([](int x) { return x * 2; }) | AsVector();
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], 10); 
}

TEST(TransformTest, CastType) {
    std::vector<int> input = {1, 2, 3};
    std::vector<std::string> output;
    
    auto result = AsDataFlow(input) | Transform([](int x) { return std::to_string(x); }) | AsVector();
        
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], "1");
    ASSERT_EQ(result[1], "2");
    ASSERT_EQ(result[2], "3");
}

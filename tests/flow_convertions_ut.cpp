#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <numeric>

TEST(FlowConvertionsTest, AsDataFlow) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto flow = AsDataFlow(input);
    ASSERT_THAT(flow, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(FlowConvertionsTest, AsVector) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | AsVector();
    ASSERT_THAT(result, testing::ElementsAreArray(std::vector<int>{1, 2, 3, 4, 5}));
}

TEST(FlowConversionsTest, EmptyInput) {
    std::vector<int> input; 
    auto flow = AsDataFlow(input);
    auto result = flow | AsVector();
    ASSERT_THAT(result, testing::IsEmpty()); 
}

TEST(FlowConversionsTest, StreamInput) {
    std::vector<std::stringstream> input(2);
    input[0] << "hello world";
    input[1] << "test flow";
    auto result = AsDataFlow(input)
        | Split(" ")
        | AsVector();
    ASSERT_THAT(
        result,
        testing::ElementsAre("hello", "world", "test", "flow")
    );
}

TEST(FlowConversionsTest, AggregateWords) {
    std::vector<std::string> input = {"cat", "dog", "cat", "bird", "dog", "cat"};
    auto result = AsDataFlow(input)
        | AggregateByKey(
            0uz,
            [](const std::string&, size_t& count) { ++count; },
            [](const std::string& token) { return token; }
        )
        | Transform([](const std::pair<std::string, size_t>& stat) { 
            return std::format("{}:{}", stat.first, stat.second); 
        })
        | AsVector();
    std::vector<std::string> expected = {"bird:1", "cat:3", "dog:2"};
    std::sort(result.begin(), result.end()); 
    ASSERT_THAT(result, testing::ElementsAreArray(expected));
}

TEST(FlowConversionsTest, NestedVector) {
    std::vector<std::vector<int>> input = {{1, 2}, {3, 4, 5}, {6}};
    auto result = AsDataFlow(input)
        | Transform([](const std::vector<int>& vec) { 
            return std::accumulate(vec.begin(), vec.end(), 0);
        })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(3, 12, 6)); 
}


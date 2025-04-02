#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <expected>

struct Department {
    std::string name;
    bool operator==(const Department& other) const = default;        
};

std::expected<Department, std::string> ParseDepartment(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Department name is empty");
    }
    if (str.contains(' ')) {
        return std::unexpected("Department name contains space");
    }
    return Department{str};
}

TEST(SplitExpectedTest, SplitExpected) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|bad department||another-good-department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(ParseDepartment) | SplitExpected();

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name is empty.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"good-department"}, Department{"another-good-department"}));
}

TEST(SplitExpectedTest, AllValidDepartments) {
    std::vector<std::stringstream> files(1);
    files[0] << "sales|marketing|engineering";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(ParseDepartment) | SplitExpected();

    auto expected_result = good_flow | AsVector();
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"sales"}, Department{"marketing"}, Department{"engineering"}));
}

TEST(SplitExpectedTest, VariousErrors) {
    std::vector<std::stringstream> files(1);
    files[0] << "valid|invalid department|another_valid| ";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(ParseDepartment) | SplitExpected();

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name contains space.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"valid"}, Department{"another_valid"}));
}

TEST(SplitExpectedTest, TwoFilesAndMultiSplit) {
    std::vector<std::stringstream> files(2);
    files[0] << "valid|invalid department|another_valid| ";
    files[1] << "invalid department:another_valid, ";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|:,") | Transform(ParseDepartment) | SplitExpected();

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), 
    "Department name contains space.Department name contains space.Department name contains space.Department name contains space.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"valid"}, Department{"another_valid"},  Department{"another_valid"}));
}

TEST(FlowConversionsTest, ExpectedValues) {
    std::vector<std::expected<int, std::string>> input = {1, std::unexpected("error1"), 3, 
    std::unexpected("error2"), 5};
    auto [errors, values] = AsDataFlow(input) | SplitExpected();
    auto error_result = errors | AsVector();
    auto value_result = values | AsVector();
    ASSERT_THAT(error_result, testing::ElementsAre("error1", "error2"));
    ASSERT_THAT(value_result, testing::ElementsAre(1, 3, 5));
}
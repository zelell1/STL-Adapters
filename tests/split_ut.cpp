#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(ReadTest, ByNewLine) {
    std::vector<std::stringstream> files(2);
    files[0] << "1\n2\n3\n4\n5";
    files[1] << "6\n7\n8\n9\n10";
    auto result = AsDataFlow(files) | Split("\n") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}

TEST(ReadTest, BySpace) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}

TEST(SplitTest, EmptyInput) {
    std::vector<std::stringstream> files(1); 
    auto result = AsDataFlow(files) | Split("\n") | AsVector();
    ASSERT_TRUE(result.empty());
}

TEST(SplitTest, ConsecutiveDelimiters) {
    std::vector<std::stringstream> files(1);
    files[0] << "A,,B;;C"; 
    auto result = AsDataFlow(files) | Split(",;") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("A", "", "B", "", "C"));
}

TEST(SplitTest, MixedWhitespaceDelimiters) {
    std::vector<std::stringstream> files(1);
    files[0] << "Hello\tWorld\nNew\r\nParagraph";
    auto result = AsDataFlow(files) | Split("\t\n\r") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(
        "Hello", "World", "New", "", "Paragraph"
    ));
}

TEST(SplitTest, EdgeDelimiters) {
    std::vector<std::stringstream> files(1);
    files[0] << ",Start,,Middle,End";
    auto result = AsDataFlow(files) | Split(",") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("", "Start", "", "Middle", "End"));
}

TEST(SplitTest, EmptyTokensBetweenFiles) {
    std::vector<std::stringstream> files(2);
    files[0] << "Start|";
    files[1] << "|End";
    auto result = AsDataFlow(files) | Split("|") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("Start", "", "End"));
}

TEST(SplitTest, OnlyDelimiters) {
    std::vector<std::stringstream> files(1);
    files[0] << ",,,,";
    auto result = AsDataFlow(files) | Split(",") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("", "", "", ""));
}

#include "minishell_test_base.h"
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>  // for getcwd
#include <limits.h>  // for PATH_MAX

// ========================================
// Echo 命令测试
// ========================================

// 任务1: 完成基础 echo 测试
TEST_F(MinishellTestBase, EchoSimpleString) {
    std::string output = executeCommand("echo Hello");
    EXPECT_TRUE(output.find("Hello") != std::string::npos);
}

TEST_F(MinishellTestBase, EchoEmptyString) {
    std::string output = executeCommand("echo");
    // TODO: 验证输出（包含换行符和minishell提示符）
    EXPECT_TRUE(output.find("\n") != std::string::npos);
}

// ========================================
// Echo 参数化测试
// ========================================

// 任务2: 完成参数化测试用例构建
struct EchoTestParam {
    std::string input;           // 输入命令（不包含 "echo" ）
    std::string expected_output; // 期望包含的输出
    std::string description;     // 测试描述
};

class EchoParameterizedTest : public MinishellTestBase,
                               public ::testing::WithParamInterface<EchoTestParam> {
};

TEST_P(EchoParameterizedTest, EchoVariousInputs) {
    EchoTestParam param = GetParam();
    // TODO: 验证 "echo " + param.input 的输出是否包含期望的字符串 param.expected_output
    std::string output = executeCommand("echo " + param.input);
    EXPECT_TRUE(output.find(param.expected_output) != std::string::npos);
}

std::string getHome() {
    const char* home = std::getenv("HOME");
    return home ? std::string(home) : "/tmp";
}

// 任务3: 完成参数实例化
INSTANTIATE_TEST_SUITE_P(
    EchoTests,
    EchoParameterizedTest,
    ::testing::Values(
        // TODO: 在此添加至少4个测试参数，包括双引号字符串，单引号字符串，环境变量，不带引号的多个单词
        EchoTestParam{"\"Hello World\"", "Hello World", "Double quoted string"},
        EchoTestParam{"'Single Quotes'", "Single Quotes", "Single quoted string"},
        // 继续添加更多测试用例...
        EchoTestParam{"$HOME", getHome(), "Environment variable"},
        EchoTestParam{"Hello World", "Hello World", "Unquoted multiple words"}
    )
);

// ========================================
// cd 命令测试（边界值分析）
// ========================================

class CdCommandTest : public MinishellTestBase,
                      public ::testing::WithParamInterface<std::string> {
};

// 任务4: 完成 cd 命令的边界值预期输出判断
TEST_P(CdCommandTest, ChangeDirectory) {
    std::string target_dir = GetParam();
    std::string now_dir = executeCommand("pwd");
    std::string output = executeCommand("cd " + target_dir + "; pwd");
    // 然后验证输出中包含目标目录(注意考虑特殊情况)
    if (target_dir == "/") {
        EXPECT_TRUE(output.find("/") != std::string::npos);
    } else if (target_dir == ".") {
        // TODO: 当前目录不变，可以 pwd 获取当前工作目录进行验证
        EXPECT_TRUE(output.find(now_dir) != std::string::npos);
    } else if (target_dir == "..") {
        // TODO: 父目录，假设测试在某个已知目录下运行
        std::string current = now_dir;
        std::string expected;
        if (current == "/") {
            expected = "/";
        } else {
            if (current.back() == '/') current.pop_back();
            size_t last_slash = current.find_last_of('/');
            if (last_slash == 0) {
                expected = "/";
            } else {
                expected = current.substr(0, last_slash);
            }
        }
        if (!output.empty() && output.back() == '\n') output.pop_back();
        EXPECT_TRUE(output.find(expected) != std::string::npos);
    } else {
        // TODO: 一般目录， pwd 验证是否成功切换
        EXPECT_TRUE(output.find(target_dir) != std::string::npos);
    }
    
}


INSTANTIATE_TEST_SUITE_P(
    CdBoundaryTests,
    CdCommandTest,
    ::testing::Values(
        "/", // root directory
        ".", // current directory
        "..", // parent directory
        "/tmp" // typical valid directory
    )
);

// ========================================
// 主函数
// ========================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

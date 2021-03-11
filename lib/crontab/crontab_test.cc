#include "include/crontab.h"

#include <iostream>
#include <vector>

#include "include/assert.h"
#include "include/log.h"
#include "thread/this_thread.h"
#include "gtest/gtest.h"

namespace cg {

class CronTabTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        cg::SetMinLogLevel(cg::LOG_LEVEL_ALL);
    }

    void SetUp() {
    }

    void TearDown() {
    }

private:
};

TEST_F(CronTabTest, Invalid) {
    // check invalid by regex
    EXPECT_EQ(false, CronTab::Invalid("* * * * *"));
    EXPECT_EQ(false, CronTab::Invalid("*/2 * * * *"));
    EXPECT_EQ(false, CronTab::Invalid("* */2 * * *"));
    EXPECT_EQ(false, CronTab::Invalid("* 1,2 * * *"));
    EXPECT_EQ(false, CronTab::Invalid("   * * * * *   "));
    EXPECT_EQ(false, CronTab::Invalid("* * * * 0-1"));
    EXPECT_EQ(false, CronTab::Invalid("* * * * 0-1,10-20"));
    EXPECT_EQ(false, CronTab::Invalid("* * * 0-8,10-19 0-1,10-20"));
    EXPECT_EQ(false, CronTab::Invalid("* * * 0-8,10-19, 0-1,10-20"));
    EXPECT_EQ(true, CronTab::Invalid("* * * 0--8,10-19, 0-1,10-20"));
    EXPECT_EQ(true, CronTab::Invalid("* * * 0-/8,10-19, 0-1,10-20"));
    EXPECT_EQ(true, CronTab::Invalid(""));
    EXPECT_EQ(true, CronTab::Invalid("$ * * * *"));
    EXPECT_EQ(true, CronTab::Invalid("- - - * 1,2"));
}

TEST_F(CronTabTest, ParsePolicy) {
    EXPECT_EQ(POLICY_DIVISION, ParsePolicy("*"));
    EXPECT_EQ(POLICY_DIVISION, ParsePolicy("*/2"));
    EXPECT_EQ(POLICY_SOMEONE, ParsePolicy("1,2"));
    EXPECT_EQ(POLICY_SOMEONE, ParsePolicy("0-1"));
    EXPECT_EQ(POLICY_SOMEONE, ParsePolicy("0-1,10-19"));
    EXPECT_EQ(POLICY_SOMEONE, ParsePolicy("0-1,10-19,"));
    EXPECT_EQ(POLICY_SOMEONE, ParsePolicy("0-1,10-19,,,,,,,"));
    EXPECT_EQ(POLICY_SOMEONE, ParsePolicy("0-1,,,,,,,,,10-19"));
    EXPECT_EQ(POLICY_NUM, ParsePolicy("0--1,10-19,"));
    EXPECT_EQ(POLICY_NUM, ParsePolicy("0-1,10-,19,"));
    EXPECT_EQ(POLICY_NUM, ParsePolicy(""));
    EXPECT_EQ(POLICY_NUM, ParsePolicy("-"));
}

TEST_F(CronTabTest, ParseData) {
    ParseData(POLICY_DIVISION, "*");
    ParseData(POLICY_DIVISION, "*/2");
    ParseData(POLICY_SOMEONE, "1,2");
    ParseData(POLICY_SOMEONE, "1-2");
    ParseData(POLICY_SOMEONE, "1-2,10-19");
    auto data = ParseData(POLICY_SOMEONE, "1-2,,,,,,10-19");
    for (const auto& it : data) {
        std::cout << "curr:" << static_cast<uint32_t>(it) <<std::endl;
    }
    data = ParseData(POLICY_SOMEONE, "1-2,10-19,,,,,");
    for (const auto& it : data) {
        std::cout << "curr:" << static_cast<uint32_t>(it) <<std::endl;
    }
}

TEST_F(CronTabTest, GenCronTab) {
    GenCronTab("* * * * *");
    GenCronTab("*/1 * * * *");
    GenCronTab("* */2 * * *");
    GenCronTab("*/1 */2 * * *");
    GenCronTab("* 1,2 * * *");
    GenCronTab("*/2 1,2 * * *");
}

TEST_F(CronTabTest, Basic) {
    {
        CronTab crontab;
        // rule: * * * * *
        // policy: DIVISION
        std::vector<Rule> rules(RULE_TYPE_NUM);
        for (uint8_t i = 0; i < RULE_TYPE_NUM; ++i) {
            std::vector<uint8_t> data;
            rules[i] = CronTab::GenRule(static_cast<RuleType>(i), POLICY_DIVISION, data);
        }
        crontab.Init(rules);
        for (int i = 0; i < 1000; ++i) {
            EXPECT_EQ(true, crontab.CanExecute());
        }
    }
    {
        CronTab crontab;
        // rule: 0-59 * * * *
        // policy: devsion + minutes someone
        std::vector<Rule> rules(RULE_TYPE_NUM);
        uint8_t i = 0;
        std::vector<uint8_t> data;
        for (int j = 0; j < 60; ++j) {
            data.emplace_back(j);
        }
        rules[i] = CronTab::GenRule(static_cast<RuleType>(i), POLICY_SOMEONE, data);
        for (i = 1; i < RULE_TYPE_NUM; ++i) {
            std::vector<uint8_t> data;
            rules[i] = CronTab::GenRule(static_cast<RuleType>(i), POLICY_DIVISION, data);
        }

        crontab.Init(rules);
        EXPECT_EQ(true, crontab.CanExecute());
    }
}

}  // end of namespace cg

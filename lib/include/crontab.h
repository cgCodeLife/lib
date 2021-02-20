#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#include "lib/include/log.h"
#include "lib/include/strings.h"
#include "re2/re2.h"

namespace cg {

static const char* k_pattern_division_everyone = "\\*";
static const char* k_pattern_division_every = "\\*/[0-9]+";
static const char* k_pattern_someone = "([0-9]+[,]*)+";
static const char* k_pattern_someone_range = "([0-9]+-[0-9]+[,]*)+";  // eg: 0-10,15-20

struct Date {
    uint64_t timestamp_;

    Date() : timestamp_(0) {}
};

enum RuleType {
    RULE_TYPE_MON = uint8_t(0),
    RULE_TYPE_WEEK,
    RULE_TYPE_DAY,
    RULE_TYPE_HOUR,
    RULE_TYPE_MINUTE,
    RULE_TYPE_NUM,
};

enum Policy {
    POLICY_DIVISION = uint8_t(0),
    POLICY_SOMEONE,
    POLICY_NUM,
};

struct Rule {
    RuleType type_;
    Policy policy_;
    std::vector<uint8_t> list_;

    Rule() {
        type_ = RULE_TYPE_NUM;
        policy_ = POLICY_NUM;
    }

    bool Empty() {
        return type_ == RULE_TYPE_NUM || policy_ == POLICY_NUM || list_.empty();
    }

    bool Find(int date) {
        for (auto& it : list_) {
            if (it == static_cast<uint8_t>(date)) {
                return true;
            }
        }
        return false;
    }

    int Distance(int date) {
        uint8_t min_gap = 255;
        uint8_t res = 0;
        for (auto& it : list_) {
            if (policy_ == POLICY_DIVISION && static_cast<uint32_t>(it) == 0) {
                LOG(INFO) << "ignore type_:" << type_;
                res = 0;
                break;
            }
            auto gap = std::min(static_cast<uint8_t>(std::abs(static_cast<uint8_t>(date) - it)),
                    min_gap);
            if (gap != min_gap) {
                res = it;
            }
        }
        return static_cast<int>(res);
    }
};

class CronTab {
public:
    // check crontab pattern
    static bool Invalid(const std::string& pattern) {
        std::vector<std::string> v;
        StringSplit(" ", pattern, &v);
        if (v.size() != 5) {
            fprintf(stderr, "invalid pattern for split. pattern:%s, size:%u\n",
                    pattern.c_str(), static_cast<uint32_t>(v.size()));
            return true;
        }
        for (const auto& it : v) {
            if (!RE2::FullMatch(it, k_pattern_division_every)
                    && !(RE2::FullMatch(it, k_pattern_division_everyone))
                    && !(RE2::FullMatch(it, k_pattern_someone))
                    && !(RE2::FullMatch(it, k_pattern_someone_range))) {
                fprintf(stderr, "invalid regex. pattern:%s\n", it.c_str());
                return true;
            }
        }
        return false;
    }

    static bool Invalid(RuleType type, Policy policy) {
        return (type < RULE_TYPE_MON || type >= RULE_TYPE_NUM)
            || (policy < POLICY_DIVISION || policy >= POLICY_NUM);
    }

    static Rule GenRule(RuleType type, Policy policy, const std::vector<uint8_t> data) {
        if (Invalid(type, policy)) {
            return Rule{};
        }
        Rule rule;
        rule.type_ = type;
        rule.policy_ = policy;
        rule.list_ = data;  // just copy
        return rule;
    }

public:
    CronTab() {}

    bool Init(const std::vector<Rule>& rules) {
        for (auto& rule : rules) {
            if (Invalid(rule.type_, rule.policy_)) {
                return false;
            }
            rules_[rule.type_] = rule;
        }
        setLastDivision();
        return true;
    }

    bool CanExecute();

private:
    bool month();

    bool week();

    bool day();

    bool hour();

    bool minute();

    bool compare(RuleType type);

    int getCurrentDate(RuleType);

    bool enoughDistance();

    void setLastDivision();

private:
    // Consistent with the crontab
    Rule rules_[RULE_TYPE_NUM];
    Date last_exe_;
    RuleType last_division_;
};

// NOTE(caoge): parse crontab config by regex
CronTab* GenCronTab(const std::string& pattern);

Policy ParsePolicy(const std::string& pattern);

std::vector<uint8_t> ParseData(const Policy& policy, const std::string& pattern);

void parseRange(const std::string& pattern, uint8_t* begin, uint8_t* end);

}  // end of namespace cg
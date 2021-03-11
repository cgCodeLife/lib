#include "include/crontab.h"

#include <time.h>

#include <string>
#include <unordered_map>

#include "include/assert.h"
#include "include/log.h"
#include "system/timestamp.h"

namespace cg {

bool CronTab::CanExecute() {
    return month() && week() && day() && hour() && minute();
}

bool CronTab::month() {
    return compare(RULE_TYPE_MON);
}

bool CronTab::week() {
    return compare(RULE_TYPE_WEEK);
}

bool CronTab::day() {
    return compare(RULE_TYPE_DAY);
}

bool CronTab::hour() {
    return compare(RULE_TYPE_HOUR);
}

bool CronTab::minute() {
    return compare(RULE_TYPE_MINUTE);
}

bool CronTab::compare(RuleType type) {
    // no set this type, default no limit
    if (rules_[type].Empty()) {
        return true;
    }
    bool ans = false;
    auto curr = getCurrentDate(type);
    switch (rules_[type].policy_) {
        case POLICY_DIVISION: {
            ans = (curr % rules_[type].list_[0] == 0);
        } break;
        case POLICY_SOMEONE: {
            auto rule = rules_[type];
            ans = rule.Find(curr);
        } break;
        default:
            break;
    }
    if (!ans) {
        LOG(DEBUG) << "Someone plice failed. type=" << static_cast<uint32_t>(type)
                  << ", curr=" << curr << ", val=" << static_cast<uint32_t>(rules_[type].list_[0])
                  << ", policy=" << static_cast<uint32_t>(rules_[type].policy_);
    }
    return ans;
}

int CronTab::getCurrentDate(RuleType type) {
    time_t tt = time(nullptr);
    struct tm stm;
    localtime_r(&tt, &stm);
    int ans = 0;
    switch (type) {
        case RULE_TYPE_MON:
            ans = stm.tm_mon;
            break;
        case RULE_TYPE_WEEK:
            ans = stm.tm_wday;
            break;
        case RULE_TYPE_DAY:
            ans = stm.tm_mday;
            break;
        case RULE_TYPE_HOUR:
            ans = stm.tm_hour;
            break;
        case RULE_TYPE_MINUTE:
            ans = stm.tm_min;
            break;
        default:
            ans = -1;
            break;
    }
    return ans;
}

Policy ParsePolicy(const std::string& pattern) {
    if (RE2::FullMatch(pattern, k_pattern_division_everyone) ||
        RE2::FullMatch(pattern, k_pattern_division_every)) {
        return POLICY_DIVISION;
    } else if (RE2::FullMatch(pattern, k_pattern_someone) ||
               RE2::FullMatch(pattern, k_pattern_someone_range)) {
        return POLICY_SOMEONE;
    }
    return POLICY_NUM;
}

std::vector<uint8_t> ParseData(const Policy& policy, const std::string& pattern) {
    std::vector<uint8_t> data;
    switch (policy) {
        case POLICY_DIVISION: {
            auto str = pattern;
            if (RE2::FullMatch(str, k_pattern_division_everyone)) {
                data.push_back(static_cast<uint8_t>(1));
            } else {
                auto tmp = str.substr(str.find_first_of("/") + 1);
                CG_ASSERT(RE2::FullMatch(tmp, "[0-9]+"));
                data.push_back(static_cast<uint8_t>(std::stoi(tmp)));
            }
        } break;
        case POLICY_SOMEONE: {
            auto str = pattern;
            if (RE2::FullMatch(str, k_pattern_someone)) {
                auto pos = str.find_first_of(",");
                while (pos != std::string::npos) {
                    auto tmp = str.substr(0, pos);
                    if (tmp != "") {
                        CG_ASSERT(RE2::FullMatch(tmp, "[0-9]+"));
                        data.push_back(static_cast<uint8_t>(std::stoi(tmp)));
                    }
                    str = str.substr(pos + 1);
                    pos = str.find_first_of(",");
                }
                if (str != "" && str != ",") {
                    CG_ASSERT(RE2::FullMatch(str, "[0-9]+"));
                    data.push_back(static_cast<uint8_t>(std::stoi(str)));
                }
            } else if (RE2::FullMatch(str, k_pattern_someone_range)) {
                // eg: 0-10,15-20
                // eg: ["0-10", "15-20"]
                std::vector<std::string> range_list;
                StringSplit(",", str, &range_list);
                for (const auto& it : range_list) {
                    uint8_t begin = 0;
                    uint8_t end = 0;
                    parseRange(it, &begin, &end);
                    for (uint8_t i = begin; i <= end; ++i) {
                        data.push_back(i);
                    }
                }
            }
        } break;
        default:
            CG_ASSERT(false);
            break;
    }
    return data;
}

CronTab* GenCronTab(const std::string& pattern) {
    std::vector<std::string> v;
    StringSplit(" ", pattern, &v);
    CG_ASSERT_EQ(5U, v.size());
    std::vector<Rule> rules(RULE_TYPE_NUM);
    for (int i = RULE_TYPE_NUM - 1; i >= 0; --i) {
        auto policy = ParsePolicy(v[i]);
        auto data = ParseData(policy, v[i]);
        rules[i] = CronTab::GenRule(static_cast<RuleType>(RULE_TYPE_NUM - i - 1), policy, data);
    }
    CronTab* crontab = new CronTab();
    crontab->Init(rules);
    return crontab;
}

void parseRange(const std::string& pattern, uint8_t* begin, uint8_t* end) {
    auto pos = pattern.find_first_of("-");
    auto begin_str = pattern.substr(0, pos);
    auto end_str = pattern.substr(pos + 1);
    *begin = static_cast<uint8_t>(std::stoi(begin_str));
    *end = static_cast<uint8_t>(std::stoi(end_str));
}

}  // end of namespace cg

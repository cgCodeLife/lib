#include "lib/include/crontab.h"

#include <time.h>

#include <string>
#include <unordered_map>

#include "lib/include/assert.h"
#include "lib/include/log.h"
#include "lib/system/timestamp.h"

namespace cg {

static bool isLeapYear() {
    time_t tt = time(nullptr);
    struct tm stm;
    localtime_r(&tt, &stm);
    auto this_year = 1900 + stm.tm_year;
    if (this_year % 4 == 0) {
        if (this_year % 100 == 0) {
            if (this_year % 400 == 0) {
                return true;
            }
        }
    }
    return false;
}

static int getDaysByMonth(int m) {
    static std::unordered_map<int, int> month_map = {{0, 31}, {1, 29},  {2, isLeapYear() ? 29 : 28},
                                                     {3, 30}, {4, 31},  {5, 30},
                                                     {6, 31}, {7, 30},  {8, 30},
                                                     {9, 31}, {10, 30}, {11, 31}};
    if (month_map.find(m) == month_map.end()) {
        return 30;  // default
    }
    return month_map[m];
}

bool CronTab::CanExecute() {
    if (last_exe_.timestamp_ == 0UL) {
        last_exe_.timestamp_ = GetCurrentTime();
        return true;  // first access
    }
    if (enoughDistance() && month() && week() && day() && hour() && minute()) {
        last_exe_.timestamp_ = GetCurrentTime();
        return true;
    }
    return false;
}

void CronTab::setLastDivision() {
    last_division_ = RULE_TYPE_MINUTE;
    if (!rules_[RULE_TYPE_MINUTE].Empty()
            && rules_[RULE_TYPE_MINUTE].policy_ == POLICY_DIVISION) {
        last_division_ = RULE_TYPE_MINUTE;
        return;
    }

    if (!rules_[RULE_TYPE_HOUR].Empty()
            && rules_[RULE_TYPE_HOUR].policy_ == POLICY_DIVISION) {
        last_division_ = RULE_TYPE_HOUR;
        return;
    }

    if (!rules_[RULE_TYPE_DAY].Empty()
            && rules_[RULE_TYPE_DAY].policy_ == POLICY_DIVISION) {
        last_division_ = RULE_TYPE_DAY;
        return;
    }

    if (!rules_[RULE_TYPE_WEEK].Empty()
            && rules_[RULE_TYPE_WEEK].policy_ == POLICY_DIVISION) {
        last_division_ = RULE_TYPE_WEEK;
        return;
    }

    if (!rules_[RULE_TYPE_MON].Empty()
            && rules_[RULE_TYPE_MON].policy_ == POLICY_DIVISION) {
        last_division_ = RULE_TYPE_MON;
        return;
    }
}

// NOTE(caoge): In Division mode, use a smaller time range, such as the minute
// level will override the hour level,
// so don't configure something like every XX minutes, every XX hours, the next hours are useless
bool CronTab::enoughDistance() {
    auto curr = GetCurrentTime();
    auto diff = static_cast<uint64_t>(curr - last_exe_.timestamp_);
    // mon
    if (!rules_[RULE_TYPE_MON].Empty()) {
        auto min_gap = static_cast<uint64_t>(86400 * getDaysByMonth(getCurrentDate(RULE_TYPE_MON)));
        auto diff_month = diff / min_gap;
        min_gap *= (rules_[RULE_TYPE_MON].policy_ == POLICY_SOMEONE)
                           ? 1
                           : rules_[RULE_TYPE_MON].Distance(diff_month);
        if (diff >= min_gap) {
            LOG(INFO) << "Mon distance passed.";
            return true;
        }
        if (last_division_ == RULE_TYPE_MON || rules_[RULE_TYPE_MON].policy_ == POLICY_SOMEONE) {
            LOG(DEBUG) << "Mon not enough. diff:" << diff << ", min_gap:" << min_gap;
            return false;
        }
    }
    // week
    if (!rules_[RULE_TYPE_WEEK].Empty()) {
        auto min_gap = static_cast<uint64_t>(86400 * 7);
        auto diff_week = diff / min_gap;
        min_gap *= (rules_[RULE_TYPE_WEEK].policy_ == POLICY_SOMEONE)
                           ? 1
                           : rules_[RULE_TYPE_WEEK].Distance(diff_week);
        if (diff >= min_gap) {
            LOG(INFO) << "Week distance passed.";
            return true;
        }
        if (last_division_ == RULE_TYPE_WEEK || rules_[RULE_TYPE_WEEK].policy_ == POLICY_SOMEONE) {
            LOG(DEBUG) << "Week not enough. diff:" << diff << ", min_gap:" << min_gap;
            return false;
        }
    }
    // day
    if (!rules_[RULE_TYPE_DAY].Empty()) {
        auto min_gap = static_cast<uint64_t>(86400);
        auto diff_day = diff / min_gap;
        min_gap *= (rules_[RULE_TYPE_DAY].policy_ == POLICY_SOMEONE)
                           ? 1
                           : rules_[RULE_TYPE_DAY].Distance(diff_day);
        if (diff >= min_gap) {
            LOG(INFO) << "Day distance passed.";
            return true;
        }
        if (last_division_ == RULE_TYPE_DAY || rules_[RULE_TYPE_DAY].policy_ == POLICY_SOMEONE) {
            LOG(DEBUG) << "Day not enough. diff:" << diff << ", min_gap:" << min_gap;
            return false;
        }
    }
    // hour
    if (!rules_[RULE_TYPE_HOUR].Empty()) {
        auto min_gap = static_cast<uint64_t>(3600);
        auto diff_hour = diff / min_gap;
        min_gap *= (rules_[RULE_TYPE_HOUR].policy_ == POLICY_SOMEONE)
                           ? 1
                           : rules_[RULE_TYPE_HOUR].Distance(diff_hour);
        if (diff >= min_gap) {
            LOG(INFO) << "Hour distance passed.";
            return true;
        }
        if (last_division_ == RULE_TYPE_HOUR || rules_[RULE_TYPE_HOUR].policy_ == POLICY_SOMEONE) {
            LOG(DEBUG) << "Hour not enough. diff:" << diff << ", min_gap:" << min_gap;
            return false;
        }
    }
    // minute
    if (!rules_[RULE_TYPE_MINUTE].Empty()) {
        auto min_gap = static_cast<uint64_t>(60);
        auto diff_minute = static_cast<uint64_t>(diff / min_gap);
        min_gap *= (rules_[RULE_TYPE_MINUTE].policy_ == POLICY_SOMEONE)
                           ? 1
                           : rules_[RULE_TYPE_MINUTE].Distance(diff_minute);
        if (diff >= min_gap) {
            LOG(INFO) << "Minute distance passed.";
            return true;
        }
        LOG(DEBUG) << "Minute not enough. diff:" << diff << ", min_gap:" << min_gap;
    }
    LOG(DEBUG) << "Not enough distance.";
    return false;
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
            ans = true;  // handle by enoughDistance
        } break;
        case POLICY_SOMEONE: {
            auto rule = rules_[type];
            ans = rule.Find(curr);
        } break;
        default:
            break;
    }
    if (!ans) {
        LOG(DEBUG) << "Someone plice failed. type:" << type;
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
    if (RE2::FullMatch(pattern, k_pattern_division_everyone)
            || RE2::FullMatch(pattern, k_pattern_division_every)) {
        return POLICY_DIVISION;
    } else if (RE2::FullMatch(pattern, k_pattern_someone)
            || RE2::FullMatch(pattern, k_pattern_someone_range)) {
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
                data.push_back(1);
            } else {
                auto tmp = str.substr(str.find_first_of("/") + 1);
                ASSERT(RE2::FullMatch(tmp, "[0-9]+"));
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
                        ASSERT(RE2::FullMatch(tmp, "[0-9]+"));
                        data.push_back(static_cast<uint8_t>(std::stoi(tmp)));
                    }
                    str = str.substr(pos + 1);
                    pos = str.find_first_of(",");
                }
                if (str != "" && str != ",") {
                    ASSERT(RE2::FullMatch(str, "[0-9]+"));
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
            ASSERT(false);
            break;
    }
    return data;
}

CronTab* GenCronTab(const std::string& pattern) {
    std::vector<std::string> v;
    StringSplit(" ", pattern, &v);
    ASSERT_EQ(5U, v.size());
    std::vector<Rule> rules(RULE_TYPE_NUM);
    for (uint8_t i = 0; i < RULE_TYPE_NUM; ++i) {
        auto policy = ParsePolicy(v[i]);
        auto data = ParseData(policy, v[i]);
        rules[i] = CronTab::GenRule(static_cast<RuleType>(i), policy, data);
    }
    CronTab* crontab = new CronTab();
    crontab->Init(rules);
    return crontab;
}

void parseRange(const std::string& pattern, uint8_t* begin, uint8_t* end) {
    auto pos = pattern.find_first_of("-");
    auto begin_str = pattern.substr(0, pos);
    auto end_str = pattern.substr(pos+1);
    *begin = static_cast<uint8_t>(std::stoi(begin_str));
    *end = static_cast<uint8_t>(std::stoi(end_str));
}

}  // end of namespace cg

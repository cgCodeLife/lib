#include "crontab.h"

#include <time.h>

// GetCurrentTime you shoulde define by you self

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
    return enoughDistance() && month() && week() && day() && hour() && minute();
}

bool CronTab::enoughDistance() {
    bool ans = true;
    auto curr = GetCurrentTime();
    auto diff = static_cast<uint64_t>(curr - last_exe_.timestamp_);
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
        LOG(WARNING) << "Minute not enough. diff:" << diff << ", min_gap:" << min_gap;
        ans = false;
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
        LOG(WARNING) << "Hour not enough. diff:" << diff << ", min_gap:" << min_gap;
        ans = false;
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
        LOG(WARNING) << "Day not enough. diff:" << diff << ", min_gap:" << min_gap;
        ans = false;
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
        LOG(WARNING) << "Week not enough. diff:" << diff << ", min_gap:" << min_gap;
        ans = false;
    }
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
        LOG(WARNING) << "Mon not enough. diff:" << diff << ", min_gap:" << min_gap;
        ans = false;
    }
    if (ans) {
        return true;
    }
    LOG(WARNING) << "Nout enough distance.";
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
    if (ans) {
        last_exe_.timestamp_ = GetCurrentTime();
    } else {
        LOG(WARNING) << "Someone plice failed. type:" << type;
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

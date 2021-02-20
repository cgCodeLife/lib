
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

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

    bool haveZero() {
        for (auto& it : list_) {
            if (it == 0 && policy_ != POLICY_SOMEONE) {
                return true;
            }
        }
        return false;
    }

    bool Empty() {
        return type_ == RULE_TYPE_NUM || policy_ == POLICY_NUM || list_.empty() || haveZero();
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

private:
    // Consistent with the crontab
    Rule rules_[RULE_TYPE_NUM];
    Date last_exe_;
};


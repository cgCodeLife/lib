/**
 * Author: caoge@strivemycodelife@163.com
 * Date:2021-03-12
 */
#pragma once

#include <stdlib.h>

#include "include/log.h"
#include "include/macros.h"

#define ASSERT(c) cg::Assertion(__FILE__, __LINE__).Is((c), #c)
#define ASSERT_TRUE(c) cg::Assertion(__FILE__, __LINE__).Is((c), #c)
#define ASSERT_FALSE(c) cg::Assertion(__FILE__, __LINE__).Is(!(c), #c)
#define CG_CHECK_NOTNULL(p) cg::Assertion(__FILE__, __LINE__).Is((p != nullptr) \
        "'" #p "' Must Not Null")

namespace cg {

class Assertion {
public:
    Assertion(const char* f, int l) : ok_(true), fname_(f), line_(l) {}

    ~Assertion() {
        if (LILKELY(ok_)) {
            return;
        }
        LOGF_FATAL("%s:%d:%s\n", fname_, line_, ss_.str().c_str());
        abort();
    }

    void inline SetStatus(const char* msg) {
        ss_ << " Assertion failure " << msg;
        ok_ = false;
    }

    Assertion& Is(bool b, const char* msg) {
        if (UNLIKELY(!b)) {
            SetStatus(msg);
        }
        return *this;
    }

private:
    bool ok_;
    const char* fname_;
    int line_;
    std::stringstream ss_;
};

}  // end of namespace cg

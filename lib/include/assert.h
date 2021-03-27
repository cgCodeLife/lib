/**
 * Author: caoge@strivemycodelife@163.com
 * Date:2021-03-12
 */
#pragma once

#include <stdlib.h>
#include <sstream>

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
        if (LIKELY(ok_)) {
            return;
        }
        LOG(FATAL) << "fname:" << fname_ << ", line:" << line_ << ", msg:" <<  ss_.str().c_str();
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

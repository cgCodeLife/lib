/**
 * Author: caoge@strivemycodelife@163.com
 * Date: 2021-03-11
 */

#include "include/slice.h"

namespace cg {

char toHex(unsigned char v) {
    if (v <= 9) {
        return '0' + v;
    }
    return 'A' + v - 10;
}

int fromHex(char c) {
    if (c >= 'a' && c <= 'f') {
        c -= ('a' - 'A');
    }
    if (c < '0' || (c > '9' && (c < 'A' || c > 'F'))) {
        return -1;
    }
    if (c <= '9') {
        return c - '0';
    }
    return c - '0' + 10;
}

std::string Slice::ToString(bool hex) const {
    std::string result;
    if (hex) {
        result.reserve(2 * size_);
        for (std::size_t i = 0; i < size_; ++i) {
            unsigned char c = data_[i];
            result.push_back(toHex(c >> 4));
            result.push_back(toHex(c & 0xf));
        }
    } else {
        result.assign(data_, size_);
    }
    return result;
}

bool Slice::DecodeHex(std::string* result) const {
    std::string::size_type len = size_;
    if (len % 2) {
        return false;
    }
    if (!result) {
        return false;
    }
    result.clear();
    result->reserve(len / 2);
    for (std::size_t i = 0; i < len;) {
        int h1 = fromHex(data_[i++]);
        if (h1 < 0) {
            return false;
        }
        int h2 = fromHex(data_[i++]);
        if (h2 < 0) {
            return false;
        }
        result->push_back(h1 << 4 | h2);
    }
    return true;
}

}  // end of namespace cg

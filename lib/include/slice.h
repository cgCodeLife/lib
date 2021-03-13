/**
 * Author: caoge
 * Date: 2021-03-11
 */
#pragma once
#include <string.h>
#include <cstddef>
#include <string>

#include "assert.h"

namespace cg {

// thread unsafe
class Slice {
public:
    Slice() : data_(""), size_(0) {}

    Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}

    Slice(const char* s) : data_(s) {
        size_ = (s == nullptr) ? 0 : strlen(s);
    }

    inline const char* Data() const {
        return data_;
    }

    inline std::size_t Size() const {
        return size_;
    }

    inline bool Empty() const {
        return size_ == 0;
    }

    inline char operator[](std::size_t n) const {
        ASSERT_TRUE(n < size_);
        return data_[n];
    }

    inline void Clear() {
        data_ = "";
        size_ = 0;
    }

    inline void RemovePrefix(std::size_t n) {
        data_ += n;
        size_ -= n;
    }

    inline void RemoveSuffix(std::size_t n) {
        ASSERT_TRUE(n <= size_);
        size_ -= n;
    }

    std::string ToString(bool hex = false) const;

    bool DecodeHex(std::string* result) const;

    inline int Compare(const Slice& b) const {
        ASSERT(data_ != nullptr && b.data_ != nullptr);
        const std::size_t min_len = (size_ < b.size_) ? size_ : b.size_;
        int r  = memcmp(data_, b.data_, min_len);
        if (r == 0) {
            if (size_ < b.size_) {
                r = -1;
            } else if (size_ > b.size_) {
                r = 1;
            }
        }
        return r;
    }

    bool StartsWith(const Slice& x) const {
        return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
    }

    bool EndsWith(const Slice& x) const {
        return ((size_ >= x.size_) && (memcmp(data_ + size_ - x.size_, x.data_, x.size_)));
    }

    inline std::size_t DifferenceOffset(const Slice& b) const {
        std::size_t off = 0;
        const std::size_t len = (size_ < b.size_) ? size_ : b.size_;
        for (; off < len; ++off) {
            if (data_[off] != b.data_[off]) {
                break;
            }
        }
        return off;
    }

private:
    const char* data_;
    std::size_t size_;
};

inline bool operator==(const Slice& a, const Slice& b) const {
    return ((a.Size() == b.Size()) && (memcmp(a.Data(), b.Data(), a.Size()) == 0));
}

inline bool operator!=(const Slice& a, const Slice& b) const {
    return !(a == b);
}

}  // end of namespace cg

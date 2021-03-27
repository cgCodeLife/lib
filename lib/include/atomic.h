/**
 * Author: caoge@strivemycodelife@163.com
 * Date: 2021-03-27
 */

#pragma once

namespace raft {

template<type T>
class Atomic {
public:
    T operator()() {
        return v_;
    }

    operator++() {
    }

    operator++(int) {
    }

private:
    T v_;
};

}  // end of namespace raft

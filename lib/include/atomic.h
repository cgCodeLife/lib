
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

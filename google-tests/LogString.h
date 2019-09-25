#ifndef LOGSTRING_H_
#define LOGSTRING_H_

#include <sstream>
#include <string>
#include <utility>

namespace hearing_aid::tests {
class LogString {
    std::stringstream s{};
public:
    void insert(std::string s_) {
        s << std::move(s_);
    }

    bool isEmpty() const {
        return s.str().empty();
    }

    operator std::string() const { return s.str(); }
};
}

#endif

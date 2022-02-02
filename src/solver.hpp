/*
 * Copyright (c) 2022 David Leeds <davidesleeds@gmail.com>
 *
 * Wordle Solver is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <cassert>
#include <cctype>
#include <string_view>
#include <array>
#include <bitset>
#include <vector>

#pragma once

namespace wordle {

using namespace std;

template <size_t WordSize>
class basic_solver {
public:
    static constexpr size_t word_size = WordSize;

    enum color {
        GREEN,
        YELLOW,
        BLACK
    };

    template <typename DictIterator>
    basic_solver(DictIterator begin, DictIterator end) :
        list_(begin, end)
    {
    }

    void add_hint(size_t pos, char letter, color color)
    {
        assert(pos < WordSize);
        assert(letter >= 'a' && letter <= 'z');

        auto ci = letter - 'a';
        auto &rule = rules_[ci];

        /* Update rules */
        switch (color) {
        case GREEN:
            rule.yes.set(pos);
            break;
        case YELLOW:
            rule.no.set(pos);
            break;
        case BLACK:
            rule.no.set();
            break;
        }
    }

    void apply_hints()
    {
        /* Filter list */
        for (auto it = list_.begin(); it != list_.end(); ) {
            if (filter(*it)) {
                ++it;
            } else {
                it = list_.erase(it);
            }
        }
    }

    const auto &list() const
    {
        return list_;
    }

private:

    bool filter(const string_view &word) const
    {
        array<bitset<WordSize>, 26> map{};

        /* Set a bitmask of positions for each character */
        for (size_t i = 0; i < word.size(); ++i) {
            auto ci = word[i] - 'a';

            map[ci].set(i);
        }

        /* Iterate rules and filter */
        for (size_t ci = 0; ci < rules_.size(); ++ci) {
            auto &rule = rules_[ci];
            auto &pos = map[ci];

            /* Test if character is not present at known position(s) */
            if ((~pos & rule.yes).any()) {
                return false;
            }

            /* Test if character is present at known bad positions */
            if ((pos & rule.no).any()) {
                return false;
            }

            /* Test if a character is not present, but is required at an unknown position */
            if (pos.none() && rule.no.any() && !rule.no.all()) {
                return false;
            }
        }

        return true;
    }

    struct rule {
        bitset<WordSize> yes;
        bitset<WordSize> no;
    };

    vector<string_view> list_;
    array<rule, 26> rules_{};
};

using solver = basic_solver<5>;

} /* namespace wordle */

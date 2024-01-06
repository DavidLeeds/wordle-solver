/*
 * Copyright (c) 2022 David Leeds <davidesleeds@gmail.com>
 *
 * Wordle Solver is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "solver.hpp"

using namespace std;
using namespace wordle;

static constexpr string_view PROMPT     { "> " };


static vector<string> load_dictionary(const filesystem::path &path)
{
    ifstream file(path);
    string line;
    vector<string> dict;

    if (!file.is_open()) {
        throw runtime_error("could not open dictionary file");
    }

    while (getline(file, line)) {
        /* Only load usable words into memory */
        if (line.size() != solver::word_size) {
            continue;
        }

        /* Convert dictionary to lower-case */
        transform(line.begin(), line.end(), line.begin(), [](char c) -> char {
            return tolower(c);
        });

        dict.push_back(move(line));
    }

    return dict;
}


int main(int argc, const char **argv)
{
    if (argc != 2) {
        throw invalid_argument("provide dictionary path as command line argument");
    }

    auto dict = load_dictionary(argv[1]);

    solver solver(dict.begin(), dict.end());

    cout << "Wordle Solver\n\n";
    cout << "Commands:\n";
    cout << "  hint <word> <colors> Add a hint. The first argument is the 5 letter word.\n";
    cout << "                       The second argument is the initials of the colors of each letter box.\n";
    cout << "                       For example, \"bbbgy\" is black, black, black, green, yellow.\n";
    cout << "  list                 Print a list of possible words\n";
    cout << "  info                 Print dictionary info\n";

    cout << "\nEnter a command:\n";

    string line, cmd;

    for (;;) {
        cout << PROMPT;
        cout.flush();

        if (getline(cin, line) && !line.empty()) {
            istringstream ss(line);

            ss >> cmd;

            if (cmd == "hint") {
                string word, colors;

                ss >> word >> colors;

                if (word.size() != solver::word_size ||
                        colors.size() != solver::word_size) {
                    cerr << "hint 'word' and 'colors' arguments must be 5 characters\n";
                    continue;
                }

                for (size_t i = 0; i < solver::word_size; ++i) {
                    char letter = tolower(word[i]);
                    solver::color color;

                    switch (toupper(colors[i])) {
                    case 'G':
                        color = solver::GREEN;
                        break;
                    case 'Y':
                        color = solver::YELLOW;
                        break;
                    case 'B':
                        color = solver::BLACK;
                        break;
                    default:
                        cerr << "'hint' color must be G, Y, or B\n";
                        continue;
                    }

                    solver.add_hint(i, letter, color);
                }

                solver.apply_hints();
            } else if (cmd == "list") {
                cout << solver.list().size() << " options:\n";

                for (auto &w : solver.list()) {
                    cout << "  " << w << '\n';
                }
            } else if (cmd == "info") {
                cout << "Dictionary size: " << dict.size() << '\n';
            } else {
                cerr << "Unknown command: " << cmd << '\n';
            }
        }
    }

    return 0;
}

/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef WIN32
// Disable deprecated API warnings.
#pragma warning(disable : 4996)
#endif

#include "cluon/TCPConnection.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM(argv[0]); // NOLINT
    if (3 != argc) {
        std::cerr << PROGRAM
                  << " demonstrates how to use libcluon to send & receive data using a TCP connection (running for 60s)." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " IPv4-address port" << std::endl;
        std::cerr << "Example: " << PROGRAM << " 127.0.0.1 1234" << std::endl;
    } else {
        const std::string ADDRESS(argv[1]); // NOLINT
        const std::string PORT(argv[2]); // NOLINT

        cluon::TCPConnection connection(
            ADDRESS,
            static_cast<uint16_t>(std::stoi(PORT)),
            [](std::string && data, std::chrono::system_clock::time_point && ts) noexcept {
                const auto timestamp(std::chrono::system_clock::to_time_t(ts));
                std::cout << "Received " << data.size() << " bytes at "
                          << timestamp << "s" << ", containing '" << data
                          << "'." << std::endl;
            },
            [](){ std::cout << "Connection lost." << std::endl; });

        if (connection.isRunning()) {
            using namespace std::literals::chrono_literals; // NOLINT
            std::this_thread::sleep_for(5s);
            connection.send("Sending demo data!");

            std::this_thread::sleep_for(60s);
            retVal = 0;
        }
    }
    return retVal;
}

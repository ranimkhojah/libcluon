/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_OD4TOJSON_HPP
#define CLUON_OD4TOJSON_HPP

#include "cluon/cluon.hpp"
#include "cluon/OD4Session.hpp"
#include "cluon/EnvelopeConverter.hpp"
#include "cluon/Time.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

inline int32_t cluon_OD4toJSON(int32_t argc, char **argv) {
    int retVal{1};
    const std::string PROGRAM{argv[0]}; // NOLINT
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if (0 == commandlineArguments.count("cid")) {
        std::cerr << PROGRAM
                  << " dumps Containers received from an OpenDaVINCI v4 session to stdout in JSON format using an optionally supplied ODVD message specification file." << std::endl;
        std::cerr << "Usage:    " << PROGRAM << " [--odvd=<ODVD message specification file>] --cid=<OpenDaVINCI session>" << std::endl;
        std::cerr << "Examples: " << PROGRAM << " --cid=111" << std::endl;
        std::cerr << "          " << PROGRAM << " --odvd=MyMessages.odvd --cid=111" << std::endl;
    } else {
        std::string odvdFile{commandlineArguments["odvd"]};
        cluon::EnvelopeConverter envConverter;
        if (!odvdFile.empty()) {
            std::fstream fin{odvdFile, std::ios::in};
            if (fin.good()) {
                const std::string s{static_cast<std::stringstream const&>(std::stringstream() << fin.rdbuf()).str()}; // NOLINT
                std::clog << "Parsed " << envConverter.setMessageSpecification(s) << " message(s)." << std::endl;
            }
        }

        cluon::OD4Session od4Session(static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])),
            [&e2J = envConverter](cluon::data::Envelope &&envelope) noexcept {
                envelope.received(cluon::time::now());
                std::cout << e2J.getJSONFromEnvelope(envelope) << std::endl;
                std::cout.flush();
            });

        if (od4Session.isRunning()) {
            using namespace std::literals::chrono_literals; // NOLINT
            while (od4Session.isRunning()) {
                std::this_thread::sleep_for(1s);
            }
            retVal = 0;
        }
    }
    return retVal;
}

#endif

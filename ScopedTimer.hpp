/**
 * @file
 * @brief ScopedTimer
 *
 * Simple timer to measure wall-time. By default its
 * measures the time span between creation and deletion.
 *
 * @author Manuel Penschuck
 * @copyright
 * Copyright (C) 2017 Manuel Penschuck
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#ifndef SCOPED_TIMER_HPP
#define SCOPED_TIMER_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>

class ScopedTimer {
    using Clock = std::chrono::high_resolution_clock;

    std::string _prefix;
    Clock::time_point _begin;

    double _reference;

    double* _output;

public:
    ScopedTimer()
          : _begin(Clock::now()), _output(nullptr), _reference(0)
    {}

    ScopedTimer(const std::string& prefix, double reference=0.0)
          : _prefix(prefix), _begin(Clock::now()), _reference(reference), _output(nullptr)
    {}

    ScopedTimer(double& output, const std::string& prefix = "")
      : _prefix(prefix), _begin(Clock::now()),  _reference(0), _output(&output) {}

    ~ScopedTimer() {
        if (!_prefix.empty())
            report();

        if (_output)
            *_output = elapsed();
    }

    void start() {
        _begin = Clock::now();
    }

    double elapsed() const {
        const auto t2 = Clock::now();
        std::chrono::duration<double> time_span =
                std::chrono::duration_cast<std::chrono::duration<double>>(t2 - _begin);

        return (time_span.count()*1e3);
    }

    double report() const {
        return report(_prefix);
    }

    double report(const std::string & prefix) const {
        const double timeUs = elapsed();

        std::stringstream ss;

        ss << std::setprecision(1) << std::fixed;

        if (_reference <= 0.0) {
            ss << prefix << " Time elapsed: " << std::setw(12) << timeUs << "ms";
        } else {
            ss << prefix << " Time elapsed: " << std::setw(12) << timeUs << "ms "
              "(speed up:" << std::setw(5) << (_reference / timeUs) << ")";
        }

        std::cout << ss.str() << std::endl;

        return timeUs;
    }
};

#endif

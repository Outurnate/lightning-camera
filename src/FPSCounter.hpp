/* lightning-camera
 * Copyright (C) 2020 Joe Dillon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef FPSCOUNTER_HPP
#define FPSCOUNTER_HPP

#include <chrono>

#include "MovingAverage.hpp"

class FPSCounter
{
public:
  FPSCounter(size_t samplesToAverage = 5);

  void Update();
  double GetFPS() const;
  double GetFPSAveraged() const;
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> lastFrame;
  MovingAverage<double> samples;
};

#endif
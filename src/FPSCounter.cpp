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

#include "FPSCounter.hpp"

using ms = std::chrono::duration<double, std::milli>;

FPSCounter::FPSCounter(size_t samplesToAverage)
  : lastFrame(std::chrono::high_resolution_clock::now()),
    samples(samplesToAverage, 0.0)
{
}

void FPSCounter::Update()
{
  samples.Push(GetFPS());
  lastFrame = std::chrono::high_resolution_clock::now();
}

double FPSCounter::GetFPS() const
{
  return 1000.0 / std::chrono::duration_cast<ms>(std::chrono::high_resolution_clock::now() - lastFrame).count();
}

double FPSCounter::GetFPSAveraged() const
{
  return samples.Mean();
}
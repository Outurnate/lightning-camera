/* stormwatch
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

#ifndef MOVINGAVERAGE_HPP
#define MOVINGAVERAGE_HPP

#include <vector>
#include <numeric>

template<typename T>
class MovingAverage
{
public:
  MovingAverage(size_t window, T initialValue)
  : values(window, initialValue),
    position(0),
    initialValue(initialValue) { }
  
  void Push(T value)
  {
    values[position] = value;
    position = (position + 1) % values.size();
  }

  T Mean() const
  {
    return std::accumulate(values.begin(), values.end(), initialValue) / values.size();
  }
private:
  std::vector<T> values;
  size_t position;
  T initialValue;
};

#endif
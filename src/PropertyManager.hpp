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

#ifndef PROPERTYMANAGER_HPP
#define PROPERTYMANAGER_HPP

#include <magic_enum.hpp>

enum class Property
{
  EdgeDetectionSeconds,
  DebounceSeconds,
  TriggerDelay,
  TriggerThreshold,
  ClipLengthSeconds,
  BayerMode,
  Width,
  Height
};
constexpr auto PropertyEntries = magic_enum::enum_entries<Property>();

class PropertyManager
{
};

#endif
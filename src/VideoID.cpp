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

#include "VideoID.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <base64.h>

VideoID::VideoID()
{
  auto timestamp = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
  id = base64_encode(reinterpret_cast<const unsigned char*>(timestamp.c_str()), timestamp.length());
}

VideoID::VideoID(const std::string& id)
  : id(id)
{
  // try to decode
  boost::posix_time::from_iso_extended_string(GetTimestamp());
}

const std::string& VideoID::GetID() const
{
  return id;
}

const std::string VideoID::GetTimestamp() const
{
  return base64_decode(id);
}
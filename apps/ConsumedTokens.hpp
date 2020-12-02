/*
 * Copyright ( C ) 2020 New Mexico State University
 *
 * George Torres, Anju Kunnumpurathu James
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * ( at your option ) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NDN_CONSUMEDTOKEN_H
#define NDN_CONSUMEDTOKEN_H

#include "NFD/daemon/fw/ndn-token-bucket.hpp"
#include <unordered_map>

using namespace std;

namespace nfd {
namespace fw {

/**
 * @brief Links application layer to strategy layer for the token bucket.
 *
 * Links the token bucket applications to the respective qos-strategy instances.
 */
struct  Consumed {
  double m_tokens;
  double m_need;
  unordered_map<int,bool> hasSender;

  unordered_map<int,TokenBucket*> sender1;  
  unordered_map<int,TokenBucket*> sender2;
  unordered_map<int,TokenBucket*> sender3;
};

extern Consumed CT;

}
}
#endif
//
// Copyright (C) 2012 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_LINKSTATUS_H_
#define __INET_LINKSTATUS_H_

#include "INETDefs.h"

class INET_API LinkStatus : public cSimpleModule {
  public:
    enum Status {
        LINK_PLUGGED,
        LINK_UNPLUGGED
    };

    static simsignal_t linkStatusSignal;

  private:
    Status status;

  public:
    LinkStatus() { }
    virtual ~LinkStatus() { }

    Status getStatus() const { return status; }

  private:
    void signalStatus() { emit(linkStatusSignal, this); }
};

#endif

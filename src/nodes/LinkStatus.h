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
    bool updateLink;
    Status status;
    cChannel * link;

  public:
    LinkStatus() { }
    virtual ~LinkStatus() { }

    static Status getStatusWithDefault(LinkStatus * linkStatus, Status defaultValue = LINK_PLUGGED) { return linkStatus ? linkStatus->getStatus() : defaultValue; }
    Status getStatus() const { return status; }
    void setStatus(Status status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  private:
    void initialize(int stage);
    void handleMessage(cMessage * message) { throw cRuntimeError("This module doesn't handle messages"); }

  private:
    void updateDisplayString();
    void signalStatus();
};

#endif

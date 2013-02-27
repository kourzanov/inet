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
#include "IStatus.h"

class INET_API LinkStatusData : public IStatus
{
  public:
    enum Status {
      LINK_PLUGGED,
      LINK_UNPLUGGED
    };

    static simsignal_t linkStatusSignal; // the signal used to notify listeners about status changes

  protected:
    Status status;    // the actual status
    cModule * module; // the module representing this status or NULL if there's no such module
    cChannel * link;  // the network link belonging to this status of NULL if there's no such link

  public:
    LinkStatusData() : status(LINK_PLUGGED), module(NULL), link(NULL) { }
    virtual ~LinkStatusData() { }

    static Status getStatusWithDefault(LinkStatusData * linkStatusData, Status defaultValue = LINK_PLUGGED) {
        return linkStatusData ? linkStatusData->getStatus() : defaultValue;
    }

    Status getStatus() const { return status; }
    void setStatus(Status status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  protected:
    void updateDisplayString();
    void signalStatus();
};

class INET_API LinkStatus : public cSimpleModule, public LinkStatusData
{
  public:
    LinkStatus() { }
    virtual ~LinkStatus() { }

  protected:
    void initialize(int stage);
    void handleMessage(cMessage * message) { throw cRuntimeError("This module doesn't handle messages"); }
};

#endif

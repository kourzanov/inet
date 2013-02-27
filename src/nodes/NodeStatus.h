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

#ifndef __INET_NODESTATUS_H_
#define __INET_NODESTATUS_H_

#include "INETDefs.h"
#include "IStatus.h"

class INET_API NodeStatusData : public IStatus
{
  public:
    enum Status {
      NODE_ON,
      NODE_OFF
    };

    static simsignal_t nodeStatusSignal; // the signal used to notify listeners about status changes

  protected:
    Status status;    // the actual status
    cModule * module; // the module representing this status or NULL if there's no such module
    cModule * node;   // the network node belonging to this status of NULL if there's no such node

  public:
    NodeStatusData() : status(NODE_ON), module(NULL), node(NULL) { }
    virtual ~NodeStatusData() { }

    static Status getStatusWithDefault(NodeStatusData * nodeStatusData, Status defaultValue = NODE_ON) {
        return nodeStatusData ? nodeStatusData->getStatus() : defaultValue;
    }

    Status getStatus() const { return status; }
    void setStatus(Status status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  protected:
    void updateDisplayString();
    void signalStatus();
};

class INET_API NodeStatus : public cSimpleModule, public NodeStatusData
{
  public:
    NodeStatus() { }
    virtual ~NodeStatus() { }

  protected:
    void initialize(int stage);
    void handleMessage(cMessage * message) { throw cRuntimeError("This module doesn't handle messages"); }
};

#endif

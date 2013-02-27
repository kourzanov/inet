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

class INET_API NodeStatus : public cSimpleModule, public IStatus {
  public:
    enum Status {
        NODE_ON,
        NODE_OFF
    };

    static simsignal_t nodeStatusSignal;

  private:
    bool updateNode;
    Status status;
    cModule * node;

  public:
    NodeStatus() { }
    virtual ~NodeStatus() { }

    static Status getStatusWithDefault(NodeStatus * nodeStatus, Status defaultValue = NODE_ON) { return nodeStatus ? nodeStatus->getStatus() : defaultValue; }
    Status getStatus() const { return status; }
    void setStatus(Status status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  protected:
    void initialize(int stage);
    void handleMessage(cMessage * message) { throw cRuntimeError("This module doesn't handle messages"); }

  private:
    void updateDisplayString();
    void signalStatus();
};

#endif

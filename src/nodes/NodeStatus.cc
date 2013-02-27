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

#include "NodeStatus.h"
#include "ModuleAccess.h"

Define_Module(NodeStatus)

simsignal_t NodeStatus::nodeStatusSignal = SIMSIGNAL_NULL;

void NodeStatus::initialize(int stage)
{
    if (stage == 0) {
        updateNode = par("updateNode");
        nodeStatusSignal = registerSignal("nodeStatusChanged");
        node = findContainingNode(this);
        setStatus(NODE_ON);
    }
}

void NodeStatus::setStatus(Status status)
{
    this->status = status;
    updateDisplayString();
    signalStatus();
}

const char * NodeStatus::getStatusByName() const
{
    switch (status) {
        case NODE_ON:
            return "on";
        case NODE_OFF:
            return "off";
        default:
            throw cRuntimeError("Unknown status");
    }
}

void NodeStatus::setStatusByName(const char * name)
{
    if (!strcmp(name, "on"))
        setStatus(NODE_ON);
    else if (!strcmp(name, "off"))
        setStatus(NODE_OFF);
    else
        throw cRuntimeError("Unknown status");
}

void NodeStatus::updateDisplayString()
{
    const char * icon;
    switch (status) {
        case NODE_ON:
            icon = "status/check";
            break;
        case NODE_OFF:
            icon = "status/cross";
            break;
        default:
            throw cRuntimeError("Unknown status");
    }
    getDisplayString().setTagArg("i", 0, icon);
    if (updateNode)
        node->getDisplayString().setTagArg("i2", 0, icon);
}

void NodeStatus::signalStatus()
{
    emit(nodeStatusSignal, this);
}

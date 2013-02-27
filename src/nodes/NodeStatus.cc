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

simsignal_t NodeStatusData::nodeStatusSignal = SIMSIGNAL_NULL;

void NodeStatusData::setStatus(Status status)
{
    this->status = status;
    updateDisplayString();
    signalStatus();
}

const char * NodeStatusData::getStatusByName() const
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

void NodeStatusData::setStatusByName(const char * name)
{
    if (!strcmp(name, "on"))
        setStatus(NODE_ON);
    else if (!strcmp(name, "off"))
        setStatus(NODE_OFF);
    else
        throw cRuntimeError("Unknown status");
}

void NodeStatusData::updateDisplayString()
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
    if (module)
        module->getDisplayString().setTagArg("i", 0, icon);
    if (node)
        node->getDisplayString().setTagArg("i2", 0, icon);
}

void NodeStatusData::signalStatus()
{
    if (module)
        module->emit(nodeStatusSignal, module);
}

void NodeStatus::initialize(int stage)
{
    if (stage == 0) {
        module = this;
        node = par("updateNode") ? findContainingNode(this) : NULL;
        nodeStatusSignal = registerSignal("nodeStatusChanged");
        setStatus(NODE_ON);
    }
}

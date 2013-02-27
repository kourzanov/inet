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

#include "InterfaceStatus.h"

Define_Module(InterfaceStatus)

simsignal_t InterfaceStatusData::interfaceStatusSignal = SIMSIGNAL_NULL;

void InterfaceStatusData::setStatus(Status status)
{
    this->status = status;
    updateDisplayString();
    signalStatus();
}

const char * InterfaceStatusData::getStatusByName() const
{
    switch (status) {
        case INTERFACE_UP:
            return "up";
        case INTERFACE_DOWN:
            return "down";
        default:
            throw cRuntimeError("Unknown status");
    }
}

void InterfaceStatusData::setStatusByName(const char * name)
{
    if (!strcmp(name, "up"))
        setStatus(INTERFACE_UP);
    else if (!strcmp(name, "down"))
        setStatus(INTERFACE_DOWN);
    else
        throw cRuntimeError("Unknown status");
}

void InterfaceStatusData::updateDisplayString()
{
    const char * icon;
    switch (status) {
        case INTERFACE_UP:
            icon = "status/up";
            break;
        case INTERFACE_DOWN:
            icon = "status/down";
            break;
        default:
            throw cRuntimeError("Unknown status");
    }
    if (module)
        module->getDisplayString().setTagArg("i", 0, icon);
    if (interface)
        interface->getDisplayString().setTagArg("i2", 0, icon);
}

void InterfaceStatusData::signalStatus()
{
    if (module)
        module->emit(interfaceStatusSignal, module);
}

void InterfaceStatus::initialize(int stage)
{
    if (stage == 0) {
        module = this;
        interface = par("updateInterface") ? getParentModule() : NULL;
        interfaceStatusSignal = registerSignal("interfaceStatusChanged");
        setStatus(INTERFACE_UP);
    }
}

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

#include "LinkStatus.h"

Define_Module(LinkStatus)

simsignal_t LinkStatusData::linkStatusSignal = SIMSIGNAL_NULL;

void LinkStatusData::setStatus(Status status)
{
    this->status = status;
    updateDisplayString();
    signalStatus();
}

const char * LinkStatusData::getStatusByName() const
{
    switch (status) {
        case LINK_PLUGGED:
            return "on";
        case LINK_UNPLUGGED:
            return "off";
        default:
            throw cRuntimeError("Unknown status");
    }
}

void LinkStatusData::setStatusByName(const char * name)
{
    if (!strcmp(name, "on"))
        setStatus(LINK_PLUGGED);
    else if (!strcmp(name, "off"))
        setStatus(LINK_UNPLUGGED);
    else
        throw cRuntimeError("Unknown status");
}

void LinkStatusData::updateDisplayString()
{
    const char * icon;
    switch (status) {
        case LINK_PLUGGED:
            icon = "status/connect";
            break;
        case LINK_UNPLUGGED:
            icon = "status/disconnect";
            break;
        default:
            throw cRuntimeError("Unknown status");
    }
    if (module)
        module->getDisplayString().setTagArg("i", 0, icon);
    if (link)
        link->getDisplayString().setTagArg("i2", 0, icon);
}

void LinkStatusData::signalStatus()
{
    if (module)
        module->emit(linkStatusSignal, module);
}

void LinkStatus::initialize(int stage)
{
    if (stage == 0) {
        module = this;
        link = NULL; // TODO: par("updateLink") ? NULL : NULL;
        linkStatusSignal = registerSignal("linkStatusChanged");
        setStatus(LINK_PLUGGED);
    }
}

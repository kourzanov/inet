//
// Copyright (C) 2004, 2009 Andras Varga
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

#include <string.h>

#include "SCMP.h"

#include "GenericNetworkProtocolControlInfo.h"
#include "PingPayload_m.h"

Define_Module(SCMP);

void SCMP::handleMessage(cMessage *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    if (!strcmp(arrivalGate->getName(), "localIn"))
        processPacket(check_and_cast<SCMPPacket *>(msg));
    else if (!strcmp(arrivalGate->getName(), "pingIn"))
        sendEchoRequest(check_and_cast<PingPayload *>(msg));
}

void SCMP::processPacket(SCMPPacket *msg)
{
    switch (msg->getType())
    {
        case SCMP_ECHO_REQUEST:
            processEchoRequest(msg);
            break;
        case SCMP_ECHO_REPLY:
            processEchoReply(msg);
            break;
        default:
            throw cRuntimeError("Unknown SCMP type %d", msg->getType());
    }
}

void SCMP::processEchoRequest(SCMPPacket *request)
{
    // turn request into a reply
    SCMPPacket *reply = request;
    reply->setName((std::string(request->getName()) + "-reply").c_str());
    reply->setType(SCMP_ECHO_REPLY);
    // swap src and dest
    // TBD check what to do if dest was multicast etc?
    GenericNetworkProtocolControlInfo *ctrl = check_and_cast<GenericNetworkProtocolControlInfo *>(reply->getControlInfo());
    Address src = ctrl->getSourceAddress();
    Address dest = ctrl->getDestinationAddress();
    ctrl->setInterfaceId(-1);
    ctrl->setSourceAddress(dest);
    ctrl->setDestinationAddress(src);
    send(reply, "sendOut");
}

void SCMP::processEchoReply(SCMPPacket *reply)
{
    GenericNetworkProtocolControlInfo *ctrl = check_and_cast<GenericNetworkProtocolControlInfo*>(reply->removeControlInfo());
    PingPayload *payload = check_and_cast<PingPayload *>(reply->decapsulate());
    payload->setControlInfo(ctrl);
    delete reply;
    long originatorId = payload->getOriginatorId();
    PingMap::iterator i = pingMap.find(originatorId);
    if (i != pingMap.end())
        send(payload, "pingOut", i->second);
    else
    {
        EV << "Received ECHO REPLY has an unknown originator ID: " << originatorId << ", packet dropped." << endl;
        delete payload;
    }
}

void SCMP::sendEchoRequest(PingPayload *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    int i = arrivalGate->getIndex();
    pingMap[msg->getOriginatorId()] = i;

    GenericNetworkProtocolControlInfo *ctrl = check_and_cast<GenericNetworkProtocolControlInfo*>(msg->removeControlInfo());
    ctrl->setProtocol(IP_PROT_ICMP);
    SCMPPacket *request = new SCMPPacket(msg->getName());
    request->setType(SCMP_ECHO_REQUEST);
    request->encapsulate(msg);
    request->setControlInfo(ctrl);
    send(request, "sendOut");
}
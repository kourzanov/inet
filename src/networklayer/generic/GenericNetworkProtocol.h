//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef __INET_GENERICNETWORKPROTOCOL_H
#define __INET_GENERICNETWORKPROTOCOL_H

#include "QueueBase.h"
#include "InterfaceTableAccess.h"
#include "GenericRoutingTable.h"
#include "GenericControlInfo_m.h"
#include "GenericDatagram.h"
#include "ProtocolMap.h"

/**
 * Implements a simple network protocol.
 */
class INET_API GenericNetworkProtocol : public QueueBase
{
  protected:
    GenericRoutingTable *rt;  //TODO change to IGenericRoutingTable?
    IInterfaceTable *ift;
    cGate *queueOutGate;

    // config
    int defaultHopLimit;

    // working vars
    ProtocolMapping mapping; // where to send packets after decapsulation

    // statistics
    int numLocalDeliver;
    int numDropped;
    int numUnroutable;
    int numForwarded;

  protected:
    // utility: look up interface from getArrivalGate()
    virtual InterfaceEntry *getSourceInterfaceFrom(cPacket *msg);

    // utility: show current statistics above the icon
    virtual void updateDisplayString();

    /**
     * Encapsulate packet coming from higher layers into GenericDatagram, using
     * the control info attached to the packet.
     */
    virtual GenericDatagram *encapsulate(cPacket *transportPacket, InterfaceEntry *&destIE);

    /**
     * Encapsulate packet coming from higher layers into GenericDatagram, using
     * the given control info. Override if you subclassed controlInfo and/or
     * want to add options etc to the datagram.
     */
    virtual GenericDatagram *encapsulate(cPacket *transportPacket, InterfaceEntry *&destIE, GenericControlInfo *controlInfo);

    /**
     * Creates a blank Generic datagram. Override when subclassing GenericDatagram is needed
     */
    virtual GenericDatagram *createGenericDatagram(const char *name);

    /**
     * Handle GenericDatagram messages arriving from lower layer.
     * Decrements TTL, then invokes routePacket().
     */
    virtual void handlePacketFromNetwork(GenericDatagram *datagram);

    /**
     * Handle messages (typically packets to be send in Generic) from transport or ICMP.
     * Invokes encapsulate(), then routePacket().
     */
    virtual void handleMessageFromHL(cPacket *msg);

    /**
     * Performs routing. Based on the routing decision, it dispatches to
     * reassembleAndDeliver() for local packets, to fragmentAndSend() for forwarded packets,
     * to handleMulticastPacket() for multicast packets, or drops the packet if
     * it's unroutable or forwarding is off.
     */
    virtual void routePacket(GenericDatagram *datagram, InterfaceEntry *destIE, bool fromHL);

    /**
     * Forwards packets to all multicast destinations, using fragmentAndSend().
     */
    virtual void routeMulticastPacket(GenericDatagram *datagram, InterfaceEntry *destIE, InterfaceEntry *fromIE);

    /**
     * Perform reassembly of fragmented datagrams, then send them up to the
     * higher layers using sendToHL().
     */
    virtual void reassembleAndDeliver(GenericDatagram *datagram);

    /**
     * Decapsulate and return encapsulated packet after attaching GenericControlInfo.
     */
    virtual cPacket *decapsulateGeneric(GenericDatagram *datagram);

    /**
     * Fragment packet if needed, then send it to the selected interface using
     * sendDatagramToOutput().
     */
    virtual void fragmentAndSend(GenericDatagram *datagram, InterfaceEntry *ie, Address nextHopAddr);

    /**
     * Last TTL check, then send datagram on the given interface.
     */
    virtual void sendDatagramToOutput(GenericDatagram *datagram, InterfaceEntry *ie, Address nextHopAddr);

  public:
    GenericNetworkProtocol() {}

  protected:
    /**
     * Initialization
     */
    virtual void initialize();

    /**
     * Processing of Generic datagrams. Called when a datagram reaches the front
     * of the queue.
     */
    virtual void endService(cPacket *pk);
};

#endif

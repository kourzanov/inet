/*
 * Copyright (C) 2004 Andras Varga
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __INET_ARP_H
#define __INET_ARP_H

//#include <stdio.h>
//#include <string.h>
//#include <vector>
#include <map>

#include "INETDefs.h"

#include "MACAddress.h"
#include "ModuleAccess.h"
#include "IPv4Address.h"
#include "IARPCache.h"

// Forward declarations:
class ARPPacket;
class IInterfaceTable;
class InterfaceEntry;
class IIPv4RoutingTable;

/**
 * ARP implementation.
 */
class INET_API ARP : public cSimpleModule, public IARPCache
{
  public:
    struct ARPCacheEntry;
    typedef std::map<IPv4Address, ARPCacheEntry*> ARPCache;
    typedef std::vector<cMessage*> MsgPtrVector;

    // IPv4Address -> MACAddress table
    // TBD should we key it on (IPv4Address, InterfaceEntry*)?
    struct ARPCacheEntry
    {
        InterfaceEntry *ie; // NIC to send the packet to
        bool pending; // true if resolution is pending
        MACAddress macAddress;  // MAC address
        simtime_t lastUpdate;  // entries should time out after cacheTimeout
        int numRetries; // if pending==true: 0 after first ARP request, 1 after second, etc.
        cMessage *timer;  // if pending==true: request timeout msg
        MsgPtrVector pendingPackets;  // if pending==true: ptrs to packets waiting for resolution
                                      // (packets are owned by pendingQueue)
        ARPCache::iterator myIter;  // iterator pointing to this entry
    };

  protected:
    simtime_t retryTimeout;
    int retryCount;
    simtime_t cacheTimeout;
    bool doProxyARP;
    bool globalARP;

    long numResolutions;
    long numFailedResolutions;
    long numRequestsSent;
    long numRepliesSent;

    static simsignal_t sentReqSignal;
    static simsignal_t sentReplySignal;
    static simsignal_t failedResolutionSignal;
    static simsignal_t initiatedResolutionSignal;

    ARPCache arpCache;
    static ARPCache globalArpCache;
    static int globalArpCacheRefCnt;

    cQueue pendingQueue; // outbound packets waiting for ARP resolution

    IInterfaceTable *ift;
    IIPv4RoutingTable *rt;  // for answering ProxyARP requests
    cGate *ipOutGate;

  public:
    ARP();
    virtual ~ARP();
    int numInitStages() const {return 5;}
    virtual MACAddress getDirectAddressResolution(const IPv4Address &) const;
    virtual IPv4Address getInverseAddressResolution(const MACAddress &) const;
    void setChangeAddress(const IPv4Address &);

  protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual void processOutboundPacket(cMessage *msg);
    virtual void sendPacketToNIC(cMessage *msg, InterfaceEntry *ie, const MACAddress& macAddress, int etherType);

    virtual void initiateARPResolution(ARPCacheEntry *entry);
    virtual void sendARPRequest(InterfaceEntry *ie, IPv4Address ipAddress);
    virtual void requestTimedOut(cMessage *selfmsg);
    virtual bool addressRecognized(IPv4Address destAddr, InterfaceEntry *ie);
    virtual void processARPPacket(ARPPacket *arp);
    virtual void updateARPCache(ARPCacheEntry *entry, const MACAddress& macAddress);

    virtual void dumpARPPacket(ARPPacket *arp);
    virtual void updateDisplayString();

};

#endif


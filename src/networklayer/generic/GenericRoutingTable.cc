//
// Copyright (C) 2013 Andras Varga
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

#include <algorithm>
#include <sstream>

#include "GenericRoutingTable.h"

#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "GenericRoute.h"
#include "GenericNetworkProtocolInterfaceData.h"
#include "NotificationBoard.h"
#include "NotifierConsts.h"


Define_Module(GenericRoutingTable);

GenericRoutingTable::GenericRoutingTable()
{
    ift = NULL;
    nb = NULL;
}

GenericRoutingTable::~GenericRoutingTable()
{
    for (unsigned int i=0; i<routes.size(); i++)
        delete routes[i];
    for (unsigned int i=0; i<multicastRoutes.size(); i++)
        delete multicastRoutes[i];
}

void GenericRoutingTable::initialize(int stage)
{
    if (stage==0)
    {
        // get a pointer to the NotificationBoard module and IInterfaceTable
        nb = NotificationBoardAccess().get();
        ift = InterfaceTableAccess().get();

        const char * addressTypeString = par("addressType");
        if (!strcmp(addressTypeString, "mac"))
            addressType = Address::MAC;
        else if (!strcmp(addressTypeString, "modulepath"))
            addressType = Address::MODULEPATH;
        else if (!strcmp(addressTypeString, "moduleid"))
            addressType = Address::MODULEID;
        else
            throw cRuntimeError("Unknown address type");
        forwardingEnabled = par("forwardingEnabled").boolValue();
        multicastForwardingEnabled = par("multicastForwardingEnabled");

        nb->subscribe(this, NF_INTERFACE_CREATED);
        nb->subscribe(this, NF_INTERFACE_DELETED);
        nb->subscribe(this, NF_INTERFACE_STATE_CHANGED);
        nb->subscribe(this, NF_INTERFACE_CONFIG_CHANGED);
        nb->subscribe(this, NF_INTERFACE_IPv4CONFIG_CHANGED);

//TODO        WATCH_PTRVECTOR(routes);
//TODO        WATCH_PTRVECTOR(multicastRoutes);
        WATCH(forwardingEnabled);
        WATCH(multicastForwardingEnabled);
        WATCH(routerId);
    }
    else if (stage==1)
    {
//        // L2 modules register themselves in stage 0, so we can only configure
//        // the interfaces in stage 1.
//        const char *filename = par("routingFile");

        // At this point, all L2 modules have registered themselves (added their
        // interface entries). Create the per-interface IPv4 data structures.
        IInterfaceTable *interfaceTable = InterfaceTableAccess().get();
        for (int i=0; i<interfaceTable->getNumInterfaces(); ++i)
            configureInterface(interfaceTable->getInterface(i));
        configureLoopback();

//        // read routing table file (and interface configuration)
//        RoutingTableParser parser(ift, this);
//        if (*filename && parser.readRoutingTableFromFile(filename)==-1)
//            error("Error reading routing table file %s", filename);

//TODO
//        // set routerId if param is not "" (==no routerId) or "auto" (in which case we'll
//        // do it later in stage 3, after network configurators configured the interfaces)
//        const char *routerIdStr = par("routerId").stringValue();
//        if (strcmp(routerIdStr, "") && strcmp(routerIdStr, "auto"))
//            routerId = IPv4Address(routerIdStr);
    }
    else if (stage==3)
    {
        // routerID selection must be after stage==2 when network autoconfiguration
        // assigns interface addresses
        configureRouterId();

//        // we don't use notifications during initialize(), so we do it manually.
//        // Should be in stage=3 because autoconfigurator runs in stage=2.
//        updateNetmaskRoutes();

        //printRoutingTable();
    }
}

void GenericRoutingTable::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module doesn't process messages");
}

void GenericRoutingTable::receiveChangeNotification(int category, const cObject *details)
{
    // TODO:
}

void GenericRoutingTable::configureRouterId()
{
    if (routerId.isUnspecified())  // not yet configured
    {
        const char *routerIdStr = par("routerId").stringValue();
        if (!strcmp(routerIdStr, "auto"))  // non-"auto" cases already handled in stage 1
        {
            // choose highest interface address as routerId
            for (int i=0; i<ift->getNumInterfaces(); ++i)
            {
                InterfaceEntry *ie = ift->getInterface(i);
                if (!ie->isLoopback()) {
                    Address interfaceAddr = ie->getGenericNetworkProtocolData()->getAddress();
                    if (routerId.isUnspecified() || routerId < interfaceAddr)
                        routerId = interfaceAddr;
                }
            }
        }
    }
//    else // already configured
//    {
//        // if there is no interface with routerId yet, assign it to the loopback address;
//        // TODO find out if this is a good practice, in which situations it is useful etc.
//        if (getInterfaceByAddress(routerId)==NULL)
//        {
//            InterfaceEntry *lo0 = ift->getFirstLoopbackInterface();
//            lo0->getGenericNetworkProtocolData()->setAddress(routerId);
//        }
//    }
}

void GenericRoutingTable::configureInterface(InterfaceEntry *ie)
{
    int metric = (int) (ceil(2e9 / ie->getDatarate()));  // use OSPF cost as default
    int interfaceModuleId = ie->getInterfaceModule() ? ie->getInterfaceModule()->getParentModule()->getId() : -1;
    // mac
    GenericNetworkProtocolInterfaceData *d = new GenericNetworkProtocolInterfaceData();
    d->setMetric(metric);
    if (addressType == Address::MAC)
        d->setAddress(ie->getMacAddress());
    else if (ie->getInterfaceModule() && addressType == Address::MODULEPATH)
        d->setAddress(ModulePathAddress(interfaceModuleId));
    else if (ie->getInterfaceModule() && addressType == Address::MODULEID)
        d->setAddress(ModuleIdAddress(interfaceModuleId));
    ie->setGenericNetworkProtocolData(d);
}

void GenericRoutingTable::configureLoopback()
{
    InterfaceEntry *ie = ift->getFirstLoopbackInterface();

//TODO needed???
//    // add IPv4 info. Set 127.0.0.1/8 as address by default --
//    // we may reconfigure later it to be the routerId
//    IPv4InterfaceData *d = new IPv4InterfaceData();
//    d->setIPAddress(IPv4Address::LOOPBACK_ADDRESS);
//    d->setNetmask(IPv4Address::LOOPBACK_NETMASK);
//    d->setMetric(1);
//    ie->setIPv4Data(d);
}


void GenericRoutingTable::updateDisplayString()
{
    if (!ev.isGUI())
        return;

//TODO
//    char buf[80];
//    if (routerId.isUnspecified())
//        sprintf(buf, "%d+%d routes", (int)routes.size(), (int)multicastRoutes.size());
//    else
//        sprintf(buf, "routerId: %s\n%d+%d routes", routerId.str().c_str(), (int)routes.size(), (int)multicastRoutes.size());
//    getDisplayString().setTagArg("t", 0, buf);
}

bool GenericRoutingTable::routeLessThan(const GenericRoute *a, const GenericRoute *b)
{
    // helper for sort() in addRoute(). We want routes with longer
    // prefixes to be at front, so we compare them as "less".
    // For metric, a smaller value is better (we report that as "less").
    if (a->getPrefixLength() != b->getPrefixLength())
        return a->getPrefixLength() > b->getPrefixLength();

    if (a->getDestination() != b->getDestination())
        return a->getDestination() < b->getDestination();

    return a->getMetric() < b->getMetric();
}

bool GenericRoutingTable::isForwardingEnabled() const
{
    return forwardingEnabled;
}

bool GenericRoutingTable::isMulticastForwardingEnabled() const
{
    return multicastForwardingEnabled;
}

Address GenericRoutingTable::getRouterId() const
{
    return routerId;
}

bool GenericRoutingTable::isLocalAddress(const Address& dest) const
{
    //TODO: Enter_Method("isLocalAddress(%s)", dest.str().c_str());

    // collect interface addresses if not yet done
    for (int i=0; i<ift->getNumInterfaces(); i++)
    {
        Address interfaceAddr = ift->getInterface(i)->getGenericNetworkProtocolData()->getAddress();
        if (interfaceAddr == dest)
            return true;
    }
    return false;
}

InterfaceEntry* GenericRoutingTable::getInterfaceByAddress(const Address& address) const
{
    // collect interface addresses if not yet done
    for (int i=0; i<ift->getNumInterfaces(); i++)
    {
        InterfaceEntry *ie = ift->getInterface(i);
        Address interfaceAddr = ie->getGenericNetworkProtocolData()->getAddress();
        if (interfaceAddr == address)
            return ie;
    }
    return NULL;
}

GenericRoute* GenericRoutingTable::findBestMatchingRoute(const Address& dest) const
{
    //TODO Enter_Method("findBestMatchingRoute(%u.%u.%u.%u)", dest.getDByte(0), dest.getDByte(1), dest.getDByte(2), dest.getDByte(3)); // note: str().c_str() too slow here

    // find best match (one with longest prefix)
    // default route has zero prefix length, so (if exists) it'll be selected as last resort
    GenericRoute *bestRoute = NULL;
    for (RouteVector::const_iterator i=routes.begin(); i!=routes.end(); ++i)
    {
        GenericRoute *e = *i;
        if (e->isEnabled() && !e->isExpired())
        {
            if (dest.matches(e->getDestination(), e->getPrefixLength()))
            {
                bestRoute = const_cast<GenericRoute *>(e);
                break;
            }
        }
    }
    return bestRoute;
}

InterfaceEntry* GenericRoutingTable::getOutputInterfaceForDestination(const Address& dest) const
{
    //TODO Enter_Method("getInterfaceForDestAddr(%u.%u.%u.%u)", dest.getDByte(0), dest.getDByte(1), dest.getDByte(2), dest.getDByte(3)); // note: str().c_str() too slow here

    const IRoute *e = findBestMatchingRoute(dest);
    return e ? e->getInterface() : NULL;
}

Address GenericRoutingTable::getNextHopForDestination(const Address& dest) const
{
    //TODO Enter_Method("getGatewayForDestAddr(%u.%u.%u.%u)", dest.getDByte(0), dest.getDByte(1), dest.getDByte(2), dest.getDByte(3)); // note: str().c_str() too slow here

    const IRoute *e = findBestMatchingRoute(dest);
    return e ? e->getNextHop() : Address();
}

bool GenericRoutingTable::isLocalMulticastAddress(const Address& dest) const
{
    return dest.isMulticast(); //TODO
}

IMulticastRoute* GenericRoutingTable::findBestMatchingMulticastRoute(const Address& origin, const Address& group) const
{
    return NULL; //TODO
}

int GenericRoutingTable::getNumRoutes() const
{
    return routes.size();
}

IRoute* GenericRoutingTable::getRoute(int k) const
{
    ASSERT(k >= 0 && k < routes.size());
    return routes[k];
}

IRoute* GenericRoutingTable::getDefaultRoute() const
{
    // if there is a default route entry, it is the last valid entry
    for (RouteVector::const_reverse_iterator i=routes.rbegin(); i!=routes.rend() && (*i)->getPrefixLength() == 0; ++i)
    {
        if ((*i)->isEnabled() && !(*i)->isExpired())
            return *i;
    }
    return NULL;
}

void GenericRoutingTable::addRoute(IRoute* route)
{
    Enter_Method("addRoute(...)");

    GenericRoute* entry = dynamic_cast<GenericRoute*>(route);

    // check that the interface exists
    if (!entry->getInterface())
        error("addRoute(): interface cannot be NULL");

    // add to tables
    // we keep entries sorted, so that we can stop at the first match when doing the longest prefix matching
    RouteVector::iterator pos = upper_bound(routes.begin(), routes.end(), entry, routeLessThan);
    routes.insert(pos, entry);

    entry->setRoutingTable(this);

    updateDisplayString();

    nb->fireChangeNotification(NF_ROUTE_ADDED, entry);
}

IRoute* GenericRoutingTable::removeRoute(IRoute* route)
{
    Enter_Method("removeRoute(...)");

    GenericRoute* entry = dynamic_cast<GenericRoute*>(route);

    RouteVector::iterator i = std::find(routes.begin(), routes.end(), entry);
    if (i!=routes.end())
    {
        routes.erase(i);
        updateDisplayString();
        ASSERT(entry->getRoutingTable() == this); // still filled in, for the listeners' benefit
        nb->fireChangeNotification(NF_ROUTE_DELETED, entry);
        entry->setRoutingTable(NULL);
        return entry;
    }
    return NULL;
}

bool GenericRoutingTable::deleteRoute(IRoute* entry)
{
    IRoute *route = removeRoute(entry);
    delete route;
    return route != NULL;
}

int GenericRoutingTable::getNumMulticastRoutes() const
{
    return 0; //TODO
}

IMulticastRoute* GenericRoutingTable::getMulticastRoute(int k) const
{
    return NULL; //TODO
}

void GenericRoutingTable::addMulticastRoute(IMulticastRoute* entry)
{
    //TODO
}

IMulticastRoute* GenericRoutingTable::removeMulticastRoute(IMulticastRoute* entry)
{
    return NULL; //TODO
}

bool GenericRoutingTable::deleteMulticastRoute(IMulticastRoute* entry)
{
    return false; //TODO
}

void GenericRoutingTable::purgeExpiredRoutes()
{
}

IRoute* GenericRoutingTable::createRoute()
{
    return new GenericRoute();
}



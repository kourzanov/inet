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

#ifndef __INET_ADDRESS_H
#define __INET_ADDRESS_H

#include "INETDefs.h"
#include "uint128.h"

#include "IPv4Address.h"
#include "IPv6Address.h"
#include "MACAddress.h"
#include "ModuleIdAddress.h"
#include "ModulePathAddress.h"

class IAddressPolicy;

/**
 * TODO
 */
class INET_API Address
{
    public:
        enum AddressType {
            NONE,
            IPv4,
            IPv6,
            MAC,
            MODULEPATH,
            MODULEID
        };
    private:
        uint64 hi;
        uint64 lo;

    private:
        uint64 get(AddressType type) const;
        void set(AddressType type, uint64 lo);

    public:
        Address() { set(NONE, 0); }
        Address(const char *str) { tryParse(str); }
        Address(const IPv4Address& addr) { set(addr); }
        Address(const IPv6Address& addr) { set(addr); }
        Address(const MACAddress& addr) { set(addr); }
        Address(const ModuleIdAddress& addr) { set(addr); }
        Address(const ModulePathAddress& addr) { set(addr); }

        void set(const IPv4Address& addr) { set(IPv4, addr.getInt()); }
        void set(const IPv6Address& addr);
        void set(const MACAddress& addr) { set(MAC, addr.getInt()); }
        void set(const ModuleIdAddress& addr) { set(MODULEID, addr.getId()); }
        void set(const ModulePathAddress& addr) { set(MODULEPATH, addr.getId()); }

        IPv4Address toIPv4() const { return getType() == NONE ? IPv4Address() : IPv4Address(get(IPv4)); }
        IPv6Address toIPv6() const { return IPv6Address(hi, lo); }
        MACAddress toMAC() const { return MACAddress(get(MAC)); }
        ModuleIdAddress toModuleId() const { return ModuleIdAddress(get(MODULEID)); }
        ModulePathAddress toModulePath() const { return ModulePathAddress(get(MODULEPATH)); }

        std::string str() const;
        AddressType getType() const;
        IAddressPolicy * getAddressPolicy() const;

        bool tryParse(const char *addr);

        bool isUnspecified() const;
        bool isUnicast() const;
        bool isMulticast() const;
        bool isBroadcast() const;

        bool operator<(const Address& other) const;
        bool operator==(const Address& other) const;
        bool operator!=(const Address& other) const;

        bool matches(const Address& other, int prefixLength) const;
};

inline std::ostream& operator<<(std::ostream& os, const Address& address)
{
    return os << address.str();
}

#endif

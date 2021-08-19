#ifndef __MYOS__NET__ARP_H
#define __MYOS__NET__ARP_H

#include "common/types.h"
#include "net/etherframe.h"

namespace myos{
    namespace net{
        struct AddressResolutionProtocalMessage{
            common::uint16_t hardwareType;
            common::uint16_t protocal;
            common::uint8_t hardwareAddressSize;
            common::uint8_t protocolAddressSize;
            common::uint16_t command;

            common::uint64_t srcMAC : 48;
            common::uint32_t srcIP;
            common::uint64_t dstMAC : 48;
            common::uint32_t dstIP;

        }__attribute__((packed));


        class AddressResolutionProtocal : public EtherFrameHandler{
        public:
            AddressResolutionProtocal(EtherFrameProvider* backend);
            ~AddressResolutionProtocal();

            bool OnEtherFrameReceived(common::uint8_t* etherframePayload,common::uint32_t size);

            void RequestMACAddress(common::uint32_t IP_BE);
            common::uint64_t Resolve(common::uint32_t IP_BE);
            common::uint64_t GetMACFromCache(common::uint32_t IP_BE);
            // broadcast
            void BroadcastMACAddress(common::uint32_t IP_BE);
        private:
            common::uint32_t IPcache[128];
            common::uint32_t MACcache[128];
            int numCacheEntries;

        };

    }

}


#endif
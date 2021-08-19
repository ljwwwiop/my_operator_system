#include "net/arp.h"

using namespace myos;
using namespace myos::common;
using namespace myos::net;
using namespace myos::drivers;


AddressResolutionProtocal::AddressResolutionProtocal(EtherFrameProvider* backend) 
    : EtherFrameHandler(backend, 0x806)
{
    numCacheEntries = 0;
}

bool AddressResolutionProtocal::OnEtherFrameReceived(uint8_t* etherframePayload,uint32_t size)
{
    if(size < sizeof(AddressResolutionProtocalMessage) )
    {
        return false;
    }

    AddressResolutionProtocalMessage* arp = (AddressResolutionProtocalMessage*)etherframePayload;

    if(arp->hardwareType == 0x0100){
            if(arp->protocal == 0x0008 && arp->hardwareAddressSize ==6 && arp->protocolAddressSize == 4 && arp->dstIP == backend->GetIPAddress()){
                switch(arp->command){
                case 0x0100:
                    // request
                    arp->command == 0x0200;
                    arp->dstIP = arp->srcIP;
                    arp->dstMAC = arp->srcMAC;
                    arp->srcIP = backend->GetIPAddress();
                    arp->srcMAC = backend->GetMACAddress();
                    return true;
                case 0x0200:
                    // response
                    if(numCacheEntries < 128){
                        IPcache[numCacheEntries] = arp->srcIP;
                        MACcache[numCacheEntries] = arp->srcMAC;
                        numCacheEntries++;
                    }
                    break;
                }
            }

    }

    return false;
}

void AddressResolutionProtocal::RequestMACAddress(uint32_t IP_BE)
{
    AddressResolutionProtocalMessage arp;
    arp.hardwareType = 0x0100;
    arp.protocal = 0x0008;
    arp.hardwareAddressSize = 6;
    arp.protocolAddressSize = 4;
    arp.command = 0x0100;
    arp.srcMAC = backend->GetMACAddress();
    arp.srcIP = backend->GetIPAddress();
    arp.dstMAC = 0xffffffffffff;
    arp.dstIP = IP_BE;

    this->Send(arp.dstMAC,(uint8_t*)&arp,sizeof(AddressResolutionProtocalMessage) );
}



uint64_t AddressResolutionProtocal::GetMACFromCache(uint32_t IP_BE)
{
    for(int i = 0; i < numCacheEntries;i++)
    {
        if(IPcache[i] == IP_BE){
            return MACcache[i];
        }
    }
    return 0xffffffffffff;
}

uint64_t AddressResolutionProtocal::Resolve(uint32_t IP_BE)
{
    uint64_t result = GetMACFromCache(IP_BE);
    if(result == 0xffffffffffff){
        RequestMACAddress(IP_BE);
    }

    while( result == 0xffffffffffff){
        result = GetMACFromCache(IP_BE);
    }
    return result;

}


void  AddressResolutionProtocal::BroadcastMACAddress(uint32_t IP_BE){
    AddressResolutionProtocalMessage arp; // 注意大小端
    arp.hardwareType = 0x0100;
    arp.protocal = 0x0008;
    arp.hardwareAddressSize = 6;
    arp.protocolAddressSize = 4;
    arp.command = 0x0200;

    arp.srcMAC = backend->GetMACAddress();
    arp.srcIP = backend->GetIPAddress();
    arp.dstMAC = Resolve(IP_BE);
    arp.dstIP = IP_BE;

    this->Send(arp.dstMAC,(uint8_t*)&arp,sizeof(AddressResolutionProtocalMessage) );
}


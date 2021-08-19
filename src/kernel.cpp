#include "common/types.h"
#include "gdt.h"
#include "hardwarecommunication/interrupts.h"
#include "hardwarecommunication/pci.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "drivers/driver.h"
#include "drivers/vga.h"
#include "gui/desktop.h"
#include "gui/window.h"
#include "multitasking.h"
#include "memorymanagement.h"
#include "drivers/amd_973.h"
#include "net/etherframe.h"
#include "net/arp.h"
#include "net/ipv4.h"
#include "net/icmp.h"
#include "net/udp.h"


using namespace myos;
using namespace myos::gui;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::net;
using namespace myos::hardwarecommunication;

// 实现handler->OnKeyDown
void printf(const char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    // 取出低16位数,要是显示器能够显示最大就是32K字符
    static uint8_t x = 0,y = 0;

    for(int i = 0;str[i];i++){
        switch(str[i])
        {
            case '\n':
                y++;
                x = 0;
                break;
            default:
                VideoMemory[80*y + x] = (VideoMemory[80*y + x] & 0xFF00) |str[i]; 
                x++;
                break;
        }

        if(x >= 80){
            x = 0;
            y++;
        }
        if(y >= 25){
            for(y = 0;y<25;y++){
                for(x = 0;x<80;x++){
                    VideoMemory[80*y + x] = (VideoMemory[80*y + x] & 0xFF00) |' ';
                }
            }
            x = 0,y = 0;
        }

    }
}

// handler->OnKeyDown 重载
void printfHex(uint8_t key){
    char* foo = (char*)"00";
    const char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0x0f];
    foo[1] = hex[key & 0x0f];
    printf((const char *)foo);
}


class PrintKeyboardEventHandler :public KeyboardEventHandler{
public:
    void OnKeyDown(char c)
    {
        char* foo = (char*)" ";
        foo[0] = c;
        printf(foo);
    }

};

class MouseToConsole :public MouseEventHandler{
public:
    MouseToConsole() : x(40),y(12){

    }

    void OnActivate(){
        // 8048 芯片端口
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[y*80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4) | ((VideoMemory[y*80 + x] & 0x0f00) << 4) | ((VideoMemory[y*80 + x] & 0x00ff));
    }

    void OnMouseMove(int8_t nx,int8_t ny) override {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        // 低位和 高位交换 就自动变色
        VideoMemory[y*80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4) | ((VideoMemory[y*80 + x] & 0x0f00) << 4) | ((VideoMemory[y*80 + x] & 0x00ff));
        x += nx;
        if (x  < 0) x = 0;
        else if(x >= 80) x = 79;

        y += ny;
        if(y < 0 ) y = 0;
        else if(y >= 25) y = 24;
        // 移动光标
        VideoMemory[y*80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4) | ((VideoMemory[y*80 + x] & 0x0f00) << 4) | ((VideoMemory[y*80 + x] & 0x00ff)); 

    }

private:
    int8_t x,y;

};


// 定义指针
typedef void (*constructor)();
// 构造
extern "C" constructor start_ctors;

// 析构
extern "C" constructor end_ctors;

// 调用遍历函数
extern "C" void callConstructor(){
    for(constructor* i = &start_ctors;i !=&end_ctors;i++)
    {
        (*i)();
    }
}

void taskA()
{
    while(true)
    {
        printf("A");
    }
}

void taskB()
{
    while(true)
    {
        printf("B");
    }
}

class PrintfUDPHandler : public UserDatagramProtocolHandler{
public:
    void HandlerUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket, uint8_t* data,uint16_t size){
        char* foo = "";
        for(int i = 0;i<size;i++)
        {
            foo[0] = data[i];
            printf(foo);
        }
    }


};


// 声明可找到的
extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber)
{
    printf("hello world\n");
    printf("cc");

    GlobalDescriptorTable gdt;

    size_t heap = 10*1024*1024; // 10MB
    uint32_t* memupper = (uint32_t*)((size_t)multiboot_structure + 8 );
    printf("heap: 0x");
    printfHex((*memupper >> 24) & 0xff );
    printfHex((*memupper >> 16) & 0xff );
    printfHex((*memupper >> 8) & 0xff );
    printfHex((*memupper >> 0) & 0xff );

    MemoryManager memoryManger(heap,(*memupper)*1024 - heap - 10 * 1024);

    printf("\nheap: 0x");
    printfHex((heap >> 24) & 0xff );
    printfHex((heap >> 16) & 0xff );
    printfHex((heap >> 8) & 0xff );
    printfHex((heap >> 0) & 0xff );

    void* allocated = memoryManger.malloc(1024);
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xff );
    printfHex(((size_t)allocated >> 16) & 0xff );
    printfHex(((size_t)allocated >> 8) & 0xff );
    printfHex(((size_t)allocated >> 0) & 0xff );

    TaskManger taskmanger;
    // Task task1(&gdt,taskA);
    // Task task2(&gdt,taskB);
    // taskmanger.AddTask(&task1);
    // taskmanger.AddTask(&task2);

    InterruptManager interrupts(0x20,&gdt,&taskmanger);

// #define GRAPHICMODE
#ifdef GRAPHICMODE
    Desktop desktop(320,200,0x00,0x00,0xa8);
#endif
    DriverManager drvManager;

#ifdef GRAPHICMODE    
    KeyBoardDriver keyboard(&interrupts ,&desktop);
#else
    PrintKeyboardEventHandler kbhandler;
    KeyBoardDriver keyboard(&interrupts ,&kbhandler);
#endif
    // PrintKeyboardEventHandler kbhandler;
    // KeyBoardDriver keyboard(&interrupts ,&kbhandler);
    drvManager.AddDriver(&keyboard);

#ifdef GRAPHICMODE  
    MouseDriver mouse(&interrupts,&desktop);
#else
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts,&mousehandler);
#endif
    drvManager.AddDriver(&mouse);

    PeripheralComponentInterconnectControllor PCIController;
    PCIController.SelectDrivers(&drvManager,&interrupts);

    VideosGraphicsArray vga;


    drvManager.ActivateAll();

#ifdef GRAPHICMODE  
    vga.SetMode(320,200,8);
    Window w1(&desktop,10,10,20,20,0xa8,0x00,0x00);
    desktop.AddChild(&w1);

    Window w2(&desktop,40,15,30,30,0x00,0xa8,0x00);
    desktop.AddChild(&w2);
#endif
    uint8_t ip1 = 10, ip2 = 22, ip3 = 112, ip4 = 71;
    uint32_t ip_be = ( ((uint32_t)ip4<<24) | ( (uint32_t)ip3<<16 ) | ((uint32_t)ip2<<8)) | (uint32_t)ip1  ;

    uint8_t gip1 = 10, gip2 = 22, gip3 = 0, gip4 = 1;
    uint32_t gip_be = ( ((uint32_t)gip4<<24) | ( (uint32_t)gip3<<16 ) | ((uint32_t)gip2<<8)) | (uint32_t)gip1  ;

    amd_973* eth0 = (amd_973*)(drvManager.drivers[2]);

    eth0->SetIPAddres(ip_be);
    EtherFrameProvider etherframe(eth0);

    AddressResolutionProtocal arp(&etherframe);

    uint8_t subnet1 = 255, subnet2 = 255, subnet3 = 0, subnet4 = 0;
    uint32_t subnet_be = ( ((uint32_t)subnet4<<24) | ( (uint32_t)subnet3<<16 ) | ((uint32_t)subnet2<<8)) | (uint32_t)subnet1  ;

    InternetProtocolProvider ipv4(&etherframe, &arp, gip_be,subnet_be);
    InternetControlMessageProtocol icmp(&ipv4);
    UserDatagramProtocolProvider udp(&ipv4);

    // etherframe.Send(0xffffffffffff,0x608,(uint8_t*)"hello network",13);
    // eth0->Send((uint8_t*)"hello network",13);

    interrupts.Activate();

    printf("\n\n");
    // arp.Resolve(gip_be);
    // ipv4.Send(gip_be,0x0008,(uint8_t*)"hello Network",13);
    arp.BroadcastMACAddress(gip_be);
    icmp.RequestEchoReply(gip_be);

    PrintfUDPHandler udphandler;
    UserDatagramProtocolSocket* socket = udp.Listen(1234);
    udp.Bind(socket,&udphandler);

    while(1){
#ifdef GRAPHICMODE 
    desktop.Draw(&vga);
#endif

    }

}


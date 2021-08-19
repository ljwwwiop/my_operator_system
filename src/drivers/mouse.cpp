#include "drivers/mouse.h"

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(const char*);

MouseEventHandler::MouseEventHandler()
{

}

void MouseEventHandler::OnActivate(){

}

void MouseEventHandler::OnMouseDown(uint8_t button){

}

void MouseEventHandler::OnMouseUp(uint8_t button){

}

void MouseEventHandler::OnMouseMove(int8_t x,int8_t y){

}

// void MouseEventHandler::onMouseMove(int32_t x,int32_t y){

// }


MouseDriver::MouseDriver(InterruptManager* manager,MouseEventHandler* handler)
    :InterruptHandler(0x0C + manager->HardwareInterruptOffset(),manager),dataport(0x60),commandport(0x64),offset(0),button(0),handler(handler)
{   

}

MouseDriver::~MouseDriver(){}

void MouseDriver::Activate()
{
    if(handler != nullptr) 
        handler->OnActivate();

    commandport.Write(0xa8);
    commandport.Write(0x20);

    // 读取 8bit
    uint8_t status = (dataport.Read() | 2) & ~0x20; // | 1 是开启键盘中断，把键盘键盘开启 ，取反获取到第四位
    commandport.Write(0x60); // 告诉端口 准备写入
    dataport.Write(status);

    commandport.Write(0xd4); // 0xd4 鼠标驱动
    dataport.Write(0xf4);
    dataport.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    // 读取状态是否为鼠标 0x20
    if(!(status & 0x20) || handler == nullptr) return esp;

    buffer[offset] = dataport.Read();
    offset = (offset + 1)%3;

    if(offset == 0){
        handler->OnMouseMove(buffer[1],-buffer[2]);

        for(uint8_t i = 0;i<3 ;i++)
        {
            if( (buffer[0] & (1 <<i)) != (button & (1 <<i)) ){
                //VideoMemory[y*80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4) | ((VideoMemory[y*80 + x] & 0x0f00) << 4) | ((VideoMemory[y*80 + x] & 0x00ff)); 
                if( button & (1<<i)){
                    handler->OnMouseUp(i+1);
                }else{
                    handler->OnMouseDown(i+1);
                }

            }
        }
        button = buffer[0];

    }



    return esp;
}


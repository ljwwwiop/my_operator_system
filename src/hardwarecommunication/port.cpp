#include "hardwarecommunication/port.h"

using namespace myos::common;
using namespace myos::hardwarecommunication;
/*
    修改内联汇编 -> inline 使用 多个简单的小函数 减少栈的调用空间问题
*/




Port::Port(uint16_t portnumber):portnumber(portnumber)
{}

Port::~Port(){}

Port8Bit::Port8Bit(uint16_t portnumber)
    : Port(portnumber){}

Port8Bit::~Port8Bit(){}

void Port8Bit::Write(uint8_t data){
    // 汇编语句 inb 指令，从IO端口写入一个字节
    Write8(portnumber,data);
}

uint8_t Port8Bit::Read(){
    return Read8(portnumber);
}


// 16
Port16Bit::Port16Bit(uint16_t portnumber)
    : Port(portnumber){}

Port16Bit::~Port16Bit(){}

void Port16Bit::Write(uint16_t data){
    // 汇编语句 inb 指令，从IO端口写入一个字节
    Write16(portnumber,data);
}

uint16_t Port16Bit::Read(){
    return Read16(portnumber);
}


//32
Port32Bit::Port32Bit(uint16_t portnumber)
    : Port(portnumber){}

Port32Bit::~Port32Bit(){}

void Port32Bit::Write(uint32_t data){
    // 汇编语句 inb 指令，从IO端口写入一个字节
    // __asm__ volatile("outl %0,%1" : : "a" (data) , "Nd" (portnumber));
    Write32(portnumber,data);

}

uint32_t Port32Bit::Read(){
    return Read32(portnumber);
}

//slow
Port8BitSlow::Port8BitSlow(uint16_t portnumber)
    : Port8Bit(portnumber){}

Port8BitSlow::~Port8BitSlow(){}

void Port8BitSlow::Write(uint8_t data){
    // 汇编语句 inb 指令，从IO端口写入一个字节
    WriteSlow(portnumber,data);
}




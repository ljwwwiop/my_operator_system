#include "gdt.h"

using namespace myos;
using namespace myos::common;
// using namespace myos::hardwarecommunication;


// 0x9a 0x92 分别表示type中的段类型的种类
GlobalDescriptorTable::GlobalDescriptorTable():nullSegmentDescriptor(0,0,0),unusedSegmentDescriptor(0,0,0),
    codeSegmentDescriptor(0,64*1024*1024,0x9a),dataSegmentDescriptor(0,64*1024*1024,0x92)
{
    uint32_t i[2];
    i[1] = (uint32_t)this;
    i[0] = sizeof(GlobalDescriptorTable) <<16 ; // 偏移两个字节
    // 汇编部分 valatile 不拆分命令 %0 占位符 : 分割符，p 是操作加载符
    asm volatile("lgdt (%0)": :"p" (((uint8_t *)i) + 2));
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{

}

uint16_t GlobalDescriptorTable::DataSegmentSelector(){

    return ((uint8_t*)&dataSegmentDescriptor - (uint8_t*)this) >> 3;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector(){
    return ((uint8_t*)&codeSegmentDescriptor - (uint8_t*)this) >> 3;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type){
    uint8_t* target = (uint8_t*)this;
    if(limit < 1048576){ //  这个 < 地方有思考 1048576
        target[6] = 0x40;
    }else{
        if((limit & 0xfff)!=0xfff){
            limit = (limit >> 12) - 1;
        }else{
            limit = limit >> 12;
        }
        target[6] = 0xC0;
    } 

    target[0] = limit & 0xff;
    target[1] = (limit >> 8)&0xff;
    target[6] |= (limit >> 16) & 0xf; // 取出低4位 bite

    target[2] = base & 0xff;
    target[3] = (base >> 8)& 0xff;
    target[4] = (base >> 16) & 0xff;
    target[7] = (base >> 24) & 0xff;

    target[5] = type;

}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[7];
    result = (result <<8) + target[4];
    result = (result << 8) + target[3];
    result = (result <<8) + target[2];
    return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    // limit 20 位 两个半字节
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[6] & 0xf;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xfff;
    return result;
}



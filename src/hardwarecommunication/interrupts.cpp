#include "hardwarecommunication/interrupts.h"

using namespace myos::common;
using namespace myos::hardwarecommunication;

void printf(const char*);
void printfHex(uint8_t);

InterruptHandler::InterruptHandler(uint8_t interruptNumber,InterruptManager* interruptManager)
{
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;

}

InterruptHandler::~InterruptHandler()
{
    if(interruptManager->handlers[interruptNumber] == this){
        interruptManager->handlers[interruptNumber] = 0;
    }
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActivateInterrupManager = 0;


void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t DescriptorPrivilegelLevel,
    uint8_t DescriptorType
){
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler)&0xffff; // 取2个字节
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = ((uint32_t)handler>>16)& 0xffff;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    // 设置权限
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | ((DescriptorPrivilegelLevel & 3 ) << 5) | DescriptorType;
    interruptDescriptorTable[interruptNumber].reserved = 0;
    
}

InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset,GlobalDescriptorTable* gdt,TaskManger* taskManger)
    :picMasterCommand(0x20),picMasterData(0x21),picSlaveCommand(0xA0),picSlaveData(0xA1)
{   
    this->taskManger = taskManger;
    this->hardwareInterruptOffset = hardwareInterruptOffset;
    uint16_t codeSegment = (gdt->CodeSegmentSelector()) << 3;

    const uint8_t IDT_INTERRUPT_GATE = 0xe;
    for(uint16_t i = 0;i<256;i++){
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i,codeSegment,&InterruptIgnore,0,IDT_INTERRUPT_GATE);
    }


    SetInterruptDescriptorTableEntry(0x00,codeSegment,&HandleException0x00,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x01,codeSegment,&HandleException0x01,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x02,codeSegment,&HandleException0x02,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x03,codeSegment,&HandleException0x03,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x04,codeSegment,&HandleException0x04,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x05,codeSegment,&HandleException0x05,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x06,codeSegment,&HandleException0x06,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x07,codeSegment,&HandleException0x07,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x08,codeSegment,&HandleException0x08,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x09,codeSegment,&HandleException0x09,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0A,codeSegment,&HandleException0x0A,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0B,codeSegment,&HandleException0x0B,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0C,codeSegment,&HandleException0x0C,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0D,codeSegment,&HandleException0x0D,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0E,codeSegment,&HandleException0x0E,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0F,codeSegment,&HandleException0x0F,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x10,codeSegment,&HandleException0x10,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x11,codeSegment,&HandleException0x11,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x12,codeSegment,&HandleException0x12,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x13,codeSegment,&HandleException0x13,0,IDT_INTERRUPT_GATE);

    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00,codeSegment,&HandleInterruptRequest0x00,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01,codeSegment,&HandleInterruptRequest0x01,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x02,codeSegment,&HandleInterruptRequest0x02,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x03,codeSegment,&HandleInterruptRequest0x03,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x04,codeSegment,&HandleInterruptRequest0x04,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x05,codeSegment,&HandleInterruptRequest0x05,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x06,codeSegment,&HandleInterruptRequest0x06,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x07,codeSegment,&HandleInterruptRequest0x07,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x08,codeSegment,&HandleInterruptRequest0x08,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x09,codeSegment,&HandleInterruptRequest0x09,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A,codeSegment,&HandleInterruptRequest0x0A,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B,codeSegment,&HandleInterruptRequest0x0B,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C,codeSegment,&HandleInterruptRequest0x0C,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D,codeSegment,&HandleInterruptRequest0x0D,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E,codeSegment,&HandleInterruptRequest0x0E,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F,codeSegment,&HandleInterruptRequest0x0F,0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x31,codeSegment,&HandleInterruptRequest0x31,0,IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(hardwareInterruptOffset);
    picSlaveData.Write(hardwareInterruptOffset + 8);

    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    // 设置中断
    // 初始化
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;

    // 汇编传入进去
    asm volatile("lidt %0": :"m" (idt));

}

InterruptManager::~InterruptManager()
{

}

uint16_t InterruptManager::HardwareInterruptOffset()
{
    return hardwareInterruptOffset;
}


void InterruptManager::Activate()
{
    if(ActivateInterrupManager != 0)
    {
        ActivateInterrupManager->Deactivate();
    }
    ActivateInterrupManager = this;
    // sti 开启中断
    asm("sti");
}

void InterruptManager::Deactivate()
{
    if(ActivateInterrupManager == this)
    {
        ActivateInterrupManager = 0;
        asm("cli"); // 关闭中断
    }
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber,uint32_t esp)
{
    // handler->OnKeyDown("interrupt");
    if(ActivateInterrupManager !=0 ){
        return ActivateInterrupManager->DoHandleInterrupt(interruptNumber,esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber,uint32_t esp)
{
    if(handlers[interruptNumber] !=0){
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    }else if(interruptNumber != hardwareInterruptOffset ){
        printf("UNHANDLE INTERRUPT 0x");
        printfHex(interruptNumber);
    }

    if(interruptNumber == hardwareInterruptOffset){
        esp = (uint32_t)taskManger->Schedule((CPUState*)esp);
    }

    if(hardwareInterruptOffset <= interruptNumber && interruptNumber < hardwareInterruptOffset + 16){
        // 运行中断中
        picMasterCommand.Write(0x20);
        if(hardwareInterruptOffset + 8 <= interruptNumber){
            picSlaveCommand.Write(0x20);
        }
    }

    return esp;
}



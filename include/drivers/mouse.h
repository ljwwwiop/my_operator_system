#ifndef __MYOS__DRIVERS__MOUSE_H
#define __MYOS__DRIVERS__MOUSE_H

#include "common/types.h"
#include "hardwarecommunication/port.h"
#include "hardwarecommunication/interrupts.h"
#include "drivers/driver.h"

namespace myos
{
    namespace drivers{
        class MouseEventHandler{
        public:
            MouseEventHandler();

            virtual void OnActivate();
            virtual void OnMouseDown(myos::common::uint8_t button);
            virtual void OnMouseUp(myos::common::uint8_t button);
            virtual void OnMouseMove(myos::common::int8_t x,myos::common::int8_t y);
            // virtual void onMouseMove(myos::common::int32_t x,myos::common::int32_t y);
        };

        class MouseDriver : public myos::hardwarecommunication::InterruptHandler,public Driver{
        public:
            MouseDriver(myos::hardwarecommunication::InterruptManager* manager,MouseEventHandler* handler);
            ~MouseDriver();
            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
            virtual void Activate();

        private:
            myos::hardwarecommunication::Port8Bit dataport;
            myos::hardwarecommunication::Port8Bit commandport;

            myos::common::uint8_t buffer[3]; // buffer
            myos::common::uint8_t offset; // 中断
            myos::common::uint8_t button; // 按钮状态

            myos::common::int8_t x,y; // 这里会直接向上穿过屏幕，因为uint8 没有负值，直接到了255
            MouseEventHandler* handler;
        };

    }
} // namespace myos

    





#endif

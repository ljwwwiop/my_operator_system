#ifndef __MYOS__HARDWARECOMMUNICATION__VGA_H
#define __MYOS__HARDWARECOMMUNICATION__VGA_H

#include "common/types.h"
#include "hardwarecommunication/port.h"
#include "drivers/driver.h"

namespace myos
{
    namespace drivers{
        class VideosGraphicsArray{
            public:
                VideosGraphicsArray();
                ~VideosGraphicsArray();

                bool SupportsModel(common::uint32_t width, common::uint32_t height,common::uint32_t colordepth);
                bool SetMode(common::uint32_t width, common::uint32_t height,common::uint32_t colordepth);
                void PutPixel(common::int32_t x,common::int32_t y,common::uint8_t r,common::uint8_t g,common::uint8_t b);
                void PutPixel(common::int32_t x,common::int32_t y,common::uint8_t colorindex);

                void FillRectangle(common::uint32_t x ,common::uint32_t y,common::uint32_t w,common::uint32_t h,common::uint8_t r,common::uint8_t g,common::uint8_t b);

            private:
                hardwarecommunication::Port8Bit miscPort;
                hardwarecommunication::Port8Bit crctcIndexPort;
                hardwarecommunication::Port8Bit crtcDataPort;
                hardwarecommunication::Port8Bit sequencerIndexPort;
                hardwarecommunication::Port8Bit sequencerDataPort;
                hardwarecommunication::Port8Bit graphicsControllerIndexPort;
                hardwarecommunication::Port8Bit graphicsControllerDataPort;
                hardwarecommunication::Port8Bit attributeControllerIndexPort;
                hardwarecommunication::Port8Bit attributeControllerReadPort;
                hardwarecommunication::Port8Bit attributeControllerWritePort;
                hardwarecommunication::Port8Bit attributeControllerResetPort;

                void WriteRegisters(common::uint8_t* registers);
                common::uint8_t GetColorIndex(common::uint8_t r,common::uint8_t g,common::uint8_t b);
                common::uint8_t* GetFrameBufferSegement();
        };
    }
}





#endif


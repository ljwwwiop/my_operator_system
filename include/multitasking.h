#ifndef __MULTITASKING_H
#define __MULTITASKING_H

#include "common/types.h"
#include "gdt.h"

namespace myos{
    struct CPUState{
        common::uint32_t eax, ebx, ecx,edx,esi,edi,ebp;

        common::uint32_t error,eip,cs,eflags,esp,ss;

    }__attribute__((packed));

    class Task{
        friend class TaskManger;  // 这样taskmanger 可以访问私有成员
    public:
        Task(GlobalDescriptorTable* gdt, void entrypoint());
        ~Task();
    private:
        common::uint8_t stack[4096];
        CPUState* cpustate;

    };
    

    class TaskManger{
    public:
        TaskManger();
        ~TaskManger();
        bool AddTask(Task* task);
        CPUState* Schedule(CPUState* cpustate);
    private:
        Task* tasks[256];
        int numTasks;
        int currentTask;

    };


}




#endif
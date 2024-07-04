/*
 * kernel.h
 *
 *  Created on: Jun 13, 2024
 *      Author: owenm
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

#include<stdbool.h>
#include<stdint.h>
void SVC_Handler_Main( unsigned int *svc_args );
extern uint32_t* addThreadToStack();
extern bool osCreateThread(void* funcPointer);
extern void osKernelInitialize();
extern void osKernelStart();

#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV

typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
};



#endif /* INC_KERNEL_H_ */

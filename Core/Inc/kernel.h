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

typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
}thread;



#endif /* INC_KERNEL_H_ */

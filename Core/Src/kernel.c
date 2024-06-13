/*
 * kernel.c
 *
 *  Created on: Jun 13, 2024
 *      Author: owenm
 */
#include "main.h"
#include<stdio.h>
#include<stdbool.h>
#include "kernel.h"

uint32_t* stackptr;
thread currentThread;
uint32_t* lastAllocatedStackPtr;
uint32_t* MSP_INIT_VAL;
void SVC_Handler_Main( unsigned int *svc_args )
{
	unsigned int svc_number;
	/*
	* Stack contains:
	* r0, r1, r2, r3, r12, r14, the return address and xPSR
	* First argument (r0) is svc_args[0]
	*/
	svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
	switch( svc_number )
	{
		case 10:
			__set_PSP(stackptr);
			runFirstThread();
			break;
		case 17: //17 is sort of arbitrarily chosen
			printf("Success!\r\n");
			break;
		case 1:
			printf("Failure!\r\n");
			break;
		case 2:
			printf("Unknown\r\n");
		default: /* unknown SVC */
			break;
	}
}


uint32_t* addThreadToStack(){
	if (lastAllocatedStackPtr < MSP_INIT_VAL - (0x4000 - 0x0400)){
		return NULL;
	}

	lastAllocatedStackPtr -= 0x0400;
	return lastAllocatedStackPtr;
}

void osKernelInitialize(){
	MSP_INIT_VAL = *(uint32_t**)0x0;
	printf("MSP Init is: %p\n\r",MSP_INIT_VAL); //note the %p to print a pointer. It will be in hex
	lastAllocatedStackPtr = MSP_INIT_VAL;
	printf("dhfgsk");
}

bool osCreateThread(void *funcPointer){
	stackptr = addThreadToStack();
	if (stackptr == NULL){
		return false;
	}
	*(--stackptr) = 1<<24; //A magic number, this is xPSR
	*(--stackptr) = (uint32_t)funcPointer; //the function name
	for (int i = 0; i < 14; i++){
		*(--stackptr) = 0xA; //An arbitrary number
	}

	currentThread.sp = stackptr;
	currentThread.thread_function = funcPointer;

	return true;
}

void osKernelStart(){
	__asm("SVC #10");
}

#include "main.h"
#include<stdio.h>
#include<stdbool.h>
#include "kernel.h"

uint32_t* stackptr;
struct k_thread threadArray[100];
uint32_t currentThread;
uint32_t numThreadsRunning;
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
		case 20:
			//Pend an interrupt to do the context switch
			_ICSR |= 1<<28;
			__asm("isb");
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
	//set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV

	MSP_INIT_VAL = *(uint32_t**)0x0;
	lastAllocatedStackPtr = MSP_INIT_VAL;

	numThreadsRunning = 0;
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

	currentThread = numThreadsRunning;
	numThreadsRunning += 1;
	threadArray[currentThread].sp = stackptr;
	threadArray[currentThread].thread_function = funcPointer;

	return true;
}

void osKernelStart(){
	__asm("SVC #10");
}

void osYield(){
	__asm("SVC #20");
}

void osSched(){
	threadArray[currentThread].sp = (uint32_t*)(__get_PSP() - 8*4);
	currentThread = (currentThread+1)%numThreadsRunning;
	__set_PSP(threadArray[currentThread].sp);
}

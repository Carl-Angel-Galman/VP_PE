/******************************************************************************
 * @file startup.s
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Startup Code for VPTemplate Project
 *
 *
 *****************************************************************************/

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

/**
 * @brief  This is the code that gets called when the processor first
 * starts execution following a reset event. Only the absolutely
 * necessary set is performed, after which the application
 * supplied main() routine is called.
 *
 * @param  None
 *
 * @retval : None
 */
  .align 4
  MARKER:
 	.word 0xDEC0ADDE 
 	 /*DEADC0DE written in Little Endian  */
 	 
ENDMARKER:
	.word 0xEA1DADAB
	/*ABAD1DEA written in Little Endian*/
 	
 
.section .text.Start_Handler
.type Start_Handler, %function
.global Start_Handler
Start_Handler:
    /* Copy the data segment initializers from flash to SRAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sloaddata
    movs r3, #0
    b .loopCopyData

.copyData:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

.loopCopyData:
    adds r4, r0, r3
    cmp r4, r1
    bcc .copyData

    /* Zero fill the bss segment. */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b .loopFillZerobss

.fillZerobss:
    str  r3, [r2]
    adds r2, r2, #4

.loopFillZerobss:
    cmp r2, r4
    bcc .fillZerobss
     
    ldr r2, =_sstack
    ldr r4, =_estack
    
    /*Overwrite the first address of the stack with end marker*/
    ldr r3, =ENDMARKER
    ldr r3, [r3]
    str r3, [r2]
    adds r2, r2, #4
    
    /*Loads the Marker in the right register*/
    ldr r3, = MARKER
    ldr r3, [r3]
    
    b .loopFillStack
    
/*Fill the current stack pointer with the MARKER*/
.fillStack:
    str  r3, [r2]
    adds r2, r2, #4

.loopFillStack:
	/* Condition to check if the current stack pointer has reached the end of the stack */
    cmp r2, r4
    bcc .fillStack
    
    /* Initialize the Stack-Pointer */
   	ldr r0, =_initial_stack_pointer
    
    /* Set stack pointer */
    mov   sp, r0

    /* Call the clock system intitialization function.*/
    bl  SystemInit

    /* Call the application's entry point.*/
    bl main
    bx lr
.size Start_Handler, .-Start_Handler


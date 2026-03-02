/*
 * EventQueue.h
 *
 *  Created on: Feb 27, 2026
 *      Author: kali
 */

#ifndef SRC_UTIL_EVENTQUEUE_EVENTQUEUE_H_
#define SRC_UTIL_EVENTQUEUE_EVENTQUEUE_H_


// EventQueue.h
#include <stdint.h>
#include <stdbool.h>

typedef int32_t Event_t;

#define EVENTQ_CAPACITY  32   // must be >= 2

typedef struct
{
    volatile uint16_t head;   // write index
    volatile uint16_t tail;   // read index
    Event_t buf[EVENTQ_CAPACITY];
} EventQueue_t;

void  EventQ_Init(EventQueue_t *q);
bool  EventQ_Push(EventQueue_t *q, Event_t e);   // false = full
bool  EventQ_Pop (EventQueue_t *q, Event_t *e);  // false = empty
bool  EventQ_IsEmpty(const EventQueue_t *q);
bool  EventQ_IsFull (const EventQueue_t *q);
uint16_t EventQ_Count(const EventQueue_t *q);


#endif /* SRC_UTIL_EVENTQUEUE_EVENTQUEUE_H_ */

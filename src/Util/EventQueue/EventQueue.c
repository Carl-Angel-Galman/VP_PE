/*
 * EventQueue.c
 *
 *  Created on: Feb 27, 2026
 *      Author: kali
 */




// EventQueue.c
#include "EventQueue.h"

#include "Global.h"

static inline uint16_t nextIndex(uint16_t i)
{
    return (uint16_t)((i + 1u) % EVENTQ_CAPACITY);
}

void EventQ_Init(EventQueue_t *q)
{
    q->head = 0;
    q->tail = 0;
}

bool EventQ_IsEmpty(const EventQueue_t *q)
{
    return (q->head == q->tail);
}

bool EventQ_IsFull(const EventQueue_t *q)
{
    return (nextIndex(q->head) == q->tail);
}

uint16_t EventQ_Count(const EventQueue_t *q)
{
    // Works for ring buffers with mod arithmetic
    if (q->head >= q->tail) return (uint16_t)(q->head - q->tail);
    return (uint16_t)(EVENTQ_CAPACITY - (q->tail - q->head));
}

bool EventQ_Push(EventQueue_t *q, Event_t e)
{
    uint16_t h = q->head;
    uint16_t n = nextIndex(h);

    if (n == q->tail)
    {
        // Queue full
        return false;
    }

    q->buf[h] = e;
    q->head = n;
    return true;
}

bool EventQ_Pop(EventQueue_t *q, Event_t *e)
{
    uint16_t t = q->tail;

    if (t == q->head)
    {
        // Queue empty
        return false;
    }

    *e = q->buf[t];
    q->tail = nextIndex(t);
    return true;
}

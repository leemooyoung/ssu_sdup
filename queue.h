#ifndef _SDUP_QUEUE_H
#define _SDUP_QUEUE_H

#define QUEUE_SIZE (16)

/* 큐를 없애려면 arr 배열을 free 해야 함
 * 환형 큐
 * start는 가장 앞인 원소의 배열에서의 위치
 * end는 가장 마지막인 원소의 배열에서의 위치 + 1
 */

typedef struct {
	void **arr;
	int size;
	int start;
	int end;
} QUEUE;

// 큐 초기화
QUEUE *queue_init(QUEUE *q);

// 삽입
int enqueue(QUEUE *q, void *val);

// 제거
void *dequeue(QUEUE *q);

#endif

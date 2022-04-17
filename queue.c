#include <stdlib.h>
#include <string.h>
#include "queue.h"

QUEUE * queue_init(QUEUE *q) {
	if(q == NULL)
		q = (QUEUE*)malloc(sizeof(QUEUE));
	
	if(q == NULL)
		return NULL;

	q->size = QUEUE_SIZE;
	q->arr = malloc(sizeof(void*) * q->size);
	q->start = 0;
	q->end = 0;

	return q;
}

int enqueue(QUEUE *q, void *val) {
	void **tmp;
	int count;

	if(q->start == (q->end + 1) % q->size) {
		// 원소를 하나 더 추가하면 큐가 꽉 차는 경우
		// 큐를 확장한다

		tmp = malloc(sizeof(void*) * (q->size * 2));
		if(tmp == NULL)
			return -1;

		if(q->end < q->start) {
			// 한바퀴 돌아서 end가 start보다 앞에 있을 경우
			memcpy(tmp, q->arr + q->start, sizeof(void*) * (q->size - q->start));
			memcpy(tmp + q->size - q->start, q->arr, sizeof(void*) * q->end);
			count = q->size - q->start + q->end;
		} else {
			// 한바퀴 돌지 않은 경우
			memcpy(tmp, q->arr + q->start, sizeof(void*) * (q->end - q->start));
			count = q->end - q->start;
		}

		free(q->arr);
		
		q->arr = tmp;
		q->size *= 2;
		q->start = 0;
		q->end = count;
	}
	
	q->arr[q->end] = val;
	q->end = (q->end + 1) % q->size;

	return 0;
}

void *dequeue(QUEUE *q) {
	void *res;

	if(q->start == q->end)
		return NULL;

	res = q->arr[q->start];
	q->start = (q->start + 1) % q->size;

	return res;
}

int left_in_queue(QUEUE *q) {
	if(q->start == q->end)
		return 0;
	else if(q->start < q->end)
		return q->end - q->start;
	else
		return q->size - q->start + q->end;
}

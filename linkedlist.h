#ifndef _SDUP_LINKEDLIST_H
#define _SDUP_LINKEDLIST_H

/* 아무 데이터도 갖고 있지 않은 head node만 있는 상태가 비어있는 상태인 linked list
 */

typedef struct {
	LNKLIST *prev;
	LNKLIST *next;
	void *val;
} LNKLIST;

// linkedlist 생성
LNKLIST *lnklist_init();

// linkedlist에서 주어진 node 다음에 val를 가진 node를 삽입
int lnklist_insert_after(LNKLIST *node, void *val);

// linkedlist에서 주어진 node를 삭제
int lnklist_delete(LNKLIST *node);

// linkedlist를 삭제
int lnklist_destroy(LNKLIST *head);

#endif

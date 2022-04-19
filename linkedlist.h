#ifndef _SDUP_LINKEDLIST_H
#define _SDUP_LINKEDLIST_H

/* 아무 데이터도 갖고 있지 않은 head node만 있는 상태가 비어있는 상태인 linked list
 */

typedef struct linkedlist {
	struct linkedlist *prev;
	struct linkedlist *next;
	void *val;
} LNKLIST;

// linkedlist 생성
LNKLIST *lnklist_init();

// linkedlist에서 주어진 node 다음에 val를 가진 node를 삽입
int lnklist_insert_after(LNKLIST *node, void *val);

// linkedlist에서 주어진 node를 삭제. 삭제한 node의 val을 반환
void *lnklist_delete(LNKLIST *node);

// linkedlist를 삭제
// destroy_data가 NULL이면 val에 대해 free를 호출
int lnklist_destroy(LNKLIST *head, void (*destroy_data)(void *));

// n 번째 원소를 반환한다. 없으면 NULL 반환
// 1번부터 시작. 0이 입력되면 NULL 반환
LNKLIST *lnklist_find_n(LNKLIST *head, int n);

#endif

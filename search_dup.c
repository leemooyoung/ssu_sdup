// 중복 파일 탐색 및 중복 파일 리스트 출력

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include "linkedlist.h"
#include "queue.h"
#include "search_dup.h"
#include "inputcntl.h"

extern char *trash_path;

void free_filehash(void *fh) {
	if(fh == NULL)
		return;

	if(((FILEHASH*)fh)->hash != NULL)
		free(((FILEHASH*)fh)->hash);
	
	if(((FILEHASH*)fh)->pathname != NULL)
		free(((FILEHASH*)fh)->pathname);
	
	free(fh);
}

// linkedlist 안의 FILEHASH를 가지고 있는 linkedlist를 지우기 위한 함수
void lnklist_dest_fh(void *head) {
	lnklist_destroy((LNKLIST*)head, free_filehash);
}

// 크기 오름차순, path 사전순으로 정렬하면서 삽입한다
int insert_filehash(LNKLIST *head, FILEHASH *fh) {
	LNKLIST *cur_1 = NULL;
	LNKLIST *cur_2 = NULL;
	LNKLIST *head_2 = NULL;
	LNKLIST *insert_before_1 = NULL;
	FILEHASH *fhtmp = NULL;
	int matched = 0;

	cur_1 = head->next;
	while(head != cur_1) {
		cur_2 = (LNKLIST*)(cur_1->val);
		fhtmp = (FILEHASH*)(cur_2->next->val);

		if(fh->size < fhtmp->size && insert_before_1 == NULL) {
			// 리스트의 원소와 size를 비교했을 때 처음으로 작다면
			insert_before_1 = cur_1;
		}

		if(fhtmp->size == fh->size && strcmp(fhtmp->hash, fh->hash) == 0) {
			// size와 hash가 일치하면
			matched = 1;
			break;
		}
		
		cur_1 = cur_1->next;
	}

	// 비교했을 때 처음으로 fh 쪽이 작은 원소를 마주치지 못했다면 head의 이전(가장 뒤) 에 삽입
	if(insert_before_1 == NULL)
		insert_before_1 = head;

	if(matched) {
		/* 중복된 파일을 찾았다면
		 * 1. path의 depth 오름차순
		 * 2. pathname 사전 오름차순
		 * 으로 정렬해야 한다.
		 * search_dup에서 BFS로 탐색하기 때문에 계속 뒤에 삽입하게 되면 자연스레 depth 오름차순으로 정렬되게 된다.
		 * 중요한건 같은 depth인 것들 사이에서 pathname 사전 오름차순으로 정렬하는 것이다.
		 */
		head_2 = cur_2;
		cur_2 = head_2->prev;
		fhtmp = (FILEHASH*)(cur_2->val);
		if(fhtmp->depth < fh->depth) {
			// 삽입하려는 파일이 더 깊이 있으면 가장 뒤에 삽입
			cur_2 = head_2->prev;
		} else {
			// 삽입하려는 파일과 깊이가 같으면 (얕을 수는 없다) 같은 깊이인 파일들 안에서 사전순으로 정렬
			while(cur_2 != head_2) {
				fhtmp = (FILEHASH*)(cur_2->val);
				if(fhtmp->depth < fh->depth)
					// 같은 깊이인 파일들의 범위를 벗어남
					break;

				if(strcmp(fhtmp->pathname, fh->pathname) < 0)
					// 처음으로 사전순으로 앞서는 것의 뒤에 삽입하기 위해서
					break;

				cur_2 = cur_2->prev;
			}
		}

		lnklist_insert_after(cur_2, fh);
	} else {
		// 중복된 파일을 찾지 못했다면 size 오름차순으로 정렬되도록
		// 이전에 찾은 insert_before_1의 이전에 새로운 링크드 리스트를 삽입
		head_2 = lnklist_init();
		lnklist_insert_after(head_2, fh);
		lnklist_insert_after(insert_before_1->prev, head_2);
	}

	return 0;
}

// 이중 링크드 리스트를 사용한다
LNKLIST *search_dup(char *path, off_t llimit, off_t ulimit, char *fextension, char *(*hashstrfunc)(int)) {
	QUEUE q;
	LNKLIST *head;
	LNKLIST *cur;
	LNKLIST *head2;
	LNKLIST *tmp;
	struct stat statbuf;
	char *rpath;
	DIR *dirp;
	struct dirent *dentry;
	int path_len;
	int filename_len;
	int fext_len;
	int fd;
	FILEHASH *fh;
	QENTRY *qe;
	QENTRY *qe_tmp;
	int exclude_cnt = 0;

	if(stat(path, &statbuf) < 0) {
		fprintf(stderr, "path error\n");
		return NULL;
	}
	
	if(S_ISDIR(statbuf.st_mode) == 0)
		return NULL;

	fext_len = (fextension == NULL) ? 0 : strlen(fextension);
	
	head = lnklist_init();

	queue_init(&q);
	qe = (QENTRY*)malloc(sizeof(QENTRY));
	qe->depth = 0;
	qe->pathname = realpath(path, NULL);
	enqueue(&q, qe);

	while(q.start != q.end) {
		qe = (QENTRY*)dequeue(&q);
		if(qe == NULL)
			break;

		rpath = qe->pathname;

		if(stat(rpath, &statbuf) < 0) {
			free(qe);
			free(rpath);
			continue;
		}

#ifdef DEBUG
		fprintf(stderr, "left : %d, checking %s\n", left_in_queue(&q), rpath);
#endif

		if(S_ISDIR(statbuf.st_mode)) {
			// 폴더인 경우 하위 디렉토리/파일들을 큐에 넣는다
			dirp = opendir(rpath);
			if(dirp == NULL) {
				fprintf(stderr, "opendir error : \"%s\" - %s\n", rpath, strerror(errno));
			} else {
				while((dentry = readdir(dirp)) != NULL) {
					if(strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0)
						// . 또는 ..의 경우 
						continue;
					
					// trash 디렉토리 생략
					if(trash_path != NULL && strcmp(rpath, trash_path) == 0)
						continue;

					if(
						strcmp(rpath, "/") == 0 && exclude_cnt < 3
						&& (
							strcmp(dentry->d_name, "proc") == 0
							|| strcmp(dentry->d_name, "run") == 0
							|| strcmp(dentry->d_name, "sys") == 0
						)
					) {
						// /proc, /run, /sys 디렉토리 제외
						exclude_cnt++;
						continue;
					}

					// 절대경로 구하면서 qentry 생성
					qe_tmp = (QENTRY*)malloc(sizeof(QENTRY));
					qe_tmp->depth = qe->depth + 1;
					qe_tmp->pathname = path_concat(rpath, dentry->d_name);

					enqueue(&q, qe_tmp);
				}

				closedir(dirp);
			}

			free(qe);
			free(rpath);
		} else if(S_ISREG(statbuf.st_mode)) {
			// 일반 파일인 경우 확장자, 크기 검사 후 해시 값을 구하고 중복되는 값을 찾으면서 리스트에 삽입
			// 크기 검사
			if(!(
				(0 < statbuf.st_size && (llimit == -1 || llimit <= statbuf.st_size))
				&& (ulimit == -1 || statbuf.st_size <= ulimit)
			)) {
				free(qe);
				free(rpath);
				continue;
			}

			// 확장자 검사
			if(fextension != NULL) {
				path_len = strlen(rpath);
				if(path_len - fext_len <= 0 || strcmp(rpath + path_len - fext_len, fextension) != 0) {
					free(qe);
					free(rpath);
					continue;
				}
			}

			fd = open(rpath, O_RDONLY);
			if(fd < 0) {
				fprintf(stderr, "open error : \"%s\" - %s\n", rpath, strerror(errno));
			} else {
				fh = (FILEHASH*)malloc(sizeof(FILEHASH));
				fh->size = statbuf.st_size;
				fh->pathname = rpath;
				fh->hash = hashstrfunc(fd);
				fh->mtime = statbuf.st_mtime;
				fh->atime = statbuf.st_atime;
				fh->depth = qe->depth;

				close(fd);
				insert_filehash(head, fh);
			}

			free(qe);
		} else {
			// 이외의 경우 아무것도 하지 않음
		}
	}

	// 리스트를 돌면서 길이가 1인 중복세트 삭제
	cur = head->next;
	while(head != cur) {
		head2 = (LNKLIST*)(cur->val);
		tmp = cur->next;
		if(head2->next == head2->prev) {
			lnklist_destroy(head2, free_filehash);
			lnklist_delete(cur);
		}

		cur = tmp;
	}

	free(q.arr);
	return head;
}

void timestr(char *buf, time_t t) {
	char time_format[] = "%Y-%m-%d %H:%M:%S";

	strftime(buf, TIMESTR_BUF_SIZE, time_format, gmtime(&t));
}

void print_dup_list(FILE *fp, LNKLIST *head) {
	LNKLIST *cur_1;
	LNKLIST *head_2;
	LNKLIST *cur_2;
	LNKLIST *tmp;
	FILEHASH *fh;
	int set_i;
	int file_i;
	char bytes_buf[50];
	char mtime_buf[TIMESTR_BUF_SIZE];
	char atime_buf[TIMESTR_BUF_SIZE];

	set_i = 1;
	cur_1 = head->next;
	while(head != cur_1) {
		head_2 = (LNKLIST*)(cur_1->val);
		cur_2 = head_2->next;
		fh = (FILEHASH*)(cur_2->val);
		file_i = 1;

		if(head_2 == cur_2) {
			cur_1 = cur_1->next;
			continue;
		}

		size_to_sep_str(bytes_buf, fh->size);
		fprintf(fp, "---- Identical files #%d (%s bytes - %s) ----\n", set_i, bytes_buf, fh->hash);

		while(head_2 != cur_2) {
			fh = (FILEHASH*)(cur_2->val);
			timestr(mtime_buf, fh->mtime);
			timestr(atime_buf, fh->atime);
			fprintf(fp, "[%d] %s (mtime : %s) (atime : %s)\n", file_i, fh->pathname, mtime_buf, atime_buf);
			file_i++;
			cur_2 = cur_2->next;
		}

		set_i++;
		cur_1 = cur_1->next;
		fprintf(fp, "\n");
	}
}

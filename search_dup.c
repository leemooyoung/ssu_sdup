#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "linkedlist.h"
#include "queue.h"
#include "search_dup.h"
#include "inputcntl.h"

#define TIMESTR_BUF_SIZE 20

// linkedlist 안의 linkedlist를 지우기 위한 함수
int lnklist_dest_data(void *val) {
	return lnklist_destroy((LNKLIST*)val, NULL);
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
		// 중복된 파일을 찾았다면 pathname 사전 오름차순으로 정렬되도록 삽입
		head_2 = cur_2;
		cur_2 = head_2->next;
		while(head_2 != cur_2) {
			fhtmp = (FILEHASH*)(cur_2->val);

			if(strcmp(fh->pathname, fhtmp->pathname) < 0)
				break;

			cur_2 = cur_2->next;
		}

		lnklist_insert_after(cur_2->prev, fh);
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
LNKLIST *search_dup(char *path, off_t llimit, off_t ulimit, char *fextension, char *(*hashfunc)(int)) {
	QUEUE q;
	LNKLIST *head;
	LNKLIST *cur;
	LNKLIST *head2;
	LNKLIST *tmp;
	struct stat statbuf;
	char *rpath;
	char *concat_tmp;
	DIR *dirp;
	struct dirent *dentry;
	int path_len;
	int filename_len;
	int fext_len;
	int fd;
	FILEHASH *fh;

	if(stat(path, &statbuf) < 0) {
		fprintf(stderr, "path error\n");
		return NULL;
	}
	
	if(S_ISDIR(statbuf.st_mode) == 0)
		return NULL;

	fext_len = (fextension == NULL) ? 0 : strlen(fextension);
	
	head = lnklist_init();

	queue_init(&q);
	enqueue(&q, realpath(path, NULL));

	while(q.start != q.end) {
		rpath = (char*)dequeue(&q);

		if(rpath == NULL)
			break;

		if(stat(rpath, &statbuf) < 0)
			continue;

#ifdef DEBUG
		fprintf(stderr, "left : %d, checking %s\n", left_in_queue(&q), rpath);
#endif

		if(S_ISDIR(statbuf.st_mode)) {
			// 폴더인 경우 하위 디렉토리/파일들을 큐에 넣는다
			dirp = opendir(rpath);
			while((dentry = readdir(dirp)) != NULL) {
				if(strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0)
					// . 또는 ..의 경우 
					continue;

				// 절대경로 구하기
				path_len = strlen(rpath);
				filename_len = strlen(dentry->d_name);
				concat_tmp = (char*)malloc(sizeof(char) * (path_len + filename_len + 2));

				strcpy(concat_tmp, rpath);
				concat_tmp[path_len] = '/';
				strcpy(concat_tmp + path_len + 1, dentry->d_name);

				enqueue(&q, concat_tmp);
			}

			closedir(dirp);
			free(rpath);
		} else if(S_ISREG(statbuf.st_mode)) {
			// 일반 파일인 경우 확장자, 크기 검사 후 해시 값을 구하고 중복되는 값을 찾으면서 리스트에 삽입
			// 크기 검사
			if(!(
				(0 < statbuf.st_size && (llimit == -1 || llimit <= statbuf.st_size))
				&& (ulimit == -1 || statbuf.st_size <= ulimit)
			)) {
				free(rpath);
				continue;
			}

			// 확장자 검사
			if(fextension != NULL) {
				path_len = strlen(rpath);
				if(path_len - fext_len <= 0 || strcmp(rpath + path_len - fext_len, fextension) != 0) {
					free(rpath);
					continue;
				}
			}

			fd = open(rpath, O_RDONLY);

			fh = (FILEHASH*)malloc(sizeof(FILEHASH));
			fh->size = statbuf.st_size;
			fh->pathname = rpath;
			fh->hash = hashfunc(fd);
			fh->mtime = statbuf.st_mtime;
			fh->atime = statbuf.st_atime;

			close(fd);
			insert_filehash(head, fh);
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
			lnklist_destroy(head2, NULL);
			lnklist_delete(cur);
		}

		cur = tmp;
	}

	free(q.arr);
	return head;
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
	char time_format[] = "%Y-%m-%d %H:%M:%S";

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
			strftime(mtime_buf, TIMESTR_BUF_SIZE, time_format, gmtime(&(fh->mtime)));
			strftime(atime_buf, TIMESTR_BUF_SIZE, time_format, gmtime(&(fh->atime)));
			fprintf(fp, "[%d] %s (mtime : %s) (atime : %s)\n", file_i, fh->pathname, mtime_buf, atime_buf);
			file_i++;
			cur_2 = cur_2->next;
		}

		set_i++;
		cur_1 = cur_1->next;
		fprintf(fp, "\n");
	}
}

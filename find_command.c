#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "find_command.h"
#include "inputcntl.h"
#include "search_dup.h"
#include "linkedlist.h"

#define INPUT_SIZE (100)

char *trash_path;
char trash_name[] = ".trash";

void print_option_help() {
	printf(
">> [SET_INDEX] [OPTION ...]\n\
   [OPTION ...]\n\
   d [LIST_IDX] : delete [LIST_IDX] file\n\
   i : ask for confirmation before delete\n\
   f : force delete except the recently modified file\n\
   t : force move to Trash except the recently modified file\n\
   da : force delete all files\n\
   ta : force move all files to Trash\n\
   of : force delete except the oldest modified file\n\
   ot : force move to Trash except the oldest modified file\n\n");
}

// 임의의 디렉토리를 trash 디렉토리로 사용해도 된다고 하셨기 때문에 ~/.trash로 정함
char *create_trash_dir(char *trash_name) {
	char *home_path;
	char *trash_path_loc;
	struct stat statbuf;

	if((home_path = getenv("HOME")) == NULL) {
		fprintf(stderr, "trash folder creation error\n");
		return NULL;
	}

	trash_path_loc = path_concat(home_path, trash_name);

	if(stat(trash_path_loc, &statbuf) < 0) {
		if(mkdir(trash_path_loc, 0755) < 0) {
			// 없어서 만들었는데 만들기 실패
			fprintf(stderr, "trash folder creation error\n");
			free(trash_path_loc);
			return NULL;
		}
	} else
		if(!S_ISDIR(statbuf.st_mode)) {
			// 있는데 디렉토리가 아님
			fprintf(stderr, "trash folder creation error. delete \"%s\"\n", trash_path_loc);
			free(trash_path_loc);
			return NULL;
		}

	return trash_path_loc;
}

int find_command(int argc, char *argv[], char *(*hashstrfunc)(int)) {
	char *fext;
	off_t llimit;
	off_t ulimit;
	char *target_dir;
	char *env_home;
	LNKLIST *head;
	LNKLIST *set;
	LNKLIST *flist;
	FILEHASH *fh;
	DIR *dp;
	struct timeval start_tv;
	struct timeval end_tv;
	int sec;
	int usec;
	char input[INPUT_SIZE];
	char **words = NULL;
	char *end;
	int set_index;
	int list_index;
	int option;
	int is_worng_option;

	if(argc != 5) {
		fprintf(stderr, "%s [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n", argv[0]);
		return 1;
	}

	// FILE_EXTENSION 인자 검사
	if(strcmp(argv[1], "*") == 0) {
		fext = NULL;
	} else if(strncmp(argv[1], "*.", 2) == 0 && argv[1][2] != '\0') {
		fext = argv[1] + 1;
	} else {
		fprintf(stderr, "[FILE_EXTENSION] : * | *.[extension]\n");
		return 1;
	}

	// MINSIZE, MAXSIZE 검사
	if((llimit = strtosize(argv[2])) == -2) {
		fprintf(stderr, "[MINSIZE] : ([integer] | [float])(kb | KB | mb | MB | gb | GB)\n");
		return 1;
	}

	if((ulimit = strtosize(argv[3])) == -2) {
		fprintf(stderr, "[MAXSIZE] : ([integer] | [float])(kb | KB | mb | MB | gb | GB)\n");
		return 1;
	}

	env_home = getenv("HOME");
	if(strcmp(argv[4], "~") == 0 && env_home != NULL) {
		target_dir = env_home;
	} else if(strncmp(argv[4], "~/", 2) == 0 && env_home != NULL) {
		target_dir = path_concat(env_home, argv[4] + 2);
	} else
		target_dir = argv[4];

	if((dp = opendir(target_dir)) == NULL) {
		fprintf(stderr, "[TARGET_DIRECTORY] is wrong\n");
		return 1;
	}

	closedir(dp);

	// trash 디렉토리 생성
	if(trash_path == NULL) {
		trash_path = create_trash_dir(trash_name);
	}

	gettimeofday(&start_tv, NULL);
	head = search_dup(target_dir, llimit, ulimit, fext, hashstrfunc);
	gettimeofday(&end_tv, NULL);

	sec = end_tv.tv_sec - start_tv.tv_sec;
	usec = end_tv.tv_usec - start_tv.tv_usec;
	if(usec < 0) {
		sec--;
		usec += 1000000;
	}

	if(head->next == head) {
		// 중복 파일이 없다면
		lnklist_destroy(head, NULL);
		printf("No duplicates in %s\n", target_dir);
		printf("Searching time: %d:%06d(sec:usec)\n\n", sec, usec);
		return 0;
	} else {
		print_dup_list(stdout, head);
		printf("Searching time: %d:%06d(sec:usec)\n\n", sec, usec);
	}

	// 옵션 
	while(1) {
		printf(">> ");
		if(get_input(stdin, input, INPUT_SIZE) == NULL) {
			fprintf(stderr, "read error\n");
			break;
		}

		if(strcmp(input, "exit") == 0) {
			printf(">> Back to Prompt\n");
			break;
		}

		words = split_by_word(input);

		if(words == NULL)
			continue;
		
		// index 는 1부터 시작하기 때문에 words[0]이 0인지 숫자가 아닌건지 구분할 필요 없음
		set_index = strtol(words[0], &end, 10);
		if(set_index == 0 || *end != '\0') {
			print_option_help();
			free(words);
			continue;
		}

		is_worng_option = 0;
		if(words[1] == NULL) {
			is_worng_option = 1;
		} else if(words[2] == NULL) {
			// 옵션에 따른 작업 수행
			if(strcmp(words[1], "i") == 0) {
				option = FIND_OPTION_INTERACTIVE;
			} else if(strcmp(words[1], "f") == 0) {
				option = FIND_OPTION_FORCE;
			} else if(strcmp(words[1], "t") == 0) {
				option = FIND_OPTION_TRASH;
			} else if(strcmp(words[1], "da") == 0) {
				option = FIND_OPTION_DELETE_ALL;
			} else if(strcmp(words[1], "ta") == 0) {
				option = FIND_OPTION_TRASH_ALL;
			} else if(strcmp(words[1], "of") == 0) {
				option = FIND_OPTION_OLD_FORCE;
			} else if(strcmp(words[1], "ot") == 0) {
				option = FIND_OPTION_OLD_TRASH;
			} else {
				is_worng_option = 1;
			}
		} else if(words[3] == NULL) {
			if(strcmp(words[1], "d") == 0) {
				list_index = strtol(words[2], &end, 10);
				if(list_index == 0 || *end != '\0') {
					is_worng_option = 1;
				} else {
					option = FIND_OPTION_DELETE;
				}
			} else {
				is_worng_option = 1;
			}
		} else {
			is_worng_option = 1;
		}

		free(words);

		if(!is_worng_option)
			dup_set_process(head, set_index, option, list_index);
		else {
			print_option_help();
			continue;
		}

		printf("\n");

		if(head->next == head)
			break;
		else {
			print_dup_list(stdout, head);
		}
	}

	if(target_dir != argv[4] && target_dir != env_home)
		free(target_dir);
	
	if(trash_path != NULL) {
		free(trash_path);
		trash_path = NULL;
	}

	lnklist_destroy(head, lnklist_dest_fh);

	return 0;
}

int remove_all_files(LNKLIST *head, int option) {
	LNKLIST *node;
	FILEHASH *fh;
	char *del_pathname;

	if(trash_path == NULL)
		return 1;

	node = head->next;
	while(node != head) {
		fh = (FILEHASH*)(node->val);
		if(option == FIND_OPTION_FORCE || option == FIND_OPTION_DELETE_ALL || option == FIND_OPTION_OLD_FORCE) {
			// 삭제
			if(remove(fh->pathname) < 0)
				fprintf(stderr, "delete error : %s\n", fh->pathname);
		} else if(option == FIND_OPTION_TRASH || option == FIND_OPTION_TRASH_ALL || option == FIND_OPTION_OLD_TRASH) {
			// 휴지통으로 이동
			del_pathname = path_concat(trash_path, get_filename(fh->pathname));
			if(rename(fh->pathname, del_pathname) < 0) {
				// 이동 실패시 삭제
				fprintf(stderr, "move to trash failed. delete : %s\n", fh->pathname);
				if(trash_path != NULL) {
					free(trash_path);
					trash_path = NULL;
				}
				if(remove(fh->pathname) < 0)
					fprintf(stderr, "delete error : %s\n", fh->pathname);
			}

			free(del_pathname);
		}
		
		node = node->next;
	}

	return 0;
}

int dup_set_process(LNKLIST *head, int set_index, int option, int list_index) {
	char input[INPUT_SIZE];
	char timestr_buf[TIMESTR_BUF_SIZE];
	struct stat statbuf;
	LNKLIST *set_node;
	LNKLIST *set;
	LNKLIST *node;
	LNKLIST *latest;
	FILEHASH *latest_fh;
	FILEHASH *fh;

	set_node = lnklist_find_n(head, set_index);
	if(set_node == NULL) {
		fprintf(stderr, "[SET_INDEX] range error\n");
		return 1;
	}

	set = (LNKLIST*)(set_node->val);

	switch(option) {
		case FIND_OPTION_DELETE:
			node = lnklist_find_n(set, list_index);
			if(node == NULL) {
				fprintf(stderr, "[LIST_IDX] range error\n");
				return 1;
			}

			fh = lnklist_delete(node);
			if(remove(fh->pathname) < 0)
				fprintf(stderr, "delete error : %s\n", fh->pathname);

			if(set->next == set->prev)
				lnklist_destroy((LNKLIST*)lnklist_delete(set_node), free_filehash);

			printf("\"%s\" has been deleted in #%d\n", fh->pathname, set_index);
			free_filehash(fh);
		break;
		case FIND_OPTION_INTERACTIVE:
			node = set->next;
			while(node != set) {
				fh = (FILEHASH*)(node->val);
				// latest는 원래 여기에 쓸 용도의 변수는 아니지만,
				// node가 free되는 상황에 대비해 node->next를 임시로 저장해둘 공간이 필요해서 사용했다.
				latest = node->next;
				printf("Delete \"%s\"? [y/n] ", fh->pathname);
				if(get_input(stdin, input, INPUT_SIZE) == NULL) {
					fprintf(stderr, "read error\n");
					return 2;
				}

				if(strcmp(input, "y") == 0 || strcmp(input, "Y") == 0) {
					if(remove(fh->pathname) < 0)
						fprintf(stderr, "delete error : %s\n", fh->pathname);

					lnklist_delete(node);
					free_filehash(fh);
				} else if (strcmp(input, "n") == 0 || strcmp(input, "N") == 0) {
					;
				} else {
					continue;
				}

				node = latest;
			}

			if(set->next == set->prev)
				lnklist_destroy((LNKLIST*)lnklist_delete(set_node), free_filehash);
		break;
		case FIND_OPTION_TRASH:
			// trash 옵션인 경우 trash 폴더가 있는지 검사. 없다면(trash_path == NULL) 만들기를 시도
			if(trash_path == NULL) {
				trash_path = create_trash_dir(trash_name);
				if(trash_path == NULL)
					// 만들기 실패했다면 종료
					return 3;
			}
		case FIND_OPTION_FORCE:
			// trash, force 옵션의 공통부분. 중복 파일 세트를 돌면서 작업(삭제, 이동) 수행
			node = set->next;
			latest = node;
			while(node != set) {
				fh = (FILEHASH*)(node->val);
				if(((FILEHASH*)(latest->val))->mtime < fh->mtime) {
					latest = node;
				}

				node = node->next;
			}

			latest_fh = lnklist_delete(latest);
			timestr(timestr_buf, latest_fh->mtime);

			remove_all_files(set, option);
			lnklist_destroy((LNKLIST*)lnklist_delete(set_node), free_filehash);

			if(option == FIND_OPTION_FORCE)
				printf("Left file in #%d : %s (%s)\n", set_index, latest_fh->pathname, timestr_buf);
			else if(option == FIND_OPTION_TRASH)
				printf("All files in #%d have moved to Trash except \"%s\" (%s)\n", set_index, latest_fh->pathname, timestr_buf);
			
			free_filehash(latest_fh);
		break;
		case FIND_OPTION_TRASH_ALL:
			// trash 옵션인 경우 trash 폴더가 있는지 검사. 없다면(trash_path == NULL) 만들기를 시도
			if(trash_path == NULL) {
				trash_path = create_trash_dir(trash_name);
				if(trash_path == NULL)
					// 만들기 실패했다면 종료
					return 3;
			}
		case FIND_OPTION_DELETE_ALL:
			remove_all_files(set, option);
			lnklist_destroy((LNKLIST*)lnklist_delete(set_node), free_filehash);

			if(option == FIND_OPTION_DELETE_ALL)
				printf("All files in #%d have deleted\n", set_index);
			else if(option == FIND_OPTION_TRASH_ALL)
				printf("ALL files in #%d have moved to Trash\n", set_index);
		break;
		case FIND_OPTION_OLD_TRASH:
			// trash 옵션인 경우 trash 폴더가 있는지 검사. 없다면(trash_path == NULL) 만들기를 시도
			if(trash_path == NULL) {
				trash_path = create_trash_dir(trash_name);
				if(trash_path == NULL)
					// 만들기 실패했다면 종료
					return 3;
			}
		case FIND_OPTION_OLD_FORCE:
			node = set->next;
			// 여기서는 latest가 아니긴 하지만 그냥 갖다 씀
			// 가장 오래전에 수정된 파일 찾기
			latest = node;
			while(node != set) {
				fh = (FILEHASH*)(node->val);
				if(((FILEHASH*)(latest->val))->mtime > fh->mtime) {
					latest = node;
				}

				node = node->next;
			}

			latest_fh = lnklist_delete(latest);
			timestr(timestr_buf, latest_fh->mtime);

			remove_all_files(set, option);
			lnklist_destroy((LNKLIST*)lnklist_delete(set_node), free_filehash);

			if(option == FIND_OPTION_OLD_FORCE)
				printf("Left file in #%d : %s (%s)\n", set_index, latest_fh->pathname, timestr_buf);
			else if(option == FIND_OPTION_OLD_TRASH)
				printf("All files in #%d have moved to Trash except \"%s\" (%s)\n", set_index, latest_fh->pathname, timestr_buf);

			free_filehash(latest_fh);
		break;
		default:
			return 3;
	}

	return 0;
}

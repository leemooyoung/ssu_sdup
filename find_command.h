#ifndef _SDUP_FIND_COMMAND
#define _SDUP_FIND_COMMAND

#include "linkedlist.h"

#define FIND_OPTION_DELETE (1)
#define FIND_OPTION_INTERACTIVE (2)
#define FIND_OPTION_FORCE (3)
#define FIND_OPTION_TRASH (4)
#define FIND_OPTION_DELETE_ALL (5)
#define FIND_OPTION_TRASH_ALL (6)
#define FIND_OPTION_OLD_FORCE (7)
#define FIND_OPTION_OLD_TRASH (8)

// find-md5, find-sha1 명령어에 해당하는 함수
int find_command(int argc, char *argv[], char *(*hashstrfunc)(int));

/* 중복 파일 세트 처리 함수
 * option:
 *   FIND_OPTION_DELETE: 
 *     list_index 번째 원소에 해당하는 파일 삭제. 삭제 후 중복 파일 세트의 원소가 하나 이하라면 중복 파일 세트를 제거함
 *     이 옵션 이외에는 list_index가 무시됨
 *   FIND_OPTION_INTERACTIVE: 세트의 원소마다 삭제할지 말지 여부를 물어보고 삭제/유지 함 (Y|y|N|n)
 *   FIND_OPTION_FORCE: 가장 최근에 수정한 파일만 남기고 나머지 파일들을 전부 삭제. 중복 파일 세트를 제거함
 *   FIND_OPTION_TRASH: 가장 최근에 수정한 파일만 남기고 나머지 파일들을 전부 휴지통으로 옮김
 *   FIND_OPTION_DELETE_ALL: 세트의 원소를 전부 지움
 *   FIND_OPTION_TRASH_ALL: 세트의 원소를 전부 휴지통으로 이동
 *   FIND_OPTION_OLD_FORCE: 가장 예전에 수정한 파일만 남기고 나머지 파일들을 전부 삭제. 중복 파일 세트를 제거함
 *   FIND_OPTION_OLD_TRASH: 가장 예전에 수정한 파일만 남기고 나머지 파일들을 전부 휴지통으로 옮김
 */
int dup_set_process(LNKLIST *head, int set_index, int option, int list_index);

// 옵션 도움말 출력
void print_option_help();

// Trash directory 생성
char *create_trash_dir(char *trash_name);

// trash_path 가 있는 상황에서만 호출해야 한다
// LNKLIST head에 있는 모든 파일을 삭제한다
// LNKLIST는 지우지 않는다.
int remove_all_files(LNKLIST *head, int option);

#endif

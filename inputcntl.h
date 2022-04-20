#ifndef _SDUP_INPUTCNTL_H
#define _SDUP_INPUTCNTL_H

#include <stdio.h>

#define SBW_RES_ARR_DEFAULT 5

// EOF 또는 개행을 받을 때 까지의 입력을 받는다. EOF, 개행은 버려진다.
// 최대 size - 1 개 만큼의 문자를 받을 수 있다.
// size - 1 이후 EOF 또는 개행을 만날 때 까지의 입력은 버려진다.
char *get_input(FILE *fp, char * buf, int size);

// space 단위로 분리
// 원본 배열 (str) 에 있는 space를 전부 \0 로 바꾼다.
// 각 word의 시작 주소를 저장한 배열을 동적 할당해서 그 시작 주소를 반환함
char **split_by_word(char *str);

// (INTEGER|FLOAT)(kb|mb|gb|KB|MB|GB) 형식의 문자열을 off_t로 변환
// return:
// -1 : 무제한
// -2 : 잘못된 입력
// 그 외 : size
off_t strtosize(char *str);

// 천 단위마다 쉼표로 구분된 문자열 생성. 음수는 지원하지 않음
int size_to_sep_str(char *buf, off_t size);

// 두 path를 연결한다. path2는 '/'로 시작하면 안됨
char *path_concat(char *path1, char *path2);

// path에서 filename의 시작지점을 반환한다.
char *get_filename(char *path);

#endif

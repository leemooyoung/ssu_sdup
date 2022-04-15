#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include "inputcntl.h"

char *get_input(FILE *fp, char * buf, int size) {
	// EOF 또는 개행을 받을 때 까지의 입력을 받는다. EOF, 개행은 버려진다.
	// 최대 size - 1 개 만큼의 문자를 받을 수 있다.
	// size - 1 이후 EOF 또는 개행을 만날 때 까지의 입력은 버려진다.

	int c;
	int i;

	for(i = 0; i < size - 1 && (c = getc(fp)) != EOF && c != '\n'; i++)
		buf[i] = c;

	buf[i] = '\0';

	if(i == size - 1)
		while((c = getc(fp)) != EOF && c != '\n')
			;

	return c == EOF ? NULL : buf;
}

char **split_by_word(char *str) {
	// space 단위로 분리
	// 원본 배열 (str) 에 있는 space를 전부 \0 로 바꾼다.
	// 각 word의 시작 주소를 저장한 배열을 동적 할당해서 그 시작 주소를 반환함

	int word_cnt = 0;
	int arr_size = SBW_RES_ARR_DEFAULT;
	char **result;
	char *tmp;

	if(str == NULL)
		return NULL;

	result = (char**)malloc(arr_size * sizeof(char*));

	do {
		// 공백 제거
		while(*str == ' ') {
			*(str++) = '\0';
		}

		if(*str == '\0')
			break;

		// 단어 저장
		if(arr_size <= word_cnt + 1)
			result = (char**)realloc(result, (arr_size *= 2) * sizeof(char*));

		result[word_cnt++] = str;

		// 다음 공백 탐색
	} while((str = strchr(str, ' ')) != NULL);

	if(word_cnt)
		result[word_cnt] = NULL;
	else {
		free(result);
		result = NULL;
	}

	return result;
}

// -1 : 무제한
// -2 : 잘못된 입력
// 그 외 : size
off_t strtosize(char *str) {
	off_t size;
	int i;
	off_t ipart = 0;
	int frpart = 0;
	int frpart_digit = 1; // frpart 보다 큰 10의 거듭제곱 수 중 가장 작은 수
	int size_unit = 1;

	if(strcmp(str, "~") == 0)
		return -1;
	
	if(str[0] == '0' && isdigit(str[1])) {
		return -2;
	}

	i = 0;
	while(1) {
		if(isdigit(str[i])) {
			ipart = ipart * 10 + str[i] - '0';
			if(ipart < 0)
				// overflow
				return -1;
		} else
			break;

		i++;
	}

	if(str[i] == '.') {
		if(!isdigit(str[i+1]))
			return -2;
		else {
			i++;
			while(1) {
				if(isdigit(str[i])) {
					frpart = frpart * 10 + str[i] - '0';
					frpart_digit *= 10;
					if(1000000000 <= frpart_digit) {
						// 소수점 아래 9자리 밑으로는 무시
						while(isdigit(str[i]))
							i++;
						
						break;
					}
				} else
					break;
				
				i++;
			}
		}
	}

	if(str[i] == '\0') {
		return ipart;
	} else {
		if(strcmp(str + i, "kb") == 0 || strcmp(str + i, "KB") == 0) {
			size_unit = 1000;
		} else if(strcmp(str + i, "mb") == 0 || strcmp(str + i, "MB") == 0) {
			size_unit = 1000000;
		} else if(strcmp(str + i, "gb") == 0 || strcmp(str + i, "GB") == 0) {
			size_unit = 1000000000;
		} else {
			return -2;
		}
	}

	// 단위에 맞게 정수부, 실수부 합성
	if(frpart_digit < size_unit) {
		frpart *= (size_unit / frpart_digit);
	} else {
		frpart /= (frpart_digit / size_unit);
	}

	size = ipart * size_unit;
	if(size / size_unit != ipart)
		// overflow
		return -1;

	return size + frpart;
}

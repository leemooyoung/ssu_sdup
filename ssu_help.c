#include <stdio.h>
#include <stdlib.h>

int main(void) {
	printf(
"Usage:\n\
  > fmd5/fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n\
    >> [SET_INDEX] [OPTION ...]\n\
       [OPTION ...]\n\
       d [LIST_IDX] : delete [LIST_IDX] file\n\
       i : ask for confirmation before delete\n\
       f : force delete except the recently modified file\n\
       t : force move to Trash except the recently modified file\n\
  > help\n\
  > exit\n\n");
	exit(0);
}

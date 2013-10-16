/*
 * Adapted from linux-kernel/scripts/bin2c.c
 *   Jan 1999 Matt Mackall <mpm@selenic.com>
 *
 * And StackOverflow question
 *   http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
 */
#include <stdio.h>

int main(int argc, char **argv)
{
  int ch, total = 0;

  printf("static const char %s[] =\n", argv[1]);

  do {
    printf("\t\"");
    while ((ch = getchar()) != EOF) {
      total++;
      printf("\\x%02x", ch);
      if (total % 16 == 0)
        break;
    }
    printf("\"\n");
  } while (ch != EOF);

  printf("\t;\n\nstatic const int %s_size = %d;\n", argv[1], total);

  return 0;
}

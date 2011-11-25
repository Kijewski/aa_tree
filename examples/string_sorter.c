#include "../aa_tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define STR_FREE(S) free (*S)
#define STR_CMP(A,B) strcmp (*A, *B)
AA_TREE (stringTree, char*, STR_CMP, STR_FREE, malloc, free, assert)

int main (void)
{
  stringTree *tree = stringTreeNew ();
  
  char *lineptr = NULL;
  size_t n = 0;
  for (;;)
    {
      ssize_t len = getline (&lineptr, &n, stdin);
      if (len == 1)
        continue;
      else if (len <= 0)
        break;
      lineptr[len-1] = '\0';
        
      char *string = strdup (lineptr);
      if (!stringTreeInsert (tree, &string))
        {
          fprintf (stdout, "Deleting: “%.*s”.\n", (int) len, string);
          stringTreeRemove (tree, &string, true);
          free (string);
        }
    }
  free (lineptr);
  
  #define STR_FUN(S) fprintf (stdout, "Contains: “%s”.\n", *S)
  AA_FOR_EACH (stringTree, STR_FUN, tree);
  
  fprintf (stdout, "\nSize = %u.\nDepth = %u.\n", aa_size (tree), aa_depth (tree));
  
  stringTreeFree (tree);
  return 0;
}

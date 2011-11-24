#include "../aa_tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define STR_FREE(S) ({ free (*(char**)S); })
#define STR_CMP(A,B) strcmp (*A, *B)
AA_TREE (stringTree, const char*, STR_CMP, STR_FREE, malloc, free, AA_NOP)

int main ()
{
  stringTree *tree = stringTreeNew ();
  
  char *lineptr = NULL;
  size_t n = 0;
  for (;;)
    {
      ssize_t len = getline (&lineptr, &n, stdin);
      if (len < 0)
        break;
        
      const char *string = strndup (lineptr, len - (lineptr[len-1] < 32));
      const char **new_item = &string;
      
      switch (stringTreeInsert (tree, &new_item))
        {
        case AAIR_INSERTED:
          break;
        case AAIR_EXISTS:
          fprintf (stdout, "“%.*s” already exists.\n", (int) len, string);
          break;
        case AAIR_MEMORY_EXHAUSTED:
          fprintf (stdout, "Your memory seems to be exhausted.\n");
          break;
        }
    }
  free (lineptr);
  
  #define STR_FUN(S) fprintf (stdout, "%s\n", *S)
  AA_FOR_EACH (stringTree, STR_FUN, tree);
  
  stringTreeFree (tree);
  return 0;
}

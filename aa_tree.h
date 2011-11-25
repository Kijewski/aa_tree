#ifndef AA_TREE_H__
#define AA_TREE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

struct aa_node_
{
  uint32_t         level;
  struct aa_node_ *left;
  struct aa_node_ *right;
};

struct aa_tree_
{
  struct aa_node_ *root;
};

enum aa_insert_result
{
  AAIR_INSERTED,
  AAIR_EXISTS,
  AAIR_MEMORY_EXHAUSTED
};

extern struct aa_node_ *
aa_inserted_ (struct aa_node_ *t);

extern struct aa_node_ *
aa_removed_ (struct aa_node_ *t);

#define AA_NOP(X) ({ (void)0; })

#define AA_IS_NIL_(N) ((N) == (__typeof (N)) &AA_NIL_)

#define AA_CMP_FROM_LESS(TYPE, CMP)                                           \
(const __typeof(TYPE) *A, const __typeof(TYPE) *B)                            \
{                                                                             \
  if (CMP (A, B))                                                             \
    return -1;                                                                \
  else if (CMP (B, A))                                                        \
    return +1;                                                                \
  return 0;                                                                   \
}

#define AA_TREE(NAME, TYPE, CMP, FREE_DATUM, MALLOC, FREE, ASSERT)            \
                                                                              \
struct aa_node_##NAME##_;                                                     \
struct aa_tree_##NAME##_;                                                     \
typedef __typeof(TYPE) aa_type_##NAME##_;                                     \
typedef struct aa_tree_##NAME##_ NAME;                                        \
                                                                              \
/* *********************************************************************** */ \
/*                                                                         */ \
/*                       PUBLIC INTERFACE FUNCTIONS:                       */ \
/*                                                                         */ \
/*     / *********************************************************** \     */ \
/*     */                                                           /*     */ \
/*     */     static inline NAME *                                  /*     */ \
/*     */     NAME##New (void);                                     /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline void                                    /*     */ \
/*     */     NAME##Free (NAME *tree);                              /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline void                                    /*     */ \
/*     */     NAME##Init (NAME *dest);                              /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline void                                    /*     */ \
/*     */     NAME##Clear (NAME *tree,                              /*     */ \
/*     */                  bool  free_items);                       /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline bool                                    /*     */ \
/*     */     NAME##Insert (NAME *tree,                             /*     */ \
/*     */                   TYPE *datum);                           /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline enum aa_insert_result                   /*     */ \
/*     */     NAME##Insert2 (NAME  *tree,                           /*     */ \
/*     */                    TYPE **datum);                         /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline TYPE *                                  /*     */ \
/*     */     NAME##Find (NAME *tree,                               /*     */ \
/*     */                 TYPE *datum);                             /*     */ \
/*     */                                                           /*     */ \
/*     */     static inline bool                                    /*     */ \
/*     */     NAME##Remove (NAME *tree,                             /*     */ \
/*     */                   TYPE *datum,                            /*     */ \
/*     */                   bool  free_item);                       /*     */ \
/*     */                                                           /*     */ \
/*     \ *********************************************************** /     */ \
/*                                                                         */ \
/*                    "HIDDEN" IMPLEMENTATION INTERNALS:                   */ \
/*                                                                         */ \
/* *********************************************************************** */ \
                                                                              \
struct aa_node_##NAME##_                                                      \
{                                                                             \
  uint32_t                  level;                                            \
  struct aa_node_##NAME##_ *left;                                             \
  struct aa_node_##NAME##_ *right;                                            \
  aa_type_##NAME##_         datum;                                            \
};                                                                            \
                                                                              \
struct aa_tree_##NAME##_                                                      \
{                                                                             \
  struct aa_node_##NAME##_ *root;                                             \
};                                                                            \
                                                                              \
static inline void * __attribute__ ((malloc))                                 \
NAME##Malloc##_ (size_t size)                                                 \
{                                                                             \
  return MALLOC (size);                                                       \
}                                                                             \
                                                                              \
static inline void                                                            \
NAME##Free##_ (void *ptr)                                                     \
{                                                                             \
  ASSERT (ptr != NULL);                                                       \
  FREE (ptr);                                                                 \
}                                                                             \
                                                                              \
static inline void                                                            \
NAME##FreeDatum##_ (aa_type_##NAME##_ *datum)                                 \
{                                                                             \
  FREE_DATUM (datum);                                                         \
}                                                                             \
                                                                              \
static inline int                                                             \
NAME##Cmp_ (const aa_type_##NAME##_ *left, const aa_type_##NAME##_ *right)    \
{                                                                             \
  if (left == right)                                                          \
    return 0;                                                                 \
  return CMP (left, right);                                                   \
}                                                                             \
                                                                              \
static inline NAME *                                                          \
NAME##New (void)                                                              \
{                                                                             \
  NAME *result = NAME##Malloc##_ (sizeof (*result));                          \
  if (!result)                                                                \
    return NULL;                                                              \
  NAME##Init (result);                                                        \
  return result;                                                              \
}                                                                             \
                                                                              \
static inline void                                                            \
NAME##Init (NAME *dest)                                                       \
{                                                                             \
  ASSERT (dest != NULL);                                                      \
  memset (dest, 0, sizeof (*dest));                                           \
  dest->root = (struct aa_node_##NAME##_ *) &AA_NIL_;                         \
}                                                                             \
                                                                              \
static inline void                                                            \
NAME##Free (NAME *tree)                                                       \
{                                                                             \
  if (tree == NULL)                                                           \
    return;                                                                   \
  NAME##Clear (tree, true);                                                   \
  NAME##Free##_ (tree);                                                       \
}                                                                             \
                                                                              \
static inline struct aa_node_##NAME##_ *                                      \
NAME##NewNode_ (aa_type_##NAME##_ *datum)                                     \
{                                                                             \
  struct aa_node_##NAME##_ *result = NAME##Malloc##_ (sizeof (*result));      \
  if (!result)                                                                \
    return NULL;                                                              \
  memset (result, 0, sizeof (*result));                                       \
  result->level = 1;                                                          \
  result->left = result->right = (__typeof (result)) &AA_NIL_;                \
  result->datum = *datum;                                                     \
  return result;                                                              \
}                                                                             \
                                                                              \
static inline struct aa_node_##NAME##_ *                                      \
NAME##Insert##_ (aa_type_##NAME##_        **datum,                            \
                 struct aa_node_##NAME##_  *node,                             \
                 enum aa_insert_result     *result)                           \
{                                                                             \
  struct aa_node_##NAME##_ *t;                                                \
  if (AA_IS_NIL_ (node))                                                      \
    {                                                                         \
      t = NAME##NewNode_ (*datum);                                            \
      if (t)                                                                  \
        {                                                                     \
          *result = AAIR_INSERTED;                                            \
          *datum = &t->datum;                                                 \
          return t;                                                           \
        }                                                                     \
      *result = AAIR_MEMORY_EXHAUSTED;                                        \
      return NULL;                                                            \
    }                                                                         \
                                                                              \
  int cmp_result = NAME##Cmp_ (*datum, &node->datum);                         \
  if (cmp_result < 0)                                                         \
    {                                                                         \
      t = NAME##Insert##_ (datum, node->left, result);                        \
      if (!t)                                                                 \
        return NULL;                                                          \
      node->left = t;                                                         \
    }                                                                         \
  else if (cmp_result > 0)                                                    \
    {                                                                         \
      t = NAME##Insert##_ (datum, node->right, result);                       \
      if (!t)                                                                 \
        return NULL;                                                          \
      node->right = t;                                                        \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      *result = AAIR_EXISTS;                                                  \
      *datum = &node->datum;                                                  \
      return NULL;                                                            \
    }                                                                         \
                                                                              \
  return (struct aa_node_##NAME##_ *) aa_inserted_ ((struct aa_node_ *) node);\
}                                                                             \
                                                                              \
static inline enum aa_insert_result                                           \
NAME##Insert2 (struct aa_tree_##NAME##_  *tree,                               \
               aa_type_##NAME##_        **datum)                              \
{                                                                             \
  ASSERT (tree != NULL);                                                      \
  ASSERT (datum != NULL && *datum != NULL);                                   \
                                                                              \
  if (!tree->root)                                                            \
    NAME##Init (tree);                                                        \
                                                                              \
  enum aa_insert_result result = -1;                                          \
  struct aa_node_##NAME##_ *t = NAME##Insert##_ (datum, tree->root, &result); \
  if (result == AAIR_INSERTED)                                                \
    tree->root = t;                                                           \
  return result;                                                              \
}                                                                             \
                                                                              \
static inline bool                                                            \
NAME##Insert (struct aa_tree_##NAME##_ *tree,                                 \
              aa_type_##NAME##_        *datum)                                \
{                                                                             \
  return NAME##Insert2 (tree, &datum) == AAIR_INSERTED;                       \
}                                                                             \
                                                                              \
static inline aa_type_##NAME##_ *                                             \
NAME##Find (struct aa_tree_##NAME##_ *tree,                                   \
            aa_type_##NAME##_        *datum)                                  \
{                                                                             \
  ASSERT (tree != NULL);                                                      \
  ASSERT (datum != NULL);                                                     \
                                                                              \
  struct aa_node_##NAME##_ *node;                                             \
  for (node = tree->root; !AA_IS_NIL_ (node); )                               \
    {                                                                         \
      int cmp_result = NAME##Cmp_ (datum, &node->datum);                      \
      if (cmp_result < 0)                                                     \
        node = node->left;                                                    \
      else if (cmp_result > 0)                                                \
        node = node->right;                                                   \
      else                                                                    \
        return &node->datum;                                                  \
    }                                                                         \
  return NULL;                                                                \
}                                                                             \
                                                                              \
struct NAME##Remove##_data_                                                   \
{                                                                             \
  aa_type_##NAME##_         *datum;                                           \
  struct aa_node_##NAME##_  *deletee;                                         \
  bool                       free_item;                                       \
};                                                                            \
                                                                              \
static inline struct aa_node_##NAME##_ *                                      \
NAME##Remove##_ (struct aa_node_##NAME##_    *t,                              \
                 struct NAME##Remove##_data_ *data)                           \
{                                                                             \
  ASSERT (!AA_IS_NIL_ (t));                                                   \
                                                                              \
  int cmp_result = !data->deletee ? NAME##Cmp_ (data->datum, &t->datum) : -1; \
  if (cmp_result == 0)                                                        \
    data->deletee = t;                                                        \
                                                                              \
  bool is_bottom;                                                             \
  if (cmp_result < 0)                                                         \
    {                                                                         \
      is_bottom = AA_IS_NIL_ (t->left);                                       \
      if (!is_bottom)                                                         \
        t->left = NAME##Remove##_ (t->left, data);                            \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      /* go down to the bottom, finding leftmost t  */                        \
      is_bottom = AA_IS_NIL_ (t->right);                                      \
      if (!is_bottom)                                                         \
        t->right = NAME##Remove##_ (t->right, data);                          \
    }                                                                         \
                                                                              \
  if (is_bottom && data->deletee)                                             \
    {                                                                         \
      ASSERT (AA_IS_NIL_ (t->left));                                          \
                                                                              \
      if (data->free_item)                                                    \
        NAME##FreeDatum##_ (&data->deletee->datum);                           \
      data->deletee->datum = t->datum;                                        \
                                                                              \
      struct aa_node_##NAME##_ *result = t->right;                            \
      NAME##Free##_ (t);                                                      \
      return result;                                                          \
    }                                                                         \
  else if (data->deletee)                                                     \
    return (struct aa_node_##NAME##_*) aa_removed_ ((struct aa_node_ *) t);   \
  else                                                                        \
    return t;                                                                 \
}                                                                             \
                                                                              \
static inline bool                                                            \
NAME##Remove (struct aa_tree_##NAME##_ *tree,                                 \
              aa_type_##NAME##_        *datum,                                \
              bool                      free_item)                            \
{                                                                             \
  ASSERT (tree != NULL);                                                      \
  ASSERT (datum != NULL);                                                     \
                                                                              \
  if (!tree->root || AA_IS_NIL_ (tree->root))                                 \
    return false;                                                             \
                                                                              \
  struct NAME##Remove##_data_ data = { datum, NULL, free_item };              \
  tree->root = NAME##Remove##_ (tree->root, &data);                           \
  return data.deletee != NULL;                                                \
}                                                                             \
                                                                              \
static inline void                                                            \
NAME##Clear##_ (struct aa_node_##NAME##_ *node,                               \
                bool                      free_items)                         \
{                                                                             \
  if (AA_IS_NIL_ (node))                                                      \
    return;                                                                   \
                                                                              \
  NAME##Clear##_ (node->left, free_items);                                    \
  if (free_items)                                                             \
    NAME##FreeDatum##_ (&node->datum);                                        \
  NAME##Clear##_ (node->right, free_items);                                   \
  NAME##Free##_ (node);                                                       \
}                                                                             \
                                                                              \
static inline void                                                            \
NAME##Clear (struct aa_tree_##NAME##_ *tree,                                  \
             bool                      free_items)                            \
{                                                                             \
  ASSERT (tree != NULL);                                                      \
                                                                              \
  if (!tree->root)                                                            \
    return;                                                                   \
                                                                              \
  NAME##Clear##_ (tree->root, free_items);                                    \
  tree->root = NULL;                                                          \
}

#define AA_FOLD(NAME, FUN, TREE, ACCU)                                        \
({                                                                            \
  typedef __typeof (((struct aa_node_##NAME##_ *) 0)->left) _node_type;       \
  typedef __typeof (0 ? (ACCU) :                                              \
                        FUN (&((_node_type) 0)->datum, (ACCU))) _accu_type;   \
  _accu_type _accu = (ACCU);                                                  \
                                                                              \
  inline void                                                                 \
  _fold_fun (_node_type node, _accu_type *accu)                               \
  {                                                                           \
    if (AA_IS_NIL_ (node))                                                    \
      return;                                                                 \
                                                                              \
    _fold_fun (node->left, accu);                                             \
    *accu = FUN (&node->datum, *accu);                                        \
    _fold_fun (node->right, accu);                                            \
  }                                                                           \
                                                                              \
  _fold_fun ((TREE)->root, &_accu);                                           \
  _accu;                                                                      \
})

#define AA_FOR_EACH(NAME, FUN, TREE)                                          \
({                                                                            \
  typedef __typeof (&((struct aa_node_##NAME##_ *) 0)->datum) _datum_type;    \
                                                                              \
  inline void *                                                               \
  _for_each_fun (_datum_type datum, void *aux)                                \
  {                                                                           \
    (void) aux;                                                               \
    FUN (datum);                                                              \
    return NULL;                                                              \
  }                                                                           \
                                                                              \
  AA_FOLD (NAME, _for_each_fun, TREE, NULL);                                  \
  (void) 0;                                                                   \
})

extern  struct aa_node_ AA_NIL_;

#endif /* ifndef AA_TREE_H__ */

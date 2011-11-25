#ifndef AA_TREE_H__
#define AA_TREE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

enum aa_insert_result
{
  AAIR_INSERTED,
  AAIR_EXISTS,
  AAIR_MEMORY_EXHAUSTED
};

#define aa_nop(X) ({ (void)0; })
#define aa_depth(T) (aa_depth_ (AA_TREE_PTR_ (const struct aa_tree_ *, T)))
#define aa_size(T) (aa_size_ (AA_TREE_PTR_ (const struct aa_tree_ *, T)))

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
  struct aa_node_##NAME##_ *left;                                             \
  struct aa_node_##NAME##_ *right;                                            \
  uint32_t                  level;                                            \
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
  result->level = 1;                                                          \
  result->left = result->right = (__typeof (result)) &AA_NIL_;                \
  result->datum = *datum;                                                     \
  return result;                                                              \
}                                                                             \
                                                                              \
struct NAME##Insert##_data_                                                   \
{                                                                             \
  aa_type_##NAME##_     *datum;                                               \
  enum aa_insert_result  result;                                              \
  jmp_buf                bail_out;                                            \
};                                                                            \
                                                                              \
static inline struct aa_node_##NAME##_ *                                      \
NAME##Insert##_ (struct aa_node_##NAME##_    *node,                           \
                 struct NAME##Insert##_data_ *data)                           \
{                                                                             \
  struct aa_node_##NAME##_ *t;                                                \
  if (AA_IS_NIL_ (node))                                                      \
    {                                                                         \
      t = NAME##NewNode_ (data->datum);                                       \
      if (t)                                                                  \
        {                                                                     \
          data->result = AAIR_INSERTED;                                       \
          data->datum = &t->datum;                                            \
          return t;                                                           \
        }                                                                     \
      data->result = AAIR_MEMORY_EXHAUSTED;                                   \
      longjmp (data->bail_out, 1);                                            \
    }                                                                         \
                                                                              \
  int cmp_result = NAME##Cmp_ (data->datum, &node->datum);                    \
  if (cmp_result < 0)                                                         \
    {                                                                         \
      t = NAME##Insert##_ (node->left, data);                                 \
      if (!t)                                                                 \
        return NULL;                                                          \
      node->left = t;                                                         \
    }                                                                         \
  else if (cmp_result > 0)                                                    \
    {                                                                         \
      t = NAME##Insert##_ (node->right, data);                                \
      if (!t)                                                                 \
        return NULL;                                                          \
      node->right = t;                                                        \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      data->result = AAIR_EXISTS;                                             \
      data->datum = &node->datum;                                             \
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
  struct NAME##Insert##_data_ data;                                           \
  memset (&data, 0, sizeof (data));                                           \
  data.datum = *datum;                                                        \
  struct aa_node_##NAME##_ *t = NULL;                                         \
  if (!setjmp (data.bail_out))                                                \
    t = NAME##Insert##_ (tree->root, &data);                                  \
  if (t)                                                                      \
    tree->root = t;                                                           \
  *datum = data.datum;                                                        \
  return data.result;                                                         \
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
  jmp_buf                    bail_out;                                        \
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
  else if (is_bottom && !data->deletee)                                       \
    longjmp (data->bail_out, 1);                                              \
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
  struct NAME##Remove##_data_ data;                                           \
  memset (&data, 0, sizeof (data));                                           \
  data.datum = datum;                                                         \
  data.free_item = free_item;                                                 \
  if (!setjmp (data.bail_out))                                                \
    tree->root = NAME##Remove##_ (tree->root, &data);                         \
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

struct aa_node_
{
  struct aa_node_ *left;
  struct aa_node_ *right;
  uint32_t         level;
};

struct aa_tree_
{
  struct aa_node_ *root;
};

#define AA_ENSURE_(X) ({                                                      \
  typedef char _STATIC_CODE_ANALYSIS_FAILED[0-!(X)];                          \
  (void) 0;                                                                   \
})
#define AA_ENSURE_FIELD_(DT, ST, FIELD)                                       \
({                                                                            \
  typedef __typeof(DT) _dt;                                                   \
  typedef __typeof(ST) _st;                                                   \
  AA_ENSURE_ (sizeof (((_dt *) 0)->FIELD) == sizeof (((_st *) 0)->FIELD));    \
  AA_ENSURE_ ((void *) &((_dt *) 0)->FIELD == (void *) &((_st *) 0)->FIELD);  \
  (void) 0;                                                                   \
})
#define AA_ENSURE_NODE_PTR_(N)                                                \
({                                                                            \
  typedef __typeof(struct aa_node_) _dt;                                      \
  typedef __typeof(*(__typeof(N)) 0)  _st;                                    \
  AA_ENSURE_ (sizeof (_dt) == sizeof (_st) - sizeof (((_st *) 0)->datum));    \
  AA_ENSURE_FIELD_(_dt, _st, left);                                           \
  AA_ENSURE_FIELD_(_dt, _st, right);                                          \
  AA_ENSURE_FIELD_(_dt, _st, level);                                          \
  (void) 0;                                                                   \
})
#define AA_TREE_PTR_(DEST, SRC)                                               \
({                                                                            \
  typedef __typeof(*(__typeof(DEST)) 0) _dt;                                  \
  typedef __typeof(*(__typeof(SRC)) 0)  _st;                                  \
  AA_ENSURE_ (sizeof (_dt) == sizeof (_st));                                  \
  AA_ENSURE_FIELD_ (_dt, _st, root);                                          \
  AA_ENSURE_NODE_PTR_ (((_st *) 0)->root);                                    \
  (_dt *) (SRC);                                                              \
})

extern struct aa_node_ *
aa_inserted_ (struct aa_node_ *t);

extern struct aa_node_ *
aa_removed_ (struct aa_node_ *t);

extern size_t
aa_depth_ (const struct aa_tree_ *tree);

extern size_t
aa_size_ (const struct aa_tree_ *tree);

extern const  struct aa_node_ AA_NIL_;

#define AA_IS_NIL_(N) ((N) == (__typeof (N)) &AA_NIL_)

#endif /* ifndef AA_TREE_H__ */

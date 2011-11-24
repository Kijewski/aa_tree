#include "aa_tree.h"

struct aa_node_ AA_NIL_ = { 0, &AA_NIL_, &AA_NIL_ };

static struct aa_node_ *
skew (struct aa_node_ *t)
{
  if (t->left->level != t->level)
    return t;
  struct aa_node_ *l = t->left;
  t->left = l->right;
  l->right = t;
  return l;
}

static struct aa_node_ *
split (struct aa_node_ *t)
{
  if (t->level != t->right->right->level)
    return t;
  struct aa_node_ *r = t->right;
  t->right = r->left;
  r->left = t;
  ++r->level;
  return r;
}

static inline __attribute__ ((always_inline))
uint32_t min(uint32_t a, uint32_t b)
{
  if (a < b)
    return a;
  return b;
}

static struct aa_node_ *
decrease_level (struct aa_node_ *t)
{
  uint32_t should_be = min (t->left->level, t->right->level) + 1;
  if (should_be < t->level)
    {
      t->level = should_be;
      if (should_be < t->right->level)
        t->right->level = should_be;
    }
  return t;
}

struct aa_node_ *
aa_inserted_ (struct aa_node_ *t)
{
  t = skew (t);
  t = split (t);
  return t;
}

struct aa_node_ *
aa_removed_ (struct aa_node_ *t)
{
  t = decrease_level (t);
  t = skew (t);
  t->right = skew (t->right);
  t->right->right = skew (t->right->right);
  t = split (t);
  t->right = split (t->right);
  return t;
}

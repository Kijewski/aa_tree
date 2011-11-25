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
  if (t->left ->level >= t->level -1 &&
      t->right->level >= t->level -1)
    return t;
    
  --t->level;
  if (t->right->level > t->level)
    t->right->level = t->level;
  t = skew (t);
  t->right = skew (t->right);
  t->right->right = skew (t->right->right);
  t = split (t);
  t->right = split (t->right);
  return t;
}

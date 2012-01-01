/*
 * AUTHOR:  2011, 2012  René Kijewski  (rene.<surname>@fu-berlin.de)
 * LICENSE: MIT
 */

#include "aa_tree.h"

const struct aa_node_ AA_NIL_ = { (struct aa_node_ *) &AA_NIL_,
                                  (struct aa_node_ *) &AA_NIL_,
                                  0
                                };

static struct aa_node_ *
skew (struct aa_node_ *t)
{
  if (AA_IS_NIL_ (t) || t->left->level != t->level)
    return t;
  struct aa_node_ *l = t->left;
  t->left = l->right;
  l->right = t;
  return l;
}

static struct aa_node_ *
split (struct aa_node_ *t)
{
  if (AA_IS_NIL_ (t) || t->level != t->right->right->level)
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
aa_removed_ (struct aa_node_ *t, struct aa_node_ *deletee, bool is_bottom)
{
  if (is_bottom && deletee)
    {
      // ASSERT (AA_IS_NIL_ (t->left));
      struct aa_node_ *result = t->right;
      t->left = deletee->left;
      t->right = deletee->right;
      t->level = deletee->level;
      deletee = t;
      return result;
    }
    
  if (is_bottom || !deletee)
    return t;
    
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

static void
aa_depth_sub (const struct aa_node_ *t, size_t *max, size_t cur)
{
  if (AA_IS_NIL_ (t))
    {
      if (cur > *max)
        *max = cur;
      return;
    }
  aa_depth_sub (t->left,  max, cur+1);
  aa_depth_sub (t->right, max, cur+1);
}

size_t
aa_depth_ (const struct aa_tree_ *tree)
{
  if (!tree->root)
    return 0;
  size_t result = 0;
  aa_depth_sub (tree->root, &result, 0);
  return result;
}

static size_t
aa_size_sub (const struct aa_node_ *t, size_t cur)
{
  if (AA_IS_NIL_ (t))
    return cur;
  cur = aa_size_sub (t->left, cur+1);
  return aa_size_sub (t->right, cur);
}

size_t
aa_size_ (const struct aa_tree_ *tree)
{
  if (!tree->root)
    return 0;
  return aa_size_sub (tree->root, 0);
}

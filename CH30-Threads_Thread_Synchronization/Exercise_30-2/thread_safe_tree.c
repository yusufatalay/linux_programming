#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
  char *key;
  void *value;
  struct Node *left;
  struct Node *right;
  pthread_mutex_t nodeMutex;
} Node;

int initialize(Node *node)
{
  int s = pthread_mutex_init(&node->nodeMutex, NULL);
  if (s != 0)
  {
    perror("pthread_mutex_init");
    return -1;
  }

  s = pthread_mutex_lock(&node->nodeMutex);
  if (s != 0)
  {
    perror("pthread_mutex_lock");
    return -1;
  }

  node->key = NULL;
  node->value = NULL;
  node->left = NULL;
  node->right = NULL;

  s = pthread_mutex_unlock(&node->nodeMutex);
  if (s != 0)
  {
    perror("pthread_mutex_unlock");
    return -1;
  }
  return 0;
}

int add(Node *root, char *key, void *value)
{
  if (key == NULL)
  {
    printf("key cannot be NULL\n");
    return -1;
  }

  int s = pthread_mutex_lock(&root->nodeMutex);
  if (s != 0)
  {
    perror("pthread_mutex_lock");
    return -1;
  }

  if (root->key == NULL)
  {
    root->key = key;
    root->value = value;
    s = pthread_mutex_unlock(&root->nodeMutex);
    if (s != 0)
    {
      perror("pthread_mutex_lock");
      return -1;
    }

    return 0;
  }

  s = strcmp(key, root->key);
  if (s < 0)
  {
    if (root->left == NULL)
    {
      root->left = calloc(1, sizeof(Node));
      if (root->left == NULL)
      {
        perror("calloc");
        return -1;
      }
      initialize(root->left);
    }

    s = pthread_mutex_unlock(&root->nodeMutex);
    if (s != 0)
    {
      perror("pthread_mutex_lock");
      return -1;
    }

    add(root->left, key, value);
  }
  else if (s > 0)
  {

    if (root->right == NULL)
    {
      root->right = calloc(1, sizeof(Node));
      if (root->right == NULL)
      {
        perror("calloc");
        return -1;
      }
      initialize(root->right);
    }

    s = pthread_mutex_unlock(&root->nodeMutex);
    if (s != 0)
    {
      perror("pthread_mutex_lock");
      return -1;
    }

    add(root->right, key, value);
  }
  else
  {

    s = pthread_mutex_unlock(&root->nodeMutex);
    if (s != 0)
    {
      perror("pthread_mutex_lock");
      return -1;
    }

    printf("key's must be unique");
    return -1;
  }

  return 0;
}

// read-only, no need to use mutex since writes are atomic.
bool lookup(Node *root, char *key, void **value)
{

  if (key == NULL || root == NULL || root->key == NULL)
  {
    return false;
  }

  int s = strcmp(root->key, key);

  if (s > 0)
  {
    return lookup(root->left, key, value);
  }
  else if (s < 0)
  {
    return lookup(root->right, key, value);
  }
  else
  {
    value = &root->value; // *value = root->value
    return true;
  }
}
int getParent(Node *root, char *childKey, Node **parent)
{
  if (childKey == NULL || root == NULL || root->key == NULL)
  {
    return -1;
  }

  int s = strcmp(root->key, childKey);

  if (s > 0)
  {
    if (root->left != NULL)
    {
      *parent = root;
    }
    return getParent(root->left, childKey, parent);
  }
  else if (s < 0)
  {
    if (root->right != NULL)
    {
      *parent = root;
    }
    return getParent(root->right, childKey, parent);
  }
  else
  {
    return 0;
  }
}
int getNode(Node *root, char *key, Node **self)
{
  if (key == NULL || root == NULL || root->key == NULL)
  {
    return -1;
  }

  int s = strcmp(root->key, key);

  if (s > 0)
  {
    return getNode(root->left, key, self);
  }
  else if (s < 0)
  {
    return getNode(root->right, key, self);
  }
  else
  {
    *self = root;
    return 0;
  }
}

int getMin(Node *root, Node **res)
{
  if (root == NULL || root->key == NULL)
  {
    return -1;
  }

  Node *tmp;
  for (tmp = root; tmp->left != NULL; tmp = tmp->left)
  {
  }

  *res = tmp;

  return 0;
}

int getMax(Node *root, Node **res)
{

  if (root == NULL || root->key == NULL)
  {
    return -1;
  }

  Node *tmp;
  for (tmp = root; tmp->right != NULL; tmp = tmp->right)
  {
  }

  *res = tmp;

  return 0;
}

int delete(Node *root, char *key)
{
  Node *parent;
  Node *self;
  int s;
  if ((s = getParent(root, key, &parent)) != 0)
  {
    return -1;
  }

  if ((s = getNode(root, key, &self)) != 0)
  {
    return -1;
  }

  if ((s = pthread_mutex_lock(&parent->nodeMutex)) != 0)
  {
    perror("pthread_mutex_lock");
    return -1;
  }
  if (self->left == NULL && self->right == NULL)
  {
    // if node has no children just remove it
    if (strcmp(root->key, key) > 0)
    {
      parent->left = NULL;
    }
    else
    {
      parent->right = NULL;
    }
    free(self);
    if ((s = pthread_mutex_unlock(&parent->nodeMutex)) != 0)
    {
      perror("pthread_mutex_unlock");
      return -1;
    }
    return 0;
  }

  // If the node has a left child, find it's maximum, delete it from the subtree and replace that maximum instead of the given node.
  if (self->left != NULL)
  {
    Node *leftMax;
    if ((s = getMax(self->left, &leftMax)) != 0)
    {
      return -1;
    }
    if (pthread_mutex_lock(&self->nodeMutex) != 0)
    {
      perror("pthread_mutex_lock");
      return -1;
    }
    self->key = leftMax->key;
    self->value = leftMax->value;
    free(leftMax);
    if (pthread_mutex_unlock(&self->nodeMutex) != 0)
    {
      perror("pthread_mutex_unlock");
      return -1;
    }
  }
  else // If the node has the right child, remove the minimum from the right subtree and insert it in place of the node.
  {
    Node *rightMin;
    if ((s = getMin(self->right, &rightMin)) != 0)
    {
      return -1;
    }
    if (pthread_mutex_lock(&self->nodeMutex) != 0)
    {
      perror("pthread_mutex_lock");
      return -1;
    }
    self->key = rightMin->key;
    self->value = rightMin->value;
    free(rightMin);
    if (pthread_mutex_unlock(&self->nodeMutex) != 0)
    {
      perror("pthread_mutex_unlock");
      return -1;
    }
  }

  return 0;
}


#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // 1. nil 노드(=센티넬) 을 위한 메모리 할당 및 초기화
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  // RB Tree 구조체 초기화
  p->nil = nil;
  p->root = nil;  // 처음에는 root 가 nil 노드를 가리켜야 함. (=빈 트리)

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current = t->root;          // 현재 노드를 가리킬 포인터 변수

  while (current != t->nil) {
    if (key < current->key) {         // 찾는 키가 현재 노드의 키보다 작다. -> 왼쪽 자식으로 이동하기
      current = current->left;
    } else if (key > current->key) {  // 찾는 키가 현재 노드의 키보다 크다. -> 오른쪽 자식으로 이동하기
      current = current-> right;
    } else {                          // 키를 찾은 경우
      return current;
    }
  }
  return NULL;  // current 가 nil 이 되어서 종료되었다면 찾는 key를 가진 노드가 없다는 뜻
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

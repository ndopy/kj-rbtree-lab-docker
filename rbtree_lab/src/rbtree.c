#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // nil 노드(=센티넬) 을 위한 메모리 할당 및 초기화
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  // nil 노드의 색상은 반드시 BLACK
  nil->color = RBTREE_BLACK;

  // RB Tree 구조체 초기화
  p->nil = nil;
  p->root = nil;  // 처음에는 root 가 nil 노드를 가리켜야 함. (=빈 트리)

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}


void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;

  // 1. y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 만든다. (입양 보내기)
  x->right = y->left;        // 부모 입장 : 새 자식을 받아들이기
  if (y->left != t->nil) {   // 자식 입장 : 새 부모를 받아들이기
    y->left->parent = x;
  }

  // 2. y를 x의 원래 부모와 연결하기
  y->parent = x->parent;
  if (x->parent == t->nil) {  // x가 root 였다면 -> 이제 y가 새로운 루트이다.
    t->root = y;
  } else if (x == x->parent->left) { // x가 왼쪽 자식인 경우
    x->parent->left = y;
  } else {                           // x가 오른쪽 자식인 경우
    x->parent->right = y;
  }

  // 3. x를 y의 왼쪽 자식으로 만들기
  y->left = x;          // 부모 입장
  x->parent = y;        // 자식 입장
}


void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;

  // 1. y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 만든다. (입양 보내기)
  x->left = y->right;         // 부모 입장 : 새 자식을 받아들이기
  if (y->right != t->nil) {   // 자식 입장 : 새 부모를 받아들이기
    y->right->parent = x;
  }

  // 2. y를 x의 원래 부모와 연결하기
  y->parent = x->parent;
  if (x->parent == t->nil) {  // x가 root 였다면 -> 이제 y가 새로운 루트이다.
    t->root = y;
  } else if (x == x->parent->left) { // x가 왼쪽 자식인 경우
    x->parent->left = y;
  } else {                           // x가 오른쪽 자식인 경우
    x->parent->right = y;
  }

  // 3. x를 y의 오른쪽 자식으로 만들기
  y->right = x;         // 부모 입장
  x->parent = y;        // 자식 입장
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {
    node_t *parent_node = z->parent;                  // 부모 노드
    node_t *grand_parent_node = parent_node->parent;  // 할아버지 노드

    // 부모가 할아버지의 "왼쪽 자식"인 경우
    if (parent_node == grand_parent_node->left) {
      node_t *uncle_node = grand_parent_node->right; // 삼촌은 할아버지의 "오른쪽 자식"

      // case 1 (삼촌이 RED일 경우)
      if (uncle_node->color == RBTREE_RED) {  // 부모와 삼촌은 BLACK, 할아버지는 RED로 변경
        parent_node->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        grand_parent_node->color = RBTREE_RED;
        z = grand_parent_node; // z를 할아버지로 이동시켜서 다시 검사하기
      }
      // case 2, 3 (삼촌이 BLACK일 경우)
      else {
        if (z == parent_node->right) {  // z가 parent 의 오른쪽 자식 -> 꺾인 모양 (Triangle)
          z = parent_node;
          left_rotate(t, z);
          // left_rotate 이후 z의 부모가 바뀌었으므로, parent_node 를 다시 설정
          parent_node = z->parent;
        }
        // z가 parent 의 왼쪽 자식 -> 일직선 모양 (Line)
        parent_node->color = RBTREE_BLACK;
        grand_parent_node->color = RBTREE_RED;
        right_rotate(t, grand_parent_node);
      }
    }
    // 부모가 할아버지의 "오른쪽 자식"인 경우
    else {
      node_t *uncle_node = grand_parent_node->left;  // 삼촌은 할아버지의 "왼쪽 자식"

      // case 1 (삼촌이 RED일 경우)
      if (uncle_node->color == RBTREE_RED) {
        // 부모와 삼촌은 BLACK, 할아버지는 RED로 변경
        parent_node->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        grand_parent_node->color = RBTREE_RED;

        z = grand_parent_node; // z를 할아버지로 이동시켜서 다시 검사하기
      }
      // case 2, 3 (삼촌이 BLACK일 경우)
      else {
        if (z == parent_node->left) {  // z가 parent 의 왼쪽 자식 -> 꺾인 모양 (Triangle)
          z = parent_node;
          right_rotate(t, z);
          // right_rotate 이후 z의 부모가 바뀌었으므로, parent_node 를 다시 설정
          parent_node = z->parent;
        }
        // z가 parent 의 왼쪽 자식 -> 일직선 모양 (Line)
        parent_node->color = RBTREE_BLACK;
        grand_parent_node->color = RBTREE_RED;
        left_rotate(t, grand_parent_node);
      }
    }
  } // end while

  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = malloc(sizeof(node_t));   // 새 노드 생성
  new_node->key = key;

  node_t *traverse_node = t->root;
  node_t *parent_node = t->nil;

  // new_node 를 삽입할 위치 탐색
  while (traverse_node != t->nil) {
    parent_node = traverse_node;

    // 중복을 허용해야하므로, 삽입될 키 >= 현재 노드 키 라면 오른쪽으로 이동하게 한다.
    if (key < traverse_node->key) {           // 찾는 키가 현재 노드의 키보다 작다. -> 왼쪽 자식으로 이동하기
      traverse_node = traverse_node->left;
    } else {    // 찾는 키가 현재 노드의 키보다 크거나 같은 경우
      traverse_node = traverse_node-> right;
    }
  }

  // new_node 의 부모 포인터 설정
  new_node->parent = parent_node;

  // 부모 노드의 자식 포인터 설정
  if (parent_node == t->nil) {  // 트리가 비어 있는 경우
    t->root = new_node;
  } else {
    // 왼쪽 자식으로 넣을지, 오른쪽 자식으로 넣을지 결정해야 한다.
    if (key < parent_node->key) {
      parent_node->left = new_node;
    } else {
      parent_node->right = new_node;
    }
  }

  new_node->left = t->nil;           // new_node 의 자식 포인터들은 nil 노드를 가리켜야 한다.
  new_node->right = t->nil;
  new_node->color = RBTREE_RED;      // 레드-블랙 트리의 규칙에 따라 새로 삽입되는 노드의 색은 RED 이다

  rbtree_insert_fixup(t, new_node);  // 삽입으로 인해 위반될 수 있는 레드-블랙 트리 속성을 복구하기

  return new_node;
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

#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *tree = (rbtree *)calloc(1, sizeof(rbtree));
  if (tree == NULL) {
    return NULL;
  }

  // nil 노드(=센티넬) 을 위한 메모리 할당 및 초기화
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  if (nil == NULL) {
    free(tree);
    return NULL;
  }

  // nil 노드의 색상은 반드시 BLACK
  nil->color = RBTREE_BLACK;

  // RB Tree 구조체 초기화
  tree->nil = nil;
  tree->root = nil;  // 처음에는 root 가 nil 노드를 가리켜야 함. (=빈 트리)

  return tree;
}

void delete_nodes_recursively(node_t *node, node_t *nil) {
  // Base case: 현재 노드가 nil(센티넬) 노드이면 종료
  if (node == nil) {
    return;
  }

  delete_nodes_recursively(node->left, nil);   // 왼쪽 서브트리
  delete_nodes_recursively(node->right, nil);  // 오른쪽 서브트리
  free(node); // 현재 노드 메모리 반환
}

void delete_rbtree(rbtree *t) {
  if (t == NULL) {
    return;
  }

  // 루트 노드부터 모든 노드 삭제하기 (nil 노드는 종료 조건용)
  delete_nodes_recursively(t->root, t->nil);

  // 모든 노드 삭제 후, nil(센티넬) 노드와 rbtree 구조체 메모리 반환하기
  free(t->nil);
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
        }
        // z가 parent 의 왼쪽 자식 -> 일직선 모양 (Line)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
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
        }
        // z가 parent 의 왼쪽 자식 -> 일직선 모양 (Line)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  } // end while

  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = malloc(sizeof(node_t));   // 새 노드 생성

  if (new_node == NULL) {  // malloc 실패 시 NULL 반환
    return NULL;
  }

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
  if (t->root == t->nil) {
    return t->nil; // 빈 트리일 경우, nil 노드 반환
  }

  // 루트에서부터 탐색 시작
  node_t *current = t->root;

  // 왼쪽 자식이 없을 때까지(=nil일 때까지) 계속 왼쪽으로 이동하기
  while (current->left != t->nil) {
    current = current->left;
  }

  return current;
}

node_t *rbtree_max(const rbtree *t) {
  if (t->root == t->nil) {
    return t->nil; // 빈 트리일 경우, nil 노드 반환
  }

  // 루트에서부터 탐색 시작
  node_t *current = t->root;

  // 오른쪽 자식이 없을 때까지(=nil일 때까지) 계속 오른쪽으로 이동하기
  while (current->right != t->nil) {
    current = current->right;
  }

  return current;
}

node_t* get_successor(rbtree *t, node_t *p) {
  node_t *current = p->right;   // 오른쪽으로 한 칸 이동

  while (current->left != t->nil) { // 왼쪽 자녀가 없을 때까지 계속 이동
    current = current->left;
  }

  return current;
}

/**
 * 특정 노드 u를 다른 노드 v로 교체할 때 부모 자식 간의 포인터 관계를 정리하는 함수
 * @param t RB Tree
 * @param u 삭제될 노드
 * @param v 삭제될 노드를 대체할 노드
 */
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  // 1. u의 부모가 v를 가리키도록 설정 (부모 입장)
  if (u->parent == t->nil) {          // u가 root 노드일 경우 -> v가 새로운 root가 된다.
    t->root = v;
  } else if (u == u->parent->left) {  // u가 왼쪽 자식일 경우 -> u의 부모의 왼쪽 자식을 v로 바꾼다.
    u->parent->left = v;
  } else {                            // u가 오른쪽 자식일 경우 -> u의 부모의 오른쪽 자식을 v로 바꾼다.
    u->parent->right = v;
  }

  // 2. v가 u의 부모를 새로운 부모로 가리키도록 설정 (자식 입장)
  v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *x) {
  node_t *sibling;

  while (x != t->root && x->color == RBTREE_BLACK) {
    // Case: x가 왼쪽 자식인 경우
    if (x == x->parent->left) {
      sibling = x->parent->right;

      // Case 1: 형제가 RED인 경우
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;  // 형제는 BLACK으로
        x->parent->color = RBTREE_RED;  // 부모는 RED로
        left_rotate(t, x->parent);      // 부모를 기준으로 좌회전
        sibling = x->parent->right;     // 새로운 형제로 갱신
      }

      // 이제 형제는 반드시 BLACK이다.
      // Case 2: 형제의 두 자식도 BLACK인 경우
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;   // 형제를 RED로 만들고
        x = x->parent;                 // extra black 을 부모에게 넘긴다.
      } else if (sibling->right->color == RBTREE_BLACK) {
        // Case 3: 형제는 BLACK, 형제의 왼쪽 자식은 RED, 오른쪽 자식은 BLACK
        sibling->left->color = RBTREE_BLACK;  // 형제의 왼쪽 자식을 BLACK으로
        sibling->color = RBTREE_RED;          // 형제는 RED로
        right_rotate(t, sibling);             // 형제를 기준으로 우회전
        sibling = x->parent->right;           // 새로운 형제로 갱신
      } else {
        // Case 4: 형제는 BLACK, 형제의 왼쪽 자식은 BLACK, 오른쪽 자식은 RED
        sibling->color = x->parent->color;    // 형제는 부모의 색을 물려받음
        sibling->right->color = RBTREE_BLACK; // 형제의 오른쪽 자식을 BLACK으로
        x->parent->color = RBTREE_BLACK;      // 부모는 BLACK 으로
        left_rotate(t, x->parent);            // 부모를 기준으로 좌회전
        break;                                // 루프 종료
      }
    } else {
      // Case: x가 오른쪽 자식일 경우
      sibling = x->parent->left;

      // Case 1: 형제가 RED인 경우
      if (sibling->color == RBTREE_RED) {
        sibling->color = RBTREE_BLACK;    // 형제는 BLACK으로
        x->parent->color = RBTREE_RED;    // 부모는 RED로
        right_rotate(t, x->parent);       // 부모를 기준으로 우회전
        sibling = x->parent->left;        // 새로운 형제로 갱신
      }

      // 이제 형제는 반드시 BLACK 이다.
      // Case 2: 형제의 두 자식도 모두 BLACK인 경우
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK) {
        sibling->color = RBTREE_RED;   // 형제를 RED로 만들고
        x = x->parent;                 // extra black을 부모에게 넘긴다.
      } else if (sibling->left->color == RBTREE_BLACK) {
        // Case 3: 형제는 BLACK, 형제의 왼쪽 자식은 BLACK, 오른쪽 자식은 RED
        sibling->right->color = RBTREE_BLACK;  // 형제의 오른쪽 자식을 BLACK으로
        sibling->color = RBTREE_RED;           // 형제는 RED로
        left_rotate(t, sibling);               // 형제를 기준으로 좌회전
        sibling = x->parent->left;             // 새로운 형제로 갱신
      } else {
        // Case 4: 형제는 BLACK, 형제의 왼쪽 자식은 RED, 오른쪽 자식은 BLACK
        sibling->color = x->parent->color;     // 형제는 부모의 색을 물려받음
        sibling->left->color = RBTREE_BLACK;   // 형제의 왼쪽 자식을 BLACK으로
        x->parent->color = RBTREE_BLACK;       // 부모는 BLACK으로
        right_rotate(t, x->parent);            // 부모를 기준으로 우회전
        break;                                 // 루프 종료
      }
    }
  } // end while

  // red-and-black 노드 처리 및 루트는 BLACK이어야 한다는 속성 처리
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // 1. 물리적으로 삭제될 노드와 그 색상을 결정
  node_t *y = p;
  color_t y_original_color = y->color;
  char has_two_children = (p->left != t->nil && p->right != t->nil);

  if (has_two_children) {                     // 삭제될 노드의 자녀가 2개 : 삭제되는 노드의 Successor 의 색
    y = get_successor(t, p);                  // successor 찾기
    y_original_color = y->color;              // 삭제되는 색상 = successor 의 색상
  }

  // 2. y를 대체할 자식 노드(x) 찾기
  // x는 y의 자식들 중 nil이 아닌 노드이거나 nil일 수 있다.
  node_t *x = (y->left != t->nil) ? y->left : y->right;

  // 3. y를 트리에서 떼어내기 (transplant)
  // y의 부모와 x를 연결하기
  rbtree_transplant(t, y, x);

  // 4. 만약 p의 자녀가 2개였다면 p의 key 값을 y(=successor)의 값으로 업데이트
  if (y != p) {
    p->key = y->key;
  }

  // 5. BLACK 노드가 삭제되었다면 fixup 호출
  if (y_original_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, x);
  }

  // 삭제한 노드의 메모리 해제
  free(y);

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {

  return 0;
}

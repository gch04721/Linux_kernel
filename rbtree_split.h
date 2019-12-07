#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>

#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/time.h>

struct rb_split_node{
    struct rb_node node;
    int value; // sha1 hash value
};

struct rb_split_root{
    int thread_num;
    // tree들의 level정보
    int current_tree_level;

    // arrays
    // tree들에 대한 정보를 가지는 array
    struct rb_root* list_rb_root;
    // tree들의 node수
    int* list_tree_node_num;
    // 각 tree들의 min, max정보, index 0: min, 1: max
    // min, max : leaf rb_split_node 정보 저장`
    struct rb_split_node*** list_min_max;
    // 각 tree가 level의 node를 전부 채웠는지
    int* list_tree_isFull;
};

// tree 초기화, 생성할 tree수를 설정한다. 
extern void rb_split_init(struct rb_split_root* tree);

// rb_node에 해당하는 rb_split_node를 얻어오기 위한 함수
extern struct rb_split_node* rb_split_get_node(struct rb_node* _node);

// 새로운 node를 삽입하기 위한 함수
extern int rb_split_insert(struct rb_split_root* tree, void* node);
    // tree에 새로운 node를 삽입하고 coloring 진행
    int rb_split_update_tree(struct rb_split_root* tree, int index, struct rb_split_node* data);
    // min, max list update
    void rb_split_update_minmax(struct rb_split_root* tree, int index);
    // 현재 삽입한 트리가 완전한 형태인지 검사
    void rb_split_update_isFull(struct rb_split_root* tree, int index);
    // 생성된 tree중 특정 tree만 node가 쏠리는 현상을 방지하기 위함
    //void rb_split_balancing(struct rb_split_root* tree, int index);
    // leftmost child 구하기
    struct rb_split_node* rb_split_get_leftmost(struct rb_root tree, int level);
    // rightmost child 구하기
    struct rb_split_node* rb_split_get_rightmost(struct rb_root tree, int level);
    // node를 바꾸는 것이 아닌 node의 value를 변경
    void rb_split_swap_node(struct rb_split_node* node1, struct rb_split_node* node2);
    // tree에서 node를 제거하고 새로운 node를 삽입, tree의 형태는 유지됨.
    struct rb_split_node* rb_split_change_node(struct rb_split_root* tree, int index, struct rb_split_node* org, struct rb_split_node* new);

// 원하는 값에 해당하는 node를 탐색
extern struct rb_split_node* rb_split_search(struct rb_split_root* tree, int value);

// tree를 전체적으로 출력
void rb_split_print(struct rb_split_root* tree);
void rb_split__print(struct rb_node* node, int depth, int left_right);

// kfree

// 사용자가 정의 x, 정해진 대로 지우는 작업을 수행
extern void rb_split_erase(struct rb_split_root* tree, int value);
    void rb_split_erase_node(struct rb_split_root* tree, int index, struct rb_split_node* _node);

void rb_split_delete_tree(struct rb_split_root* tree);
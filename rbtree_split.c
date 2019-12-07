#include "rbtree_split.h"

#include <linux/time.h>

#define FALSE 0
#define TRUE 1

struct my_type{
    struct rb_node node;
    int value;
};

int rb_insert(struct rb_root* root, struct my_type *data){
    struct rb_node **new = &(root->rb_node), *parent = NULL;
    while(*new){
        struct my_type *this = container_of(*new, struct my_type, node);
        parent = *new;
        if(this->value > data->value)
            new = &((*new)->rb_left);
        else if(this->value < data->value)
            new = &((*new)->rb_right);
        else
        {
                return FALSE;
        }
        
    }
    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);

    return TRUE;
}

struct my_type* rb_search(struct rb_root* root, int value){
    struct rb_node *node = root->rb_node;
    while(node){
        struct my_type *data = container_of(node, struct my_type, node);
        if(data->value > value)
            node = node->rb_left;
        else if(data->value < value)
            node = node->rb_right;
        else
            return data;
    }
    return NULL;
}

int rb_delete(struct rb_root* tree, int value){
    struct my_type* data = rb_search(tree, value);
    if(data){
        rb_erase(&data->node, tree);
        kfree(data);
    }
}

void rb_print(struct rb_node* _node, int depth, int left_right){
    struct my_type* data = container_of(_node, struct my_type, node);
    printk("[%d, %d], value : %d\n", depth, left_right, data->value);
    if(_node->rb_left)
        rb_print(_node->rb_left, depth + 1, 0);
    if(_node->rb_right)
        rb_print(_node->rb_right, depth + 1, 1);
}

void rb_split_print__(struct rb_node* _node, int depth, int left_right){
    printk("[%d, %d]\n", depth, left_right);
    if(_node->rb_left)
        rb_split_print__(_node->rb_left, depth + 1, 0);
    if(_node->rb_right)
        rb_split_print__(_node->rb_right, depth +1, 1);
}


void rb_split_print_nodeStatus(struct rb_split_root* tree){
    int i=0;
    for(;i<tree->thread_num; i++){
        printk("%d tree\n", i);
        struct rb_root sub_tree = tree->list_rb_root[i];
        rb_split_print__(sub_tree.rb_node, 0, 0);
    }
}


// test용 함수
void rb_split_test(void);

int __init rbtree_split_module_init(void){
    printk("module start \n");
    rb_split_test();
    return 0;
}

void __exit rbtree_split_module_cleanup(void){
    printk("end\n");
}

module_init(rbtree_split_module_init);
module_exit(rbtree_split_module_cleanup);
MODULE_LICENSE("GPL");

void rb_split_test(void){
    int input_size = 100000;
    // split_tree test
    struct rb_split_root* my_tree = kmalloc(sizeof(struct rb_split_root), GFP_KERNEL);
    rb_split_init(my_tree);

    printk("insert start \n");
    ktime_t start, end;
    s64 actual_time;
    int i=0, ret;
    start = ktime_get();
    for(i = 0; i<10000; i++){
        struct rb_split_node* new_node = kmalloc(sizeof(struct rb_split_node), GFP_KERNEL);
        if(!new_node)
            return NULL;
        new_node->value = 3 * (i + 1) - (i % 3);

        ret = rb_split_insert(my_tree, (void*)new_node);
        
    }
    end = ktime_get();
    actual_time = ktime_to_ns(ktime_sub(end, start));
    //rb_split_print(my_tree);
    //rb_split_print_nodeStatus(my_tree);
    printk("insert end %lld time\n", (long long)actual_time);

    int search_num = 1200;
    printk("search test, test %d\n", search_num);
    struct rb_split_node* result;
    
    // time checking
    start = ktime_get();
    result = rb_split_search(my_tree, search_num);
    end = ktime_get();
    actual_time = ktime_to_ns(ktime_sub(end, start));
    // time check end
    if(result){
        printk("serach num = %d, result num = %d\n", search_num, result->value);
    }
    printk("search end, %lld time\n", (long long)actual_time);

    rb_split_delete_tree(my_tree);
    printk("rbtree_split test end\n");
    // rbtree test

    struct rb_root root_test = RB_ROOT;
    start = ktime_get();
    for(i =0; i<10000; i++){
        struct my_type* new = kmalloc(sizeof(struct my_type), GFP_KERNEL);
        if(!new)
            return NULL;
        new->value = i +1;
        if(i % 4 == 0 && i > 0){
            ret = rb_delete(&root_test, i -2);
            struct my_type* test = kmalloc(sizeof(struct my_type), GFP_KERNEL);
            test->value = i - 2;
            ret = rb_insert(&root_test, test);
        }

        ret = rb_insert(&root_test, new);
    }
    end = ktime_get();
    actual_time = ktime_to_ns(ktime_sub(end, start));
    //rb_print(root_test.rb_node, 0, 0);
    printk("rbtree insert end, %lld time \n", (long long)actual_time);
    

    start = ktime_get();
    struct my_type* _result = rb_search(&root_test, search_num);
    end = ktime_get();
    actual_time = ktime_to_ns(ktime_sub(end, start));
    if(result ){
        printk("search num =%d, result num = %d\n", search_num, _result->value);
    }
    printk("search end, %lld time\n", (long long)actual_time);

    printk("rbtree test end\n");

}

void rb_split__print(struct rb_node* _node, int depth, int left_right){
    struct rb_split_node* data = rb_split_get_node(_node);
    printk("[%d, %d], value : %d\n",depth, left_right, data->value);
    if(_node->rb_left){
        rb_split__print(_node->rb_left, depth + 1, 0);
    }
    if(_node->rb_right){
        rb_split__print(_node->rb_right, depth + 1, 1);
    }
}

void rb_split_print(struct rb_split_root* tree){
    int i=0;
    for(;i<tree->thread_num; i++){
        printk("%d tree\n", i);
        struct rb_root sub_tree = tree->list_rb_root[i];
        rb_split__print(sub_tree.rb_node, 0, 0);
    }
}

void rb_split_init(struct rb_split_root* tree){
    tree->thread_num = 3;
    tree->current_tree_level = 1;
    tree->list_rb_root = kmalloc(tree->thread_num * sizeof(struct rb_root), GFP_KERNEL);
    tree->list_tree_node_num = kmalloc(tree->thread_num * sizeof(int), GFP_KERNEL);
    tree->list_min_max = kmalloc(tree->thread_num * sizeof(struct rb_split_node**), GFP_KERNEL);
    tree->list_tree_isFull = kmalloc(tree->thread_num * sizeof(bool), GFP_KERNEL);

    int i=0;
    struct rb_split_node* dummy = kmalloc(sizeof(struct rb_split_node), GFP_KERNEL);
    for(;i<tree->thread_num; i++){
        tree->list_min_max[i] = kmalloc(2 * sizeof(struct rb_split_node*), GFP_KERNEL);

        tree->list_tree_isFull[i] = FALSE;

        tree->list_min_max[i][0] = dummy;
        tree->list_min_max[i][1] = dummy;

        tree->list_tree_node_num[i] = 0;

        tree->list_rb_root[i] = RB_ROOT;
    }
}

void rb_split_swap_node(struct rb_split_node* node1, struct rb_split_node* node2){
    // node가 아닌 node의 value 변경
    int tmp = node1->value;
    node1->value = node2->value;
    node2->value = tmp;
}

struct rb_split_node* rb_split_get_node(struct rb_node* _node){
    return container_of(_node, struct rb_split_node, node);
}

int rb_split_insert(struct rb_split_root* tree, void* data){
    // case 0, level 1이 완성되지 않음
    if(tree->current_tree_level == 1){
        int i=0, ret;
        for(;i<tree->thread_num; i++){
            // left tree부터 값을 넣기 시작
            if(tree->list_tree_isFull[i] == FALSE){
                ret = rb_split_update_tree(tree, i, (struct rb_split_node*)data);
                if(i == tree->thread_num -1){
                    // return에 걸리지 않고 나옴 == 모든 tree가 채워짐
                    /*=====================balancing===============================*/
                    int j,k;
                    for(j = 0; j<(tree->thread_num) - 1; j++){
                        struct rb_split_node* data1 = rb_split_get_node(tree->list_rb_root[j].rb_node);
                        for(k = j + 1; k<tree->thread_num; k++){  
                            //printk("%d, %d\n",j,k);  
                            struct rb_split_node* data2 = rb_split_get_node(tree->list_rb_root[k].rb_node);
                            if(data1->value > data2->value){
                                //printk("test, (%d,%d) : (%d,%d)\n",j,k,data1->value, data2->value);
                                rb_split_swap_node(data1, data2);
                                //printk("(%d, %d)\n", data1->value, data2->value);
                            }
                        }
                    }                   
                    for(i=0; i<tree->thread_num; i++){
                        //printk("before : %d, %d\n", tree->list_min_max[i][0]->value, tree->list_min_max[i][1]->value);
                        rb_split_update_minmax(tree, i);
                        //printk("after : %d, %d\n", tree->list_min_max[i][0]->value, tree->list_min_max[i][1]->value);
                    }
                    /*==============================================================*/
                    for(i=0; i<tree->thread_num; i++)
                        tree->list_tree_isFull[i] = FALSE;
                    tree->current_tree_level = 2;
                }   
                return ret;
            }
        }   
    }
    
    if(tree->current_tree_level > 1){
        struct rb_split_node* input_data = (struct rb_split_node*)data;
        int set_index = 0;
        if(input_data->value < tree->list_min_max[1][0]->value){
            // left tree
            set_index = 0;
        }
        else if(input_data->value > tree->list_min_max[1][1]->value){
            // right tree
            set_index = 2;
        }
        else{
            // center tree
            set_index = 1;
        }

        if(tree->list_tree_isFull[1] == FALSE){
            int ret;
            if(set_index == 0){
                //rb_split_swap_node(tree->list_min_max[set_index][1], tree->list_min_max[1][0]);
                input_data = rb_split_change_node(tree, set_index, tree->list_min_max[set_index][1], input_data);
                
            }
            else if(set_index == 2){
                //rb_split_swap_node(tree->list_min_max[set_index][0], tree->list_min_max[1][1]);
                input_data = rb_split_change_node(tree, set_index, tree->list_min_max[set_index][0], input_data);
                
            }
            ret = rb_split_update_tree(tree, 1, input_data);
            return ret;
        }   
        else{
            int tmp_sum = 0, i =0;
            for (; i<tree->thread_num; i++){
                tmp_sum += (tree->list_tree_isFull[i]);
            }

            if(tmp_sum == 1){
            // L, R is not Full
                if(set_index == 1){
                    struct rb_split_node* root_data = rb_split_get_node(tree->list_rb_root[set_index].rb_node);
                    if(input_data->value < root_data->value){
                        set_index = 0;
                    }
                    else if(input_data->value < root_data->value){
                        set_index = 2;
                    }
                    if(set_index == 0){
                        input_data = rb_split_change_node(tree, 1, tree->list_min_max[1][0], input_data);
                    }
                    else if(set_index == 2){
                        input_data = rb_split_change_node(tree, 1, tree->list_min_max[1][1], input_data);
                    }
                }
                int ret;
                ret = rb_split_update_tree(tree, set_index, input_data);
                return ret;
            }
            else if(tmp_sum == 2){
                if(tree->list_tree_isFull[0] == FALSE){
                    // L is not Full
                    if(set_index == 1 || set_index == 2){
                        struct rb_split_node* node_tmp = kmalloc(sizeof(struct rb_split_node), GFP_KERNEL);
                        int i;
                        for(i=2;i>=1; i--){
                            input_data = rb_split_change_node(tree, i, tree->list_min_max[i][0], input_data);
                            
                        }
                        set_index = 0;
                    }
                }
                else if(tree->list_tree_isFull[2] == FALSE){
                    // R is not Full
                    if(set_index == 0 || set_index == 1){
                        struct rb_split_node* node_tmp = kmalloc(sizeof(struct rb_split_node), GFP_KERNEL);
                        int i;
                        for(i=0; i< 2;i++){
                            input_data = rb_split_change_node(tree, i, tree->list_min_max[i][1], input_data);
                            
                        }
                        set_index = 2;
                    }
                }

                int ret;
                ret = rb_split_update_tree(tree, set_index, input_data);
                return ret;

            }
            else if(tmp_sum ==3){
                // L, R is Full
                int i=0;
                for(; i<tree->thread_num; i++)
                    tree->list_tree_isFull[i] = FALSE;
                tree->current_tree_level += 1;
                
                rb_split_insert(tree, (void*)input_data);
            }
        }
        
    }
}

int rb_split_update_tree(struct rb_split_root* tree, int index, struct rb_split_node* data){
    // 선택된 tree에 노드 생성, 값을 넣는다.
    struct rb_root* sub_tree = &(tree->list_rb_root[index]);
    struct rb_node **new = &(sub_tree->rb_node), *parent = NULL;
    while(*new){
        struct rb_split_node *this = rb_split_get_node(*new);

        parent = *new;
        if(this->value > data->value){
            new = &((*new)->rb_left);
        }
        else if(this->value < data->value){
            new = &((*new)->rb_right);
        }
        else 
            return FALSE;
    }

    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, sub_tree);

    tree->list_tree_node_num[index] += 1;
    rb_split_update_isFull(tree, index);
    rb_split_update_minmax(tree, index);
    return TRUE;
}

void rb_split_update_minmax(struct rb_split_root* tree, int index){
    struct rb_split_node* data_left = 
        rb_split_get_leftmost(tree->list_rb_root[index], tree->current_tree_level);
    struct rb_split_node* data_right = 
        rb_split_get_rightmost(tree->list_rb_root[index], tree->current_tree_level);
    
    tree->list_min_max[index][0] = data_left;
    tree->list_min_max[index][1] = data_right;
}

void rb_split_update_isFull(struct rb_split_root* tree, int index){
    int level = tree->current_tree_level;
    int node_num = tree->list_tree_node_num[index];

    int i=0;
    for(;i<level; i++){
        if((node_num >> i) & 1)
            continue;
        else
            return;
    }
    tree->list_tree_isFull[index] = TRUE;
}

struct rb_split_node* rb_split_get_leftmost(struct rb_root tree, int level){
    if(level == 1){
        if(tree.rb_node){
            struct rb_split_node *data = rb_split_get_node(tree.rb_node);
            return data;
        }
        else{
            return NULL;
        }
    }
    else{
        int i=0;
        struct rb_node* node = tree.rb_node;
        for(; i<level; i++){
            if(node->rb_left)
                node = node->rb_left;
        }
        struct rb_split_node *data = rb_split_get_node(node);
        return data;
    }
}

struct rb_split_node* rb_split_get_rightmost(struct rb_root tree, int level){
    if(level == 1){
        if(tree.rb_node){
            struct rb_split_node* data = rb_split_get_node(tree.rb_node);
            return data;
        }
        else
        {
            return NULL;
        }
    }
    else{
        int i=0;
        struct rb_node* node = tree.rb_node;
        for(; i<level; i++){
            if(node->rb_right)
                node = node->rb_right;
        }
        struct rb_split_node *data = rb_split_get_node(node);
        return data;
    }
}

struct rb_split_node* rb_split_search(struct rb_split_root* tree, int value){
    int index = 0;
    if(value < tree->list_min_max[1][0]->value)
        index = 0;
    else if(value < tree->list_min_max[1][1]->value)
        index = 1;
    else 
        index = 2;

    struct rb_node* _node = (tree->list_rb_root[index].rb_node);
    while(_node){
        struct rb_split_node* data = rb_split_get_node(_node);
        if(data->value < value){
            _node = _node->rb_right;
        }
        else if(data->value > value){
            _node = _node->rb_left;
        }
        else
        {
            return data;
        }
    }
    return NULL;
}

struct rb_split_node* rb_split_change_node(struct rb_split_root* tree, int index, struct rb_split_node* org, struct rb_split_node* new){
    // dst node를 index에 해당하는 tree에 삽입
    // src노드는 
    struct rb_split_node* node_tmp = kmalloc(sizeof(struct rb_split_node), GFP_KERNEL);
    node_tmp->value = org->value;
    rb_split_erase_node(tree, index, org);
    rb_split_update_tree(tree, index, new);
    return node_tmp;
}

void rb_split_erase(struct rb_split_root* tree, int value){
    struct rb_split_node* data = rb_split_search(tree, value);
    int index =0;
    if(value < tree->list_min_max[1][0]->value)
        index = 0;
    else if(value < tree->list_min_max[1][1]->value)
        index = 1;
    else 
        index = 2;

    rb_split_erase_node(tree, index, data);
}

void rb_split_erase_node(struct rb_split_root* tree, int index, struct rb_split_node* _node){  
    rb_erase(&(_node->node), &(tree->list_rb_root[index]));
    kfree(_node);
    tree->list_tree_node_num[index] -= 1;
    rb_split_update_isFull(tree, index);
}


void rb_split_delete_tree(struct rb_split_root* tree){
    kfree(tree->list_tree_node_num);
    kfree(tree->list_tree_isFull);
    kfree(tree->list_rb_root);
    kfree(tree->list_min_max);
}
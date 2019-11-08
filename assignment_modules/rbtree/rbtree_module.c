#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h> // kmalloc
#include <linux/time.h>

#define FALSE 0
#define TRUE 1

struct my_node{
	struct rb_node node;
	int key;
	int value;
};

int rb_insert(struct rb_root* root, struct my_node *data){
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while(*new){
		struct my_node *this = container_of(*new, struct my_node, node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if(this->key < data->key)
			new = &((*new)->rb_right);
		else 
			return FALSE;
	}

	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return TRUE;
}

struct my_node *rb_search(struct rb_root* root, int key){
	struct rb_node *node = root->rb_node;
	while(node){
		struct my_node *data = container_of(node, struct my_node, node);

		if(data->key > key)
			node = node->rb_left;
		else if(data->key < key)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

int rb_delete(struct rb_root* mytree, int key){
	struct my_node *data = rb_search(mytree, key);

	if(data){
		rb_erase(&data->node, mytree);
		kfree(data);
	}
}

int insertItems(struct rb_root* rbtree, int num){
	ktime_t start, end;
	s64 actual_time;
	int i=0, ret;

	start = ktime_get();
	for(i=0; i<num;i++){
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		if(!new)
			return NULL;
		new->value = i;
		new->key = i;

		ret = rb_insert(rbtree, new);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("%d element insert : %lld\n", num, (long long)actual_time);
	
	return ret;
}

void searchItems(struct rb_root* rbtree, int num){
	struct my_node *node;
	ktime_t start, end;
	s64 actual_time;
	int i =0;

	start = ktime_get();
	for(i=0; i<num; i++){
		node = rb_search(rbtree, i);
		if(!node){
			return NULL;
		}
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("%d element search : %lld\n", num, (long long)actual_time);
}

void deleteItems(struct rb_root* rbtree, int num){
	ktime_t start, end;
	s64 actual_time;
	int i=0;
	start = ktime_get();
	for(i=0; i<num; i++){
		rb_delete(rbtree, i);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("%d element delete : %lld\n", num, (long long)actual_time);
}

void struct_example(void){
	struct rb_root mytree_1000 = RB_ROOT;
	struct rb_root mytree_10000 = RB_ROOT;
	struct rb_root mytree_100000 = RB_ROOT;

	ktime_t start, end;
	s64 actual_time;
	int i=0, ret;

	/* rb_node create & insert */
	// insert 1000 items
	ret = insertItems(&mytree_1000, 1000);
	// insert 10000 items
	ret = insertItems(&mytree_10000, 10000);
	//insert 100000
	ret = insertItems(&mytree_100000, 100000);

	/* rbtree search every element */
	// search 1000 items
	searchItems(&mytree_1000, 1000);
	// search 10000 items
	searchItems(&mytree_10000, 10000);
	// search 100000 items
	searchItems(&mytree_100000, 100000);

	/* rbtree delete n nodes */
	// delete 1000 items
	deleteItems(&mytree_1000, 1000);
	// delete 10000 items
	deleteItems(&mytree_10000, 10000);
	// delete 100000 items
	deleteItems(&mytree_100000, 100000);
}

int __init rbtree_module_init(void){
	printk("module init\n");
	struct_example();
	return 0;
}

void __exit rbtree_module_cleanup(void){
	printk("end\n");
}

module_init(rbtree_module_init);
module_exit(rbtree_module_cleanup);
MODULE_LICENSE("GPL");

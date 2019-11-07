#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h> // kmalloc
#include <linux/time.h>

struct my_node{
	struct list_head list;
	int data;
};
void struct_example(void);
int __init link_module_init(void){
	struct_example();
	return 0;
}

void __exit link_module_cleanup(void){
	printk("end\n");
}

module_init(link_module_init);
module_exit(link_module_cleanup);
MODULE_LICENSE("GPL");

void struct_example(void){
	struct list_head my_list_1000;
	struct list_head my_list_10000;
	struct list_head my_list_100000;
	ktime_t start, end;
	s64 actual_time;
	// init list
	INIT_LIST_HEAD(&my_list_1000);
	INIT_LIST_HEAD(&my_list_10000);
	INIT_LIST_HEAD(&my_list_100000);

	// insert 1000 element

	printk("1000 element insert : ");
	int i;
	start = ktime_get();
	for(i=0; i<1000; i++){
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->list, &my_list_1000);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("%lld\n", (long long)actual_time);

	printk("10000 element insert : ");
	//int i;
	start = ktime_get();
	for(i=0; i<10000; i++){
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->list, &my_list_10000);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("%lld\n", (long long)actual_time);


	printk("100000 element insert : ");
	//int i;
	start = ktime_get();
	for(i=0; i<100000; i++){
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->list, &my_list_100000);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("%lld\n", (long long)actual_time);

	struct my_node *current_node;
	struct list_head *tmp;
	struct list_head *p;

	start = ktime_get();
	list_for_each(p, &my_list_1000){

	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("serach 1000 element : %lld\n", (long long)actual_time);

	start = ktime_get();
	list_for_each(p, &my_list_10000){

	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("serach 10000 element : %lld\n", (long long)actual_time);

	start = ktime_get();
	list_for_each(p, &my_list_100000){

	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("serach 100000 element : %lld\n", (long long)actual_time);

	// del 1000
	start = ktime_get();
	list_for_each_safe(p, tmp, &my_list_1000){
		current_node = list_entry(p, struct my_node, list);
		list_del(&current_node->list);
		kfree(current_node);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("del 1000 element : %lld\n", (long long)actual_time);

	// del 10000
	start = ktime_get();
	list_for_each_safe(p, tmp, &my_list_10000){
		current_node = list_entry(p, struct my_node, list);
		list_del(&current_node->list);
		kfree(current_node);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("del 10000 element : %lld\n", (long long)actual_time);

	// del 100000
	start = ktime_get();
	list_for_each_safe(p, tmp, &my_list_100000){
		current_node = list_entry(p, struct my_node, list);
		list_del(&current_node->list);
		kfree(current_node);
	}
	end = ktime_get();
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("del 100000 element : %lld\n", (long long)actual_time);
}
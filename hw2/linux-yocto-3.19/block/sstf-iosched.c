/*
 * elevator noop
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
};

static sector_t prev_sector;

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

//Here is where the LOOK algorithm is implemented
static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	struct request *prev_rq = NULL;
	int temp = -1;
	int prev = -1;
	sector_t sec;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
			list_for_each_entry(rq, &nd->queue, queuelist) {
				if (rq == 0) {
					printk("Error: Queue empty :(\n");
					return 0;
				}
				//Determine direction
				sec = blk_rq_pos(rq);
        			if (prev_sector >= sec) {
        				temp = prev_sector - sec;
        			} else {
        				temp = sec - prev_sector;
       				}
		        	sec = blk_rq_pos(rq);
				if (prev > temp || prev == -1) {
					prev = temp;
					prev_rq = rq;
				}
			}
		prev_sector = blk_rq_sectors(prev_rq) + blk_rq_pos(prev_rq);
		//Merge
                list_del_init(&prev_rq->queuelist);
                elv_dispatch_add_tail(q, prev_rq); 

		printk("Popping request at sector position: %llu\n",
					(unsigned long long)blk_rq_pos(prev_rq));
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	printk("Adding request at sector position: %llu\n",
			(unsigned long long)blk_rq_pos(rq));

	list_add_tail(&rq->queuelist, &nd->queue);
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("Austin Brown - 13");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");

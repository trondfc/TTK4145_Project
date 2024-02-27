#include "../orderQueue.h"
#include <assert.h>

void test_enqueue_order(order_queue_t * queue){
    order_event_t order;
    order.order_id = 1;

    order_event_t order1;
    order1.order_id = 2;

    order_event_t order2;
    order2.order_id = 3;

    queue->capacity = 10;
    queue->size = 0;
    enqueue_order(queue, &order);
    printf("order IDs: %d, %d, %d\n", queue->orders[0].order_id, queue->orders[1].order_id, queue->orders[2].order_id);
    enqueue_order(queue, &order1);
    printf("order IDs: %d, %d, %d\n", queue->orders[0].order_id, queue->orders[1].order_id, queue->orders[2].order_id);
    enqueue_order(queue, &order2);
    printf("order IDs: %d, %d, %d\n", queue->orders[0].order_id, queue->orders[1].order_id, queue->orders[2].order_id);

    printf("Queue size: %d\n", queue->size);
    assert(queue->size == 3);
    assert(queue->orders[0].order_id == 1);
    assert(queue->orders[1].order_id == 2);
    assert(queue->orders[2].order_id == 3);
}

void test_dequeue_order(order_queue_t * queue){
    order_event_t order;
    order.order_id = 1;

    queue->capacity = 10;
    queue->size = 1;
    dequeue_order(queue, &order);
    dequeue_order(queue, &order);
    assert(queue->size == 0);
    assert(queue->orders[0].order_id != 1);
}

void test_enq_dq_order(order_queue_t * queue){
    order_event_t order;
    order.order_id = 1;

    order_event_t order1;
    order1.order_id = 2;

    order_event_t order2;
    order2.order_id = 3;

    queue->capacity = 10;
    queue->size = 0;
    enqueue_order(queue, &order);
    enqueue_order(queue, &order1);
    enqueue_order(queue, &order2);

    printf("Queue size: %d\n", queue->size);
    assert(queue->size == 3);
    assert(queue->orders[0].order_id == 1);
    assert(queue->orders[1].order_id == 2);
    assert(queue->orders[2].order_id == 3);
    printf("enqueued order IDs: %d, %d, %d\n", queue->orders[0].order_id, queue->orders[1].order_id, queue->orders[2].order_id);

    printf("Testing enqueue of existing order\n");
    enqueue_order(queue, &order);
    assert(queue->size == 3);
    printf("Queue size: %d\n", queue->size);

    dequeue_order(queue, &order);
    dequeue_order(queue, &order1);
    assert(queue->size == 1);
    assert(queue->orders[0].order_id == 3);
    printf("Post dequeue order IDs: %d\n", queue->orders[0].order_id);
}

int main(){
    order_queue_t * queue = create_order_queue(10);
    printf("Allocated size of queue: %d\n", queue->capacity);
    
    test_enq_dq_order(queue);

    printf("All tests passed\n");
    return 0;
}
#include "../orderQueue.h"
#include <assert.h>

void test_enqueue_order(){
    order_queue_t queue;
    order_event_t order;
    order.order_id = 1;
    queue.capacity = 10;
    queue.size = 0;
    enqueue_order(&queue, &order);
    assert(queue.size == 1);
    assert(queue.orders[0].order_id == 1);
}

void test_dequeue_order(){
    order_queue_t queue;
    order_event_t order;
    order.order_id = 1;
    queue.capacity = 10;
    queue.size = 1;
    queue.orders[0] = order;
    dequeue_order(&queue, &order);
    assert(queue.size == 0);
}

int main(){
    test_enqueue_order();
    test_dequeue_order();
    printf("All tests passed\n");
    return 0;
}
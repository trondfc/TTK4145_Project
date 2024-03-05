#include "../inc/order_queue/orderQueue.h"
#include "../inc/sverresnetwork/sverresnetwork.h"
#include "../inc/order_queue/send_order_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

void messageReceived(const char * ip, char * data, int datalength){

  printf("Received message from %s: '%s'\n",ip,data);
}

void connectionStatus(const char * ip, int status){

  printf("A connection got updated %s: %d\n",ip,status);
}

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

void test_create_order_queue(){
    order_queue_t * queue = create_order_queue(10);
    assert(queue->capacity == 10);
    assert(queue->size == 0);
}

int main(){
    //tcp_init(messageReceived,connectionStatus);
    send_order_queue_init(messageReceived, connectionStatus);
    test_create_order_queue();

    order_queue_t * queue = create_order_queue(10);
    printf("Allocated size of queue: %d\n", queue->capacity);
    
    /* Generate different orders */

    order_event_t order;
    order.elevator_id = 1;
    order.floor = 2;
    order.order_type = GO_TO;
    order.order_id = GenerateOrderID(&order);
    printf("Order 1 ID: %d\n", order.order_id);

    order_event_t order2;
    order2.elevator_id = 1;
    order2.floor = 2;
    order2.order_type = DOWN_FROM;
    order2.order_id = GenerateOrderID(&order2);
    printf("Order 2 ID: %d\n", order2.order_id);

    order_event_t order3;
    order3.elevator_id = 2;
    order3.floor = 3;
    order3.order_type = DOWN_FROM;
    order3.order_id = GenerateOrderID(&order3);
    printf("Order 3 ID: %d\n", order3.order_id);

    order_event_t order4;
    order4.elevator_id = 3;
    order4.floor = 1;
    order4.order_type = UP_FROM;
    order4.order_id = GenerateOrderID(&order4);
    printf("Order 4 ID: %d\n", order4.order_id);

    order_event_t order5;
    order5.elevator_id = 3;
    order5.floor = 1;
    order5.order_type = UP_FROM;
    order5.order_id = GenerateOrderID(&order5);
    printf("Order 5 ID: %d\n", order5.order_id);
    /*  */

    /* Testing enqueueing */
    printf("Testing enqueueing\n");
    enqueue_order(queue, &order);
    enqueue_order(queue, &order2);
    enqueue_order(queue, &order3);
    enqueue_order(queue, &order4);
    printf("Enqueueing identical order\n");
    enqueue_order(queue, &order5);
    assert(queue->size == 4);


    /* Serialising data
    char * buffer = malloc(sizeof(order_queue_t) + sizeof(order_event_t) * queue->capacity);
    memcpy(buffer, queue, sizeof(order_queue_t));
    memcpy(buffer + sizeof(order_queue_t), queue->orders, sizeof(order_event_t) * queue->capacity);

    tcp_openConnection("127.0.0.1",9000);
    printf("Sending order queue\n");
    printf("size of order queue: %d\n", queue->size);
    printf("char size of order queue: %ld\n", sizeof(order_queue_t) + sizeof(order_event_t) * queue->size);
    tcp_send("127.0.0.1", buffer, sizeof(order_queue_t) + sizeof(order_event_t) * queue->size);

    usleep(1000);
    printf("Sending order queue\n");
    printf("size of order queue: %d\n", queue->size);
    printf("char size of order queue: %ld\n", sizeof(order_queue_t) + sizeof(order_event_t) * queue->size);
    tcp_send("127.0.0.1", buffer, sizeof(order_queue_t) + sizeof(order_event_t) * queue->size);

    free(buffer);*/

    send_order_queue_connect("127.0.0.1", 9000);
    send_order_queue_send_order("127.0.0.1", queue);
    usleep(1000);
    send_order_queue_send_order("127.0.0.1", queue);

    
    //tcp_send("127.0.0.1", (char*)queue, (sizeof(order_queue_t) + sizeof(order_event_t) * queue->capacity));

    /* Testing dequeueing */
    printf("Testing dequeueing\n");
    printf("Dequeueing non-existing order\n");
    dequeue_order(queue, &order5); /* This is supposed to return as not in queue */
    dequeue_order(queue, &order);
    dequeue_order(queue, &order2);
    dequeue_order(queue, &order3);
    dequeue_order(queue, &order4);
    assert(queue->size == 0);

    printf("All tests passed\n");
    return 0;
}
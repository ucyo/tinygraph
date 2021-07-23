#include <stdio.h>
#include <stdlib.h>

#include "tinygraph-utils.h"
#include "tinygraph-stack.h"
#include "tinygraph-queue.h"

// Note: does a ring buffer based queue implementation
// make more sense for our use case of graph based bfs?

typedef struct tinygraph_queue {
  tinygraph_stack_s lhs;
  tinygraph_stack_s rhs;
} tinygraph_queue;


static bool tinygraph_queue_refill(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  if (!tinygraph_stack_reserve(queue->rhs,
        tinygraph_stack_get_size(queue->lhs))) {

    return false;
  }


  for (uint32_t i = 0; !tinygraph_stack_is_empty(queue->lhs); ++i) {
    if (!tinygraph_stack_push(queue->rhs,
          tinygraph_stack_pop(queue->lhs))) {

      return false;
    }
  }

  return true;
}


tinygraph_queue* tinygraph_queue_construct() {
  tinygraph_queue *out = malloc(sizeof(tinygraph_queue));

  if (!out) {
    return NULL;
  }

  tinygraph_stack_s lhs = tinygraph_stack_construct();

  if (!lhs) {
    free(out);

    return NULL;
  }

  tinygraph_stack_s rhs = tinygraph_stack_construct();

  if (!rhs) {
    tinygraph_stack_destruct(lhs);

    free(out);

    return NULL;
  }

  *out = (tinygraph_queue) {
    .lhs = lhs,
    .rhs = rhs,
  };

  return out;
}


tinygraph_queue* tinygraph_queue_copy(tinygraph_queue *queue) {
  if (!queue) {
    return queue;
  }

  tinygraph_queue *copy = tinygraph_queue_construct();

  if (!copy) {
    return NULL;
  }

  tinygraph_stack_s lcopy = tinygraph_stack_copy(queue->lhs);

  if (!lcopy) {
    tinygraph_queue_destruct(copy);

    return NULL;
  }

  tinygraph_stack_s rcopy = tinygraph_stack_copy(queue->rhs);

  if (!rcopy) {
    tinygraph_stack_destruct(lcopy);

    tinygraph_queue_destruct(copy);

    return NULL;
  }

  copy->lhs = lcopy;
  copy->rhs = rcopy;

  return copy;
}


void tinygraph_queue_destruct(tinygraph_queue *queue) {
  if (!queue) {
    return;
  }

  tinygraph_stack_destruct(queue->lhs);
  tinygraph_stack_destruct(queue->rhs);

  queue->lhs = NULL;
  queue->rhs = NULL;

  free(queue);
}


bool tinygraph_queue_reserve(tinygraph_queue *queue, uint32_t capacity) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  const bool lok = tinygraph_stack_reserve(queue->lhs, capacity);
  const bool rok = tinygraph_stack_reserve(queue->rhs, capacity);

  return lok && rok;
}


uint32_t tinygraph_queue_get_front(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);
  TINYGRAPH_ASSERT(!(tinygraph_stack_is_empty(queue->lhs)
        && tinygraph_stack_is_empty(queue->rhs)));

  if (!tinygraph_stack_is_empty(queue->rhs)) {
    return tinygraph_stack_get_top(queue->rhs);
  }

  return tinygraph_stack_get_bottom(queue->lhs);
}


uint32_t tinygraph_queue_get_back(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);
  TINYGRAPH_ASSERT(!(tinygraph_stack_is_empty(queue->lhs)
        && tinygraph_stack_is_empty(queue->rhs)));

  if (!tinygraph_stack_is_empty(queue->lhs)) {
    return tinygraph_stack_get_top(queue->lhs);
  }

  return tinygraph_stack_get_bottom(queue->rhs);
}


uint32_t tinygraph_queue_get_size(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  const uint32_t lsize = tinygraph_stack_get_size(queue->lhs);
  const uint32_t rsize = tinygraph_stack_get_size(queue->rhs);

  return lsize + rsize;
}


uint32_t tinygraph_queue_get_capacity(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  const uint32_t lcapacity = tinygraph_stack_get_capacity(queue->lhs);
  const uint32_t rcapacity = tinygraph_stack_get_capacity(queue->lhs);

  return lcapacity < rcapacity ? lcapacity : rcapacity;
}


bool tinygraph_queue_is_empty(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  const bool lempty = tinygraph_stack_is_empty(queue->lhs);
  const bool rempty = tinygraph_stack_is_empty(queue->rhs);

  return lempty && rempty;
}


void tinygraph_queue_clear(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  tinygraph_stack_clear(queue->lhs);
  tinygraph_stack_clear(queue->rhs);
}


bool tinygraph_queue_push(tinygraph_queue *queue, uint32_t value) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  return tinygraph_stack_push(queue->lhs, value);
}


uint32_t tinygraph_queue_pop(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);
  TINYGRAPH_ASSERT(!(tinygraph_stack_is_empty(queue->lhs)
        && tinygraph_stack_is_empty(queue->rhs)));

  if (tinygraph_stack_is_empty(queue->rhs)) {
    const bool ok = tinygraph_queue_refill(queue);
    TINYGRAPH_ASSERT(ok);  // stacks corrupted
  }

  TINYGRAPH_ASSERT(tinygraph_stack_is_empty(queue->lhs));
  TINYGRAPH_ASSERT(!tinygraph_stack_is_empty(queue->rhs));

  return tinygraph_stack_pop(queue->rhs);
}


void tinygraph_queue_print_internal(tinygraph_queue *queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  fprintf(stderr, "queue internals\n");

  tinygraph_stack_print_internal(queue->lhs);
  tinygraph_stack_print_internal(queue->rhs);
}

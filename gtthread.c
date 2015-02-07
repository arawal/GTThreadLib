#include "gtthread.h"

static ucontext_t sched_context;
static int ids = 1;
static int qCount = 0;

struct gtthread * tq_head = NULL;
struct gtthread * tq_tail = NULL;
struct gtthread * running = NULL;
struct gtthread * p_node = NULL;
struct itimerval quantum;
struct sigaction preempt;

void gtthread_init (long period) {

  /* Initialize main context with basic settings */
  if (getcontext(&sched_context) == 0) {
    sched_context.uc_stack.ss_sp = malloc(STACKSIZELIMIT);
    sched_context.uc_stack.ss_size = STACKSIZELIMIT;
    sched_context.uc_stack.ss_flags = 0;
    sched_context.uc_link = NULL;
  } 
  else {
    printf("\nCouldn't initialize scheduler context");
    exit(-1);
  }

  /* Initialize timer values */
  quantum.it_value.tv_sec = 0;
  quantum.it_value.tv_usec = (long) period;
  quantum.it_interval = quantum.it_value;

  /* Create new thread node */
  p_node = (struct gtthread*) malloc (sizeof(struct gtthread));
  p_node->tID = 0;
  p_node->status = 9;
  p_node->next = NULL;
  getcontext(&p_node->context);  

  /* Initialize signal handler */
  preempt.sa_handler = signal_handler;
  preempt.sa_flags = SA_RESTART | SA_SIGINFO;
  sigemptyset(&preempt.sa_mask);
  if (sigaction(SIGVTALRM, &preempt, NULL) == -1) {
    printf("\nCouldn't initialize signal handler");
    exit(-1);
  }

  /* Start timer */
  if (setitimer(ITIMER_VIRTUAL, &quantum, NULL) != 0) {
    printf("\nCouldn't initialize timer.");
    exit(-1);
  }

  /* Create thread queue for the process */
  if (tq_head == NULL) {
    queue_thread(p_node);
    qCount++;
  }
  return;
}

void queue_thread (struct gtthread * t_new) {
  /*If the queue is empty, make the head and tail point to the new node */
  if(tq_head == NULL) {
    tq_head = t_new;
    tq_tail = t_new;
    running = t_new;
  }

  /*If the queue is not empty, add the new node to the end and make the tail point to it */
  else {
    tq_tail->next = t_new;
    tq_tail = t_new;
  }
}

int gtthread_create (gtthread_t *thread, void *(*start_routine)(void *), void *arg) {
  /* Check if the process has been initialized */
  if (tq_head == NULL) {
    printf("\nProcess not initialized");
    exit(-1);
  }

  /*Assign the next thread number as the ID */
  *thread = ids;

  /* Create a new thread node */
  struct gtthread *t_node = malloc (sizeof(struct gtthread));

  /* Initialize context for the new thread */
  getcontext(&t_node->context);
  t_node->context.uc_link = &sched_context;
  t_node->context.uc_stack.ss_sp = malloc(STACKSIZELIMIT);
  t_node->context.uc_stack.ss_size = STACKSIZELIMIT;
  t_node->context.uc_stack.ss_flags = 0;
  
  /* Initialize thread values */
  t_node->tID = *thread;
  t_node->status = 9;
  
  /* Link the context to the thread routine */
  makecontext(&t_node->context, (void(*)(void)) schedule_thread, 2, start_routine, arg);

  /* Add thread to the queue */
  queue_thread(t_node);
  qCount++;
  if (ids++ == 1) {
    running = t_node;
    run_thread();
  } 
  else {
    swapcontext(&running->context, &sched_context);
  }
  return 0;
}

static void schedule_thread(void *thread_function(), void *arg) {
  running->status = 1;
  running->retval = thread_function(arg);
  if (!running->status) {
    running->status = 0;
    gtthread_cancel(running->tID);
  }
  swapcontext(&running->context, &sched_context);
  return;
}

static void run_thread () {

  while (qCount != 0) {
    if (running->status >= 2) {
      swapcontext(&sched_context, &running->context);
    }

    if (qCount == 1 && p_node->status == 0) {
      exit(0);
    }
    
    if (running == tq_tail) {
      running = tq_head;
    } else {
      running = running->next;
    }
  }
}

int gtthread_cancel(gtthread_t thread) {

  struct gtthread *temp = tq_head;

  /* If queue is empty, fail */
  if (tq_head == NULL) {
    return -1;
  }
  
  /* Find the thread to be cancelled from the queue */
  for (; temp != NULL; temp = temp->next) {
    if (temp->tID == thread && temp->status != -1) {
      free(temp->context.uc_stack.ss_sp);
      temp->status = -1;
      qCount--;
      break;
    }
  }
  return 0;
}

int gtthread_join(gtthread_t thread, void **status) {
  struct gtthread *temp = tq_head;

  /* Find the thread to join to from the queue */
  for (; temp != NULL; temp = temp->next)
    if (temp->tID == thread) break;

  /* Activate join protocol for the thread to wait for the joined thread */
  while (temp->status >= 2) {}
  if ((temp->status < 2) && status != NULL) {
    *status = (void *)temp->retval;
    return 0;
  }
  return -1;
}

void gtthread_exit(void *retval) {
  running->retval = retval;
  running->status = 0;
  if (running != p_node) {
    gtthread_cancel(running->tID);
  }
  swapcontext(&running->context, &sched_context);
  return;
}

int gtthread_yield(void) {
  int flag = 0;
  getcontext(&running->context);
  if (flag == 0) {
    flag = 1;
    setcontext(&sched_context);
  }
  return flag;
}

int gtthread_equal(gtthread_t t1, gtthread_t t2) {
  if (t1 == t2) {
    return 1;
  }
  else {
    return 0;
  }
}

gtthread_t gtthread_self(void) {
  return running->tID;
}

int gtthread_mutex_init(gtthread_mutex_t *mutex) {
  if (mutex->lock == 1) {
    return -1;
  }
  mutex->lock = 0;
  mutex->owner = -1;
  return 0;
}

int gtthread_mutex_lock(gtthread_mutex_t *mutex) {
  if ((mutex->owner) == gtthread_self()) {
    return -1;
  }
  while (mutex->lock != 0 && mutex->owner != gtthread_self()) {
    gtthread_yield();
  }
  mutex->lock = 1;
  mutex->owner = gtthread_self();
  return 0;
}

int gtthread_mutex_unlock(gtthread_mutex_t *mutex) {
  if (mutex->lock == 1 && mutex->owner == gtthread_self()) {
    mutex->lock = 0;
    mutex->owner = -1;
    return 0;
  }
  return -1;
}

static void signal_handler (int sig_id) {
  if (sig_id == SIGVTALRM) {
    swapcontext(&running->context, &sched_context);
  }
  else 
    return;
}

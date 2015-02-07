/* Assuming that the the first chopstick is to the left of the first philosopher. So the setting is:

          p1  c5
       c2         p5
           
       p2         c4
          c3  p4


 */
#include "gtthread.h"

gtthread_t philosopher_t[5];
gtthread_mutex_t chopsticks[5];

void philosopher_task(int);
void eat(int);
void think(int);
void get_both_chopsticks(int);
void done_eating(int);

int main () {
  int i;
  long period = 100000;
  gtthread_init(period);

  
  for (i=0; i<5; i++) {
    gtthread_mutex_init(&chopsticks[i]);    
  }
  for (i=0; i<5; i++) {
    gtthread_create(&philosopher_t[i], (void *)philosopher_task, (int *) (i));
  }
  
  gtthread_exit(NULL);
}

void philosopher_task(int p_id) {
  while(1) {
    think(p_id);
    printf("\nPhilosopher %d's time to eat.", p_id+1);
    eat(p_id);
    printf("\nPhilosopher %d's time to think.", p_id+1);
  }
  //return;
}

void think(int p_id) {
  long int i, r_time;
  srand(time(NULL));

  r_time = rand() % 10000000;
  printf("\nPhilosopher %d is thinking.", p_id+1);
  for (i = 0; i < r_time; i++);
  //return;
}

void eat(int p_id) {
  long int i, r_time;
  srand(time(NULL));

  r_time = rand() % 10000000;
  get_both_chopsticks(p_id);
  printf("\n Philosopher %d is eating.", p_id+1);
  for(i = 0; i < r_time; i++);
  done_eating(p_id); 
  //return;
}

void get_both_chopsticks(int p_id) {
  int i1 = p_id;
  int i2 = (p_id + 1) % 5;
  
  if (p_id % 2 != 0) {
    gtthread_mutex_lock(&chopsticks[i1]);
    printf("\nPhilosopher %d has picked up the left chopstick.", p_id+1);
    gtthread_mutex_lock(&chopsticks[i2]);
    printf("\nPhilosopher %d has picked up the right chopstick.", p_id+1);
  }
  else {
    gtthread_mutex_lock(&chopsticks[i2]);
    printf("\nPhilosopher %d has picked up the right chopstick.", p_id+1);
    gtthread_mutex_lock(&chopsticks[i1]);
    printf("\nPhilosopher %d has picked up the left chopstick.", p_id+1);
  }

  //return;
}

void done_eating(int p_id) {
  int i1 = p_id;
  int i2 = (p_id + 1) % 5;
  
  if (p_id % 2 != 0) {
    gtthread_mutex_unlock(&chopsticks[i2]);
    printf("\nPhilosopher %d has released the right chopstick.", p_id+1);
    gtthread_mutex_unlock(&chopsticks[i1]);
    printf("\nPhilosopher %d has released the left chopstick.", p_id+1);
  }
  else {
    gtthread_mutex_unlock(&chopsticks[i1]);
    printf("\nPhilosopher %d has released the left chopstick.", p_id+1);
    gtthread_mutex_unlock(&chopsticks[i2]);
    printf("\nPhilosopher %d has released the right chopstick.", p_id+1);
  }

  
  //return;
}

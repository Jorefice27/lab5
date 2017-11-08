#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include "scheduling.h"

#define NUM_PROCESSES 20

struct process
{
  /* Values initialized for each process */
  int arrivaltime;  /* Time process arrives and wishes to start */
  int runtime;      /* Time process requires to complete job */
  int priority;     /* Priority of the process */

  /* Values algorithm may use to track processes */
  int starttime;
  int endtime;
  int flag;
  int remainingtime;
};

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
void round_robin(struct process *proc);
void round_robin_priority(struct process *proc);
void getAverageTime(struct process *proc);
int main()
{
  int i;
  struct process proc[NUM_PROCESSES],      /* List of processes */
                 proc_copy[NUM_PROCESSES]; /* Backup copy of processes */

  /* Seed random number generator */
  /*srand(time(0));*/  /* Use this seed to test different scenarios */
  srand(0xC0FFEE);     /* Used for test to be printed out */

  /* Initialize process structures */
  for(i=0; i<NUM_PROCESSES; i++)
  {
    proc[i].arrivaltime = rand()%100;
    proc[i].runtime = (rand()%30)+10;
    proc[i].priority = rand()%3;
    proc[i].starttime = 0;
    proc[i].endtime = 0;
    proc[i].flag = 0;
    proc[i].remainingtime = -1;
  }

  /* Show process values */
  printf("Process\tarrival\truntime\tpriority\n");
  for(i=0; i<NUM_PROCESSES; i++)
    printf("%d\t%d\t%d\t%d\n", i, proc[i].arrivaltime, proc[i].runtime, proc[i].priority);



  /* Run scheduling algorithms */
  printf("\n\nFirst come first served\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  first_come_first_served(proc_copy);

  printf("\n\nShortest remaining time\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  shortest_remaining_time(proc_copy);

  printf("\n\nRound Robin\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin(proc_copy);

  printf("\n\nRound Robin with priority\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin_priority(proc_copy);

  return 0;
}

void first_come_first_served(struct process *proc)
{
  int t = 0;
  int i;
  int complete = 0;

  while(complete < NUM_PROCESSES)
  {
    int min = INT_MAX;
    int id = -1;
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      //*Initialize* newly arrived  processes
      if(proc[i].arrivaltime <= t && proc[i].remainingtime < 0)
      {
        proc[i].remainingtime = proc[i].runtime;
      }
      if(proc[i].arrivaltime <= t && proc[i].remainingtime > 0)
      {
        if(proc[i].arrivaltime < min && proc[i].endtime == 0)
        {
          min = proc[i].arrivaltime;
          id = i;
        }
      }
    }
    if(id < 0)
    {
      //idle
      t++;
    }
    else
    {
      //execute process
      printf("Process %d started at time %d\n", id, t);
      t += proc[id].runtime;
      proc[id].remainingtime = -1;
      proc[id].endtime = t;
      printf("Process %d finished at time %d\n", id, t);
      complete++;
    }
  }
  getAverageTime(proc);
}

void shortest_remaining_time(struct process *proc)
{
  int t = 0;
  int i;
  int complete = 0;

  while(complete < NUM_PROCESSES)
  {
    int min = INT_MAX;
    int id = -1;
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      //*Initialize* newly arrived  processes
      if(proc[i].arrivaltime <= t && proc[i].remainingtime < 0)
      {
        proc[i].remainingtime = proc[i].runtime;
      }
      //find a processs that has arrived and still needs to complete
      if(proc[i].arrivaltime <= t && proc[i].remainingtime > 0)
      {
        if(proc[i].remainingtime < min && proc[i].endtime == 0)
        {
          min = proc[i].remainingtime;
          id = i;
        }
      }
    }
    if(id < 0)
    {
      //idle
      t++;
    }
    else
    {
      //execute process
      printf("Process %d started at time %d\n", id, t);
      t += proc[id].runtime;
      proc[id].remainingtime = -1;
      proc[id].endtime = t;
      printf("Process %d finished at time %d\n", id, t);
      complete++;
    }
  }

  getAverageTime(proc);
}

void round_robin(struct process *proc)
{
  int t = 0;
  int i;
  int complete = 0;
  int t0;

  while(complete < NUM_PROCESSES)
  {
    t0 = t;
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      if(proc[i].arrivaltime <= t)
      {
        if(proc[i].remainingtime < 0)
        {
          proc[i].remainingtime = proc[i].runtime;
        }
        else if(proc[i].remainingtime > 0)
        {
          if(proc[i].remainingtime == proc[i].runtime)
          {
            printf("Process %d started at time %d\n", i, t);
          }
          proc[i].remainingtime--;
          t++;

          if(proc[i].remainingtime == 0 && proc[i].endtime == 0)
          {
            proc[i].endtime = t;
            printf("Process %d finished at time %d\n", i, t);
            complete++;
          }
        }
      }
    }
    //increment time if we couldn't run any processes
    if(t0 == t)
    {
      t++;
    }
  }
  getAverageTime(proc);
}

void round_robin_priority(struct process *proc)
{
  int t = 0;
  int priority, index, i, t0, j;
  int complete = 0;

  //create queues for each priority level
  Node p0 = {NULL, NULL, NULL, 0, 0};
  Node p1 = {NULL, NULL, NULL, 0, 0};
  Node p2 = {NULL, NULL, NULL, 0, 0};

  while(complete < NUM_PROCESSES)
  {
    //add new processes to the correct queue
    for(i = 0; i < NUM_PROCESSES; i++)
    {
      if(proc[i].arrivaltime <= t)
      {
        if(proc[i].remainingtime < 0)
        {
          if(proc[i].priority == 2)
          {
            addToEnd(&p2, &proc[i], i);
          }
          else if(proc[i].priority == 1)
          {
            addToEnd(&p1, &proc[i], i);
          }
          else
          {
            addToEnd(&p0, &proc[i], i);
          }
          proc[i].remainingtime = proc[i].runtime;
        }
      }
    }
    //remove a task from the highest priority, non empty, queue and run it
    if(p2.size > 0)
    {
      Node n = pop(&p2);
      if(n.proc->remainingtime == n.proc->runtime)
      {
        printf("Process %d started at time %d\n", n.id, t);
      }
      n.proc->remainingtime --;
      t++;
      //put it at the end of the queue if it still needs to run
      if(n.proc->remainingtime > 0)
      {
        addToEnd(&p2, n.proc, n.id);
      }
      //report that it's finished
      else if(n.proc->remainingtime == 0 && n.proc->endtime == 0)
      {
        complete++;
        proc[n.id].endtime = t;
        printf("Process %d finished at time %d\n", n.id, t);
      }
    }
    else if(p1.size > 0)
    {
      Node n = pop(&p1);
      if(n.proc->remainingtime == n.proc->runtime)
      {
        printf("Process %d started at time %d\n", n.id, t);
      }
      n.proc->remainingtime --;
      t++;
      if(n.proc->remainingtime > 0)
      {
        addToEnd(&p1, n.proc, n.id);
      }
      else if(n.proc->remainingtime == 0 && n.proc->endtime == 0)
      {
        complete++;
        proc[n.id].endtime = t;
        printf("Process %d finished at time %d\n", n.id, t);
      }
    }
    else if(p0.size > 0)
    {
      Node n = pop(&p0);
      if(n.proc->remainingtime == n.proc->runtime)
      {
        printf("Process %d started at time %d\n", n.id, t);
      }
      n.proc->remainingtime --;
      t++;
      if(n.proc->remainingtime > 0)
      {
        addToEnd(&p0, n.proc, n.id);
      }
      else if(n.proc->remainingtime == 0 && n.proc->endtime == 0)
      {
        complete++;
        proc[n.id].endtime = t;
        printf("Process %d finished at time %d\n", n.id, t);
      }
    }
    else
    {
      t++;
    }
  }
  getAverageTime(proc);
}

void getAverageTime(struct process *proc)
{
  int i, sum = 0;

  for(i = 0; i < NUM_PROCESSES; i++)
  {
    sum += (proc[i].endtime - proc[i].arrivaltime);
  }

  sum /= NUM_PROCESSES;
  printf("Average time from arrival to finish is %d seconds\n", sum);
}

void addToEnd(Node *head, struct process* proc, int id)
{
	if(head->size == 0)
	{
		addToEmpty(head, proc, id);
		return;
	}
  //create Node struct
	Node* temp = (Node*) malloc(sizeof(Node));
  temp->proc = proc;
  temp->id = id;

  head->prev->next = temp;
  temp->prev = head->prev;
	temp->next = head;
	head->prev = temp;
	head->size++;
}

void addToEmpty(Node *head, struct process* proc, int id)
{
  //create Node struct
	Node* temp = (Node*) malloc(sizeof(Node));
  temp->proc = proc;
  temp->id = id;

	head->next = temp;
	head->prev = temp;
	temp->next = head;
	temp->prev = head;
	head->size++;
}

Node pop(Node *head)
{
	if(head->size > 0)
	{
		Node r = *head->next;
		head->next = r.next;
		head->next->prev = head;
		head->size--;

		return r;
	}
}

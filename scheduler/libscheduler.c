/**
 * Scheduler Lab
 * CS 241 - Fall 2016
 */
#include "libpriqueue.h"
#include "libscheduler.h"

typedef struct _job_t job_t;
typedef struct _core_t core_t;
/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job
  queue elements.
*/
struct _job_t {
  int id;
  int priority;
  int runningTime;
  int time;
  int jobNumber;
  
  int arrival_time;
  int end_time;
  int start_time;
  int time_ran;
  //int timeRan, int timeLeft
  //int startExecTime
  //int finishTime
};

struct _core_t {
  int free;   // boolean representing whether the core is in use or not
  job_t *job; // the job that the core is currently running, if any
};

priqueue_t pqueue;
core_t *cores;
scheme_t scheme;
int num_cores;
scheme_t scheme_type;

float total_waiting_time = 0;
float total_response_time = 0;
float total_turnaround_time = 0;

int number_of_jobs = 0;

int break_tie(const void *a, const void *b) { return comparer_fcfs(a, b); }

int comparer_fcfs(const void *a, const void *b) {
  job_t *job_a = (job_t*)a;
  job_t *job_b = (job_t*)b;
  if (job_a->time < job_b->time) return -1;
  else if (job_b->time < job_a->time) return 1;
  else return break_tie(job_a, job_b);
  return 0;
}

int comparer_ppri(const void *a, const void *b) {
  // Complete as is
  return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
  job_t *job_a = (job_t*)a;
  job_t *job_b = (job_t*)b;
  if (job_a->priority < job_b->priority) return -1;
  else if (job_b->priority < job_a->priority) return 1;
  else return break_tie(job_a, job_b);
}

int comparer_psjf(const void *a, const void *b) {
  return 0;
}

int comparer_rr(const void *a, const void *b) {
  // Casting to void to silence compiler warning
  (void)a;
  (void)b;
  // Picking 1 arbitarily.
  return 1;
}

int comparer_sjf(const void *a, const void *b) {
  job_t *job_a = (job_t*)a;
  job_t *job_b = (job_t*)b;
  if (job_a->runningTime < job_b->runningTime) return -1;
  else if (job_b->runningTime < job_a->runningTime) return 1;
  else return break_tie(job_a, job_b);
}

void scheduler_start_up(int numcores, scheme_t s) {

  num_cores = numcores;
  scheme_type = s;
  
  //init cores
  cores = calloc(numcores, sizeof(core_t));
  int i = 0;
  for (; i < numcores; i++) {
    cores[i].free = 1;
    cores[i].job = NULL;
  }

  //init q
  switch (s) {
  case FCFS:
    priqueue_init(&pqueue, comparer_fcfs);
    break;

  case PRI:
    priqueue_init(&pqueue, comparer_pri);
    break;

  case PPRI:
    priqueue_init(&pqueue, comparer_ppri);
    break;

  case PSJF:
    priqueue_init(&pqueue, comparer_psjf);
    break;

  case RR:
    priqueue_init(&pqueue, comparer_rr);
    break;

  case SJF:
    priqueue_init(&pqueue, comparer_sjf);
    break;

  default:
    printf("Did not recognize scheme\n");
    exit(1);
  }
}

int scheduler_new_job(int job_number, int time, int running_time,
                      int priority) {

  number_of_jobs++;
  
  job_t *new_job = malloc(sizeof(job_t));
  new_job->jobNumber = job_number;
  new_job->time = time;
  new_job->runningTime = running_time;
  new_job->priority = priority;

  int i = 0;
  for (; i < num_cores; i++) {
    if (cores[i].free) {
      cores[i].job = new_job;
      cores[i].free = 0;
      return i;
    }
  }

  //check for scehemes
  if (scheme_type == PPRI) {
    i = 0;
    for (; i < num_cores; i++) {
      if (cores[i].job != NULL && (cores[i].job)->priority > priority) {
	priqueue_offer(&pqueue, (void*)(cores[i].job));
	cores[i].job = new_job;
	return i;
      }
    }
  }

  if (scheme_type == PSJF) {
    i = 0;
    for (; i < num_cores; i++) {
      
    }
  }
  
  priqueue_offer(&pqueue, (void*)new_job);
  return -1;
}

int scheduler_job_finished(int core_id, int job_number, int time) {


  //waiting time
  if (cores[core_id].job != NULL) {
    int waiting_time = time - cores[core_id].job->time - cores[core_id].job->runningTime;
    total_waiting_time += waiting_time;
  }

  //turnaround time
  if (cores[core_id].job != NULL) {
    int turnaround_time = time - cores[core_id].job->time;
    total_turnaround_time += turnaround_time;
  }

  
  cores[core_id].free = 1;
  cores[core_id].job = NULL; //free prev job?
  
  job_t *job = (job_t*)(priqueue_poll(&pqueue));
  if (job == NULL) return -1;
  else {
    
    //response time
    if (job != NULL) {
      int response_time = time - job->time;
      total_response_time += response_time;
    }
    
    cores[core_id].free = 0;
    cores[core_id].job = job;
    return job->jobNumber;
  }
}

int scheduler_quantum_expired(int core_id, int time) {
  cores[core_id].free = 1;
  priqueue_offer(&pqueue, (void*)(cores[core_id].job));
  cores[core_id].job = NULL;
  
  job_t *job =  (job_t*)(priqueue_poll(&pqueue));
  if (job == NULL) return -1;
  else {
    cores[core_id].job = job;
    cores[core_id].free = 0;
    return job->jobNumber;
  }
}

float scheduler_average_waiting_time() {
  return total_waiting_time / number_of_jobs;
}

float scheduler_average_turnaround_time() {
  return total_turnaround_time / number_of_jobs;
}

float scheduler_average_response_time() {
  return total_response_time / number_of_jobs;
}

void scheduler_clean_up() {
  /* Do more cleaning up here */
  priqueue_destroy(&pqueue);
}

void scheduler_show_queue() {
  // This function is left entirely to you! Totally optional.
}

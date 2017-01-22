/**
 * Overworked Interns Lab
 * CS 241 - Fall 2016
 */
#include "company.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void *work_interns(void *p) {
  Company *company = (Company *)p;
  pthread_mutex_t *left_intern, *right_intern;
  int left_status;
  int right_status;
  //int company_num = Company_get_company_number(company);
  left_intern = Company_get_left_intern(company);
  right_intern = Company_get_right_intern(company);
  
  while (running) {
    /*
    left_intern = Company_get_left_intern(company);
    right_intern = Company_get_right_intern(company);
    int left_status = pthread_mutex_trylock(left_intern);
    int right_status = pthread_mutex_trylock(right_intern);
    while (left_status || right_status) {
      if (left_status == 0) {
	pthread_mutex_unlock(left_intern);
      }
      if (right_status == 0) {
	pthread_mutex_unlock(right_intern);
      }
      left_status = pthread_mutex_trylock(left_intern);
      right_status = pthread_mutex_trylock(right_intern);
    }
    Company_hire_interns(company);
    pthread_mutex_unlock(right_intern);
    pthread_mutex_unlock(left_intern);
    */
    
    left_status = pthread_mutex_trylock(left_intern);
    if (!left_status) {
      right_status = pthread_mutex_trylock(right_intern);
      if (!right_status) {
	Company_hire_interns(company);
	pthread_mutex_unlock(left_intern);
	pthread_mutex_unlock(right_intern);
	usleep(50000);
      }
      else {
	pthread_mutex_unlock(left_intern);
      }
    }
    /*
    if (company_num % 2 == 1) {
      pthread_mutex_lock(left_intern);
      pthread_mutex_lock(right_intern);
      Company_hire_interns(company);
      pthread_mutex_unlock(left_intern);
      pthread_mutex_unlock(right_intern);
    }
    else {
      pthread_mutex_lock(right_intern);
      pthread_mutex_lock(left_intern);
      Company_hire_interns(company);
      pthread_mutex_unlock(right_intern);
      pthread_mutex_unlock(left_intern);
    }
    */
  }
  return NULL;
}

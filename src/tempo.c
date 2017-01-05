#define _XOPEN_SOURCE 500

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include "timer.h"
#include "utlist.h"
// Return number of elapsed µsec since... a long time ago 

static unsigned long get_time(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    // Only count seconds since beginning of 2016 (not jan 1st, 1970)
    tv.tv_sec -= 3600UL * 24 * 365 * 46;

    return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN

typedef struct queued_event {
    void *param_save;
    struct itimerval delay;
    unsigned long time;
    struct queued_event *next, *prev;
} queued_event;
static queued_event *head = NULL;

pthread_mutex_t mutex;

static void routine(int signo) {

}

/**
 * Add an event to queue
 * @param head Head of the list
 * @param elem Elem to add
 */
void add(queued_event **head, queued_event **elem) {
    if (*head == NULL) {
        *head = *elem;
        (*head)->next = NULL;
        return;
    }
    queued_event *tmp = *head;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    (*elem)->next = NULL;
    (*elem)->prev = tmp->next;
    tmp->next = *elem;
}

/**
 * Daemon to listen on SIGALRM and process event.
 * 
 */
void* demon() {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGALRM);

    struct sigaction s;
    s.sa_handler = routine;
    sigemptyset(&s.sa_mask);
    sigaddset(&s.sa_mask, SIGALRM);
    s.sa_flags = 0;
    //Connect SIGALRM
    sigaction(SIGALRM, &s, NULL);
    queued_event *elem = NULL;

    while (1) {
        //Wait for SIGALRM
        sigsuspend(&mask);
        //Lock for race condition in list
        pthread_mutex_lock(&mutex);
        unsigned long currentTime = 0;
        unsigned long timeForNext = 0;

        //Process next event and event 500ms after now.
        while ((currentTime + 500 * 1000UL > timeForNext) && head != NULL) {
            //Process event
            sdl_push_event(head->param_save);
            elem = head;
            head = head->next;
            free(elem);
            if (head != NULL)
                //Get time for next event
                timeForNext = head->time + head->delay.it_value.tv_sec * 1000000UL + head->delay.it_value.tv_usec;
            currentTime = get_time();
        }

        if (head != NULL) {
            //Events will happen 
            timeForNext = head->time + head->delay.it_value.tv_sec * 1000000UL + head->delay.it_value.tv_usec;
            currentTime = get_time();
            //Calculate time until next event
            unsigned long timeBeforeNext = timeForNext - currentTime;
            struct itimerval nextTime;
            nextTime.it_interval.tv_sec = 0;
            nextTime.it_interval.tv_usec = 0;
            nextTime.it_value.tv_sec = timeBeforeNext / 1000000UL;
            nextTime.it_value.tv_usec = timeBeforeNext % 1000000UL;
            //Set timer to trigger next event
            setitimer(ITIMER_REAL, &(nextTime), NULL);
        }
        //Release mutex
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

/**
 * Init function, init mask and launch daemon
 * @return 
 */
int timer_init(void) {
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &block, NULL); //Block SIGALRM for all thread

    pthread_mutex_init(&mutex, NULL);

    pthread_t thread;
    //Create daemon
    if (pthread_create(&thread, NULL, demon, NULL) != 0) {
        perror("timer_init :phtread");
    }
    return 1;
}

/**
 * Return strlen-like compare 
 * @param queued_event event a
 * @param queued_event event b
 * @return 
 */
int compEvent(void *s1, void *s2) {
    struct queued_event *event1 = (struct queued_event *) s1;
    struct queued_event *event2 = (struct queued_event *) s2;
    unsigned long time1 = event1->time + event1->delay.it_value.tv_sec * 1000000UL + event1->delay.it_value.tv_usec;
    unsigned long time2 = event2->time + event2->delay.it_value.tv_sec * 1000000UL + event2->delay.it_value.tv_usec;
    return (time1 < time2) ? -1 : (time1 > time2);
}

/**
 * Set a new event
 * @param delay Delay until event
 * @param param Pointer used by SDL
 */
void timer_set(Uint32 delay, void *param) {
    queued_event *event = malloc(sizeof (struct queued_event));
    event->delay.it_interval.tv_sec = 0;
    event->delay.it_interval.tv_usec = 0;
    //Convert time
    long int seconds = delay / 1000;
    long int micros = (delay % 1000)*1000;
    event->delay.it_value.tv_sec = seconds;
    event->delay.it_value.tv_usec = micros;
    event->param_save = param;
    event->time = get_time();

    //Lock for race condition
    pthread_mutex_lock(&mutex);

    //Add to queue
    add(&head, &event);
    //Sort the queue
    DL_SORT(head, compEvent);
    if (head == event) {
        //Set timer if current event is closer in time than others
        if (setitimer(ITIMER_REAL, &(event->delay), NULL) == -1) {
            perror("setitimer");
        }
    }
    //Release the mutex
    pthread_mutex_unlock(&mutex);
}

#endif

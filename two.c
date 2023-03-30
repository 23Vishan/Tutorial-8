/*
 / OS LAB 4
 / HOSTD.c
 / 2023-03-10
 / CRN: 74026 - Group 9
 / =================================
 / Vishan Patel - 100784201
 / Ammar Salmawy - 100756573
 / Angad Singh - 100814038
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hostd.h"

// Check if the three level feedback queue is empty
// Returns true if empty, false otherwise
int is_feedback_queue_empty(Queue *lvl_1)
{
    // empty
    if (is_empty(lvl_1))
    {
        return TRUE;
    }

    // not empty
    return FALSE;
}

// Removes and returns the highest priority job from the queue
// Returns null if there are no jobs in queue
pcb *get_job(Queue *real_time, Queue *lvl_1, pcb *p)
{
    pcb *job = NULL;

    // check real time
    if (is_empty(real_time) == FALSE)
    {
        job = real_time->front->process;
        remove_from_queue(real_time);
        return job;
    }
    // check level 1
    else if (is_empty(lvl_1) == FALSE)
    {
        job = lvl_1->front->process;
        remove_from_queue(lvl_1);
        return job;
    }
    // no jobs
    else
    {
        return job;
    }
}

// suspends or terminates a running process
pcb *handle_current_job(pcb *job, Queue *real_time, Queue *lvl_1, Resources *r)
{
    // if process has completed execution
    if (--job->processor_time == 0)
    {
        // printf("process completed %d\n", job->pid);

        terminate_process(job);   // terminate job
        return_resources(r, job); // return resources
        free(job);                // free pcb block
        job = NULL;               // set pointer to null
        return job;               // return null
    }
    // if not completed, suspend process
    else
    {
        return job; // return job if real time process
    }
}

// prints all queues
void debug_queues(Queue *input, Queue *real_time, Queue *lvl_1, int time)
{
    printf("\nTIME: %d\n", time);

    printf("%sINPUT  :%s ", HYEL, reset);
    display(input->front);
    printf("----------\n");

    printf("%sREAL   :%s ", HYEL, reset);
    display(real_time->front);
    printf("----------\n");

    printf("%sLEVEL 1: %s", HYEL, reset);
    display(lvl_1->front);
    printf("----------\n");
}

// checks if all jobs have been finished
// returns 1 if true, 0 if false
int is_finished(Queue *real_time, Queue *lvl_1, pcb *cur_job)
{
    if (is_empty(real_time) && is_empty(lvl_1) && cur_job == NULL)
    {
        return TRUE;  // all jobs finished
    }
    else
    {
        return FALSE; // not all jobs are finished
    }
}

int main(int argc, char **argv)
{
    // incorrect number of arguments
    if (argc != 2)
    {
        printf("invalid number of arguments\n"); 
        printf("correct usage: ./dispatch filename.txt\n");
        exit(1);
    }

    int finished = FALSE;  // tracks if all jobs are finished
    int time = 0;          // simulated time

    // queues
    Queue *input_queue = initialize(input_queue);
    Queue *real_time = initialize(real_time);
    Queue *lvl_1 = initialize(lvl_1);

    Resources *r = initiate_resources();  // resources
    pcb *current_process = NULL;          // current process

    // add processes in dispatch list to input queue
    add_to_input_queue(real_time, lvl_1, argv[1]);

    // while not all jobs are finished
    while (!finished)
    {
        // debug_queues(input_queue, real_time, lvl_1, lvl_2, lvl_3, time);

        // if there is a current job
        if (current_process != NULL)
        {
            // suspend or terminate current job
            current_process = handle_current_job(current_process, real_time, lvl_1, r);
        }

        // if not performing a job
        if (current_process == NULL)
        {
            // get the highest priority job to perform
            current_process = get_job(real_time, lvl_1, current_process);

            // if a job is received
            if (current_process != NULL)
            {
                // perform job
                start_process(current_process);
            }
        }

        // check is all quees are empty
        if (is_finished(real_time, lvl_1, current_process) == TRUE)
        {
            finished = TRUE;
            printf("\n%sAll processes completed%s\n", BHRED, reset);
        }

        // simulate one second
        sleep(1);
        time++;
    }
    return 0;
}
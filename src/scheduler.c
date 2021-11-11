#include "project.h"
#include <math.h>

int size;
tasks *tasks_list;
int *output_array;

void *runner(void *param);

int main(int argc, char *argv[])
{
    printf("\n\n");

    FILE *fp;
    char *line = NULL;
    fp = fopen(argv[1], "r"); // opening file in read mode.

    ssize_t read;
    if (fp == NULL)
    {
        perror("File : ");
        exit(EXIT_FAILURE);
    }

    read = fscanf(fp, "%d", &size);

    printf("No. of teams : %d\n", size);

    tasks_list = (tasks *)calloc(size, sizeof(tasks));
    output_array = (int *)calloc(size, sizeof(int));

    int count = 0;

    // Reading fixtures from file and storing them in the queue.
    while (1)
    {
        float period;
        float execution;
        float deadline;
        read = fscanf(fp, "%f%f%f", &period, &execution, &deadline);
        if (read == EOF)
            break;

        tasks_list[count].period = period;
        tasks_list[count].execution = execution;
        tasks_list[count++].deadline = deadline;
    }

    printf("Tasks have been read from file, now gonna check whether they're schedulable or not : \n\n");

    // Closing the file descriptor and freeing up the memory used to read lines from a file.
    fclose(fp);

    // sorting based on period to determine priority.
    for (int i = 0; i < size; i++)
    {
        int temp = i;
        for (int j = i; j < size; j++)
        {
            if (tasks_list[i].period > tasks_list[j].period)
            {
                temp = j;
            }
        }

        tasks another_temp;
        another_temp = tasks_list[temp];
        tasks_list[temp] = tasks_list[i];
        tasks_list[i] = another_temp;
    }

    fflush(stdout);

    char *topRow[] = {"Tasks No.", "Period", "Execution Time", "Deadline"};

    printf(GRN "%*s%*s%*s%*s\n" RESET, TAB, topRow[0], TAB, topRow[1], TAB, topRow[2], TAB, topRow[3]);
    fflush(stdout);
    printf("-------------------------------------------------------------------\n");
    for (int i = 0; i < size; i++)
    {
        printf("%*d%*.3f%*.3f%*.3f",
               TAB, i + 1,
               TAB, tasks_list[i].period,
               TAB, tasks_list[i].execution,
               TAB, tasks_list[i].deadline);
        printf("\n");
    }

    fflush(stdout);

    float length_busy_interval = find_busy_interval();
    printf("\n");
    printf("Length of a Busy Interval : %f", length_busy_interval);
    printf("\n");

    // Calculating no. of jobs released of every task in the busy_interval and storing them in the struct.
    for (int i = 0; i < size; i++)
    {
        tasks_list[i].jobs = (int)ceil(length_busy_interval / tasks_list[i].period);
    }

    pthread_t tid_array[size];
    for (int i = 0; i < size; i++)
    {
        int *task_number = (int *)calloc(1, sizeof(int));
        *task_number = i;
        pthread_create(tid_array + i, NULL, runner, task_number);
    }

    for (int i = 0; i < size; i++)
    {
        if (pthread_join(tid_array[i], NULL) != 0)
        {
            perror("JOIN ");
            exit(1);
        }
    }
    printf("\n");

    for (int i = 0; i < size; i++)
    {
        printf("%d ", output_array[i]);
    }

    printf("\n");

    for (int i = 0; i < size; i++)
    {
        if (output_array[i] != 1)
        {
            printf("Not schedulable \n");
            exit(0);
        }
    }

    printf("Schedulable \n");
}

float find_busy_interval()
{
    float t = 0;
    for (int i = 0; i < size; i++)
    {
        t += tasks_list[i].execution;
    }

    float right_side;

    while (1)
    {
        right_side = 0;

        for (int i = 0; i < size; i++)
        {
            right_side += ((int)ceil(t / tasks_list[i].period)) * tasks_list[i].execution;
        }

        if (t == right_side)
            break;

        t = right_side;
    }

    return t;
}

void *runner(void *param)
{
    int *task_number = (int *)param;

    float execution = tasks_list[*task_number].execution;
    float period = tasks_list[*task_number].period;
    float deadline = tasks_list[*task_number].deadline;

    float t = tasks_list[*task_number].execution;

    float right_side;
    int count_jobs_finished = 0;
    for (int i = 0; i < tasks_list[*task_number].jobs; i++)
    {
        while (1)
        {
            right_side = 0;

            float first_part = (count_jobs_finished + 1) * execution;
            float second_part = 0 ;

            for (int i = 0; i < *task_number; i++)
            {
                second_part += floor((t + count_jobs_finished * period) / tasks_list[i].period) * tasks_list[i].execution;
            }

            right_side = first_part + second_part - count_jobs_finished * period;
            printf("Task : %d & Job : %d right_side %f\n", *task_number, count_jobs_finished, right_side);
            if (t == right_side)
                break;
            t = right_side;
        }

        printf("Task : %d & Job : %d has t %f\n", *task_number, count_jobs_finished, t);

        if (t <= count_jobs_finished * period + deadline)
        {
            printf("Task : %d & Job : %d satisfies deadline\n", *task_number, count_jobs_finished);
            count_jobs_finished++;
            t = tasks_list[*task_number].execution;
        }
        else
        {
            break;
            printf("Task : %d & Job : %d does not satisfy deadline\n", *task_number, count_jobs_finished);
        }
    }

    if (count_jobs_finished == tasks_list[*task_number].jobs)
        output_array[*task_number] = 1;

    pthread_exit(0);
}
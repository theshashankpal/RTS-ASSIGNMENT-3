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

    float utilization;

    for (int i = 0; i < count; i++)
    {
        utilization += tasks_list[i].execution / tasks_list[i].period;
    }

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

    printf("\n\n");

    if (utilization > 1)
    {
        printf("Utilization Ratio > 1, Hence :\n");
        printf(RED "Not schedulable \n" RESET);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    printf("Utilization Ratio : %f\n", utilization);

    float Liu_Leyland_Bound = size * (pow((double)2, (double)((double)1 / (double)size)) - 1);

    printf("Liu_Leyland Bound : %f \n", Liu_Leyland_Bound);

    if (utilization <= Liu_Leyland_Bound)
    {
        printf("As utilization ratio is less than Liu-Leyland Bound, Hence : \n");
        printf(GRN "Schedulable \n" RESET);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }

    else
    {
        printf("As utilization ratio is greater than Liu-Leyland Bound, Hence we need to check further : \n ");
    }

    printf("\n\n");

    pthread_t tid_array[size];

    // priority level loop
    for (int i = 0; i < size; i++)
    {
        int *priority_level_number = (int *)calloc(1, sizeof(int));
        *priority_level_number = i;
        pthread_create(tid_array + i, NULL, runner, priority_level_number);
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

    printf("Status of every level : ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", output_array[i]);
    }

    printf("\n");

    for (int i = 0; i < size; i++)
    {
        if (output_array[i] != 1)
        {
            printf(RED "Not schedulable \n" RESET);
            fflush(stdout);
            exit(0);
        }
    }

    printf("\n");

    printf(GRN "Schedulable \n" RESET);
    fflush(stdout);
}

float find_busy_interval(int level)
{
    float t = 0;
    for (int i = 0; i <= level; i++)
    {
        t += tasks_list[i].execution;
    }

    float right_side;

    while (1)
    {
        right_side = 0;

        for (int i = 0; i <= level; i++)
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

    int *priorty_level = (int *)param; // priority level receieved

    float busy_interval = find_busy_interval(*priorty_level);

    printf("Busy interval of priority level %d is : %f\n", *priorty_level, busy_interval);
    if (busy_interval < 0)
    {
        printf(RED "As negative busy interval , which means it did not converge, so priority level %d will never be satisfied\n" RESET, *priorty_level);
        fflush(stdout);
        pthread_exit(0);
    }

    int number_of_jobs = (int)ceil(busy_interval / tasks_list[*priorty_level].period);

    printf("No. of Jobs priority level %d of lowest priority task will be : %d\n", *priorty_level, number_of_jobs);

    float execution = tasks_list[*priorty_level].execution;
    float period = tasks_list[*priorty_level].period;
    float deadline = tasks_list[*priorty_level].deadline;

    float t = execution;

    float right_side;
    int count_jobs_finished = 0;
    for (int i = 0; i < number_of_jobs; i++)
    {
        while (1)
        {
            right_side = 0;

            float first_part = (count_jobs_finished + 1) * execution;
            float second_part = 0;

            for (int i = 0; i < *priorty_level; i++)
            {
                second_part += ceil((t + count_jobs_finished * period) / tasks_list[i].period) * tasks_list[i].execution;
            }

            right_side = first_part + second_part - count_jobs_finished * period;
            if (t == right_side)
                break;
            t = right_side;
        }

        printf(CYN "Priority Level : %d & Job : %d has t : %f\n" RESET, *priorty_level, count_jobs_finished, t);
        fflush(stdout);

        if (t <= deadline)
        {
            printf(GRN "Priority Level : %d & Job : %d satisfies deadline\n" RESET, *priorty_level, count_jobs_finished);
            count_jobs_finished++;
            t = execution;
        }
        else
        {
            printf(RED "Task : %d & Job : %d does not satisfy deadline\n" RESET, *priorty_level, count_jobs_finished);
            break;
        }
    }

    fflush(stdout);

    if (count_jobs_finished == number_of_jobs)
        output_array[*priorty_level] = 1;

    pthread_exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 50

int matrix[MAX][MAX];
int n;
int magic_constant;

int row_status[MAX];
int col_status[MAX];
int diag_status[2];
int unique_status;

int score = 0;
int max_score;

pthread_mutex_t lock;

/* Update score safely */
void update_score(int valid)
{
    pthread_mutex_lock(&lock);

    if(valid)
        score++;
    else
    {
        score--;
        if(score < 0)
            score = 0;
    }

    pthread_mutex_unlock(&lock);
}

/* Row validation thread */
void *check_rows(void *arg)
{
    for(int i = 0; i < n; i++)
    {
        int sum = 0;

        for(int j = 0; j < n; j++)
            sum += matrix[i][j];

        sleep(1);

        if(sum == magic_constant)
        {
            row_status[i] = 1;
            update_score(1);
        }
        else
        {
            row_status[i] = 0;
            update_score(0);
        }
    }

    printf("Thread ID-%lu: Row checks completed.\n", (unsigned long)pthread_self());
    pthread_exit(NULL);
}

/* Column validation thread */
void *check_columns(void *arg)
{
    for(int i = 0; i < n; i++)
    {
        int sum = 0;

        for(int j = 0; j < n; j++)
            sum += matrix[j][i];

        sleep(1);

        if(sum == magic_constant)
        {
            col_status[i] = 1;
            update_score(1);
        }
        else
        {
            col_status[i] = 0;
            update_score(0);
        }
    }

    printf("Thread ID-%lu: Column checks completed.\n", (unsigned long)pthread_self());
    pthread_exit(NULL);
}

/* Diagonal validation thread */
void *check_diagonals(void *arg)
{
    int sum1 = 0;
    int sum2 = 0;

    for(int i = 0; i < n; i++)
    {
        sum1 += matrix[i][i]; // check main diagonal (top-left to bottom-right)
        sum2 += matrix[i][n-i-1]; // check secondary diagonal (top-right to bottom-left)
    }

    sleep(1);

    if(sum1 == magic_constant)
    {
        diag_status[0] = 1;
        update_score(1);
    }
    else
    {
        diag_status[0] = 0;
        update_score(0);
    }

    if(sum2 == magic_constant)
    {
        diag_status[1] = 1;
        update_score(1);
    }
    else
    {
        diag_status[1] = 0;
        update_score(0);
    }

    printf("Thread ID-%lu: Diagonal checks completed.\n", (unsigned long)pthread_self());
    pthread_exit(NULL);
}

/* Uniqueness validation thread */
void *check_unique(void *arg)
{
    int seen[MAX*MAX+1] = {0};
    int valid = 1;

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            int val = matrix[i][j];

            if(val < 1 || val > n*n || seen[val])
            {
                valid = 0;
                break;
            }

            seen[val] = 1;
        }

        if(!valid)
            break;
    }

    sleep(1);

    unique_status = valid;

    if(valid)
        update_score(1);
    else
        update_score(0);

    printf("Thread ID-%lu: Uniqueness check completed.\n", (unsigned long)pthread_self());
    pthread_exit(NULL);
}

/* Print report */
void print_report()
{
    printf("\n--- Magic Square Report ---\n");

    /* Rows */
    int all_valid = 1;
    for(int i = 0; i < n; i++)
    {
        if(row_status[i] == 0)
        {
            printf("Rows: Row %d Invalid\n", i+1);
            all_valid = 0;
        }
    }
    if(all_valid)
        printf("Rows: All valid\n");

    /* Columns */
    all_valid = 1;
    for(int i = 0; i < n; i++)
    {
        if(col_status[i] == 0)
        {
            printf("Cols: Col %d Invalid\n", i+1);
            all_valid = 0;
        }
    }
    if(all_valid)
        printf("Cols: All valid\n");

    /* Diagonals */
    if(diag_status[0] && diag_status[1])
        printf("Diags: All Valid\n");
    else
    {
        if(!diag_status[0])
            printf("Diag: Main Diag Invalid\n");
        if(!diag_status[1])
            printf("Diags: Secondary Diag Invalid\n");
    }

    /* Uniqueness */
    if(unique_status)
        printf("Unique: Passes\n");
    else
        printf("Unique: Failed (Duplicates found)\n");

    printf("Final Score: %d / %d\n", score, max_score);

    if(score == max_score)
        printf("RESULT: VALID MAGIC SQUARE\n");
    else
        printf("RESULT: INVALID MAGIC SQUARE\n");
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s inputfile\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if(!fp)
    {
        printf("Error opening file.\n");
        return 1;
    }

    fscanf(fp, "%d", &n);

    if (n <= 0 || n > MAX)
    {
        printf("Invalid square size.\n");
        fclose(fp);
        return 1;
    }

    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            fscanf(fp, "%d", &matrix[i][j]);

    fclose(fp);

    magic_constant = n * (n*n + 1) / 2;
    max_score = n + n + 2 + 1;

    pthread_mutex_init(&lock, NULL);

    pthread_t t1, t2, t3, t4;

    pthread_create(&t1, NULL, check_rows, NULL);
    pthread_create(&t2, NULL, check_columns, NULL);
    pthread_create(&t3, NULL, check_diagonals, NULL);
    pthread_create(&t4, NULL, check_unique, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    print_report();

    pthread_mutex_destroy(&lock);

    return 0;
}

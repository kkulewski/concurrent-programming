#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define ROWS 3
#define COLS 3

#define EMPTY 0
#define CIRCLE 10
#define CROSS 100
#define GAME_END -1

#define KEY 666

#define SEM_1 0
#define SEM_2 1

void printBoard(int** board)
{
    printf(" ");
    for (int i = 0; i < COLS; i++)
    {
        printf(" %d", i);
    }
    printf("\n");
    for(int i = 0; i < COLS; i++)
    {
        printf("%d|", i+1);
        for(int j = 0; j < ROWS; j++)
        {
            if (board[i][j] == CIRCLE)
            {
                printf("O");
            }

            if (board[i][j] == CROSS)
            {
                printf("X");
            }

            if (board[i][j] == EMPTY)
            {
                printf(" ");
            }

            printf("|");
        }

        printf("\n");
    }

    printf("\n");
}

int makeMove(int** board, int col, int row, int sign)
{
    if((col < COLS) && (row < ROWS) && (col > 0) && (row > 0) && (board[col][row] == EMPTY))
    {
        board[col][row] = sign;
        return 0;
    }
    else
    {
        printf("Illegal move. Try again:\n");
        return -1;
    }
}

int checkMove(int** board)
{
    int occupiedFields = 0;
    for (int i = 0; i < 3; i++)
    {
        int sumVertical = 0;
        int sumHorizontal = 0;
        int sumDiagonal = 0;
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] != EMPTY)
            {
                occupiedFields += 1;
            }

            sumVertical += board[i][j];
            sumHorizontal += board[j][i];
            sumDiagonal += board[j][j];
        }

        if ((sumVertical == 3 * CIRCLE) || (sumHorizontal == 3 * CIRCLE) || (sumDiagonal == 3 * CIRCLE))
        {
            printf("End - O won\n");
            return GAME_END;
        }

        if ((sumVertical == 3 * CROSS) || (sumHorizontal == 3 * CROSS) || (sumDiagonal == 3 * CROSS))
        {
            printf("End - X won\n");
            return GAME_END;
        }
    }

    if (occupiedFields == COLS * ROWS)
    {
        printf("End - draw\n");
        return GAME_END;
    }

    return 0;
}

int g_sharedMemory, g_semaphores;

void cleanup(int signal)
{
    semctl(g_semaphores, 0, IPC_RMID, 0);
    shmctl(g_sharedMemory, IPC_RMID, 0);
    exit(0);
}

int main()
{
    signal(SIGINT, cleanup);

    g_sharedMemory = shmget(KEY, ROWS * COLS * sizeof(int), 0777|IPC_CREAT);
    int* shMem = shmat(g_sharedMemory, 0, 0);
    int** board = malloc(ROWS * sizeof(board[0]));

    for(int i = 0; i < ROWS; i++)
    {
        board[i] = (shMem + i*ROWS);
    }

    struct sembuf attemptMoveX={SEM_1, -1, 0},
                  unlockO={SEM_2, 1, 0},
                  attemptMoveO={SEM_2, -1, 0},
                  unlockX={SEM_1, 1, 0},
                  *attemptMove,
                  *unlockOpponent;

    int playerSign = 0;
    if((g_semaphores = semget(KEY, 2, 0777|IPC_CREAT|IPC_EXCL)) > 0)
    {
        printf("You are 1st player - X\n");
        playerSign = CROSS;
        semctl(g_semaphores, SEM_1, SETVAL, 0);
        semctl(g_semaphores, SEM_2, SETVAL, 0);
        attemptMove = &attemptMoveX;
        unlockOpponent = &unlockO;

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                board[i][j] = EMPTY;
            }
        }
    }
    else
    {
        printf("You are 2nd player - O\n");
        playerSign = CIRCLE;
        g_semaphores = semget(KEY, 2, 077|IPC_CREAT);
        attemptMove = &attemptMoveO;
        unlockOpponent = &unlockX;
        semop(g_semaphores, unlockOpponent, 1);
    }

    while(1)
    {
        printf("Waiting for opponent...\n");
        semop(g_semaphores, attemptMove, 1);

        printf("Board:\n");
        printBoard(board);

        if (checkMove(board) == GAME_END)
        {
            cleanup(0);
        }

        int row, col;
        do
        {
            printf("Your turn [ROW COL]: ");
            scanf("%d %d", &row, &col);
        } while(makeMove(board, row-1, col-1, playerSign) < 0);

        printBoard(board);
        semop(g_semaphores, unlockOpponent, 1);
    }

    free(board);
    cleanup(0);
}

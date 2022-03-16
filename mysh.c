// Pedro Roman
// COP 4600
// Homework 3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

// structure definition for linked liQst
typedef struct Node
{
    int level;
    int size;
    struct Node *next;
} node;

const char *COMMAND_HISTORY = "command_history.txt";
int lineCount(void);
void terminateBackgroundProgram(pid_t pid, node *backgroundList, bool flag);

int byeBye(void)
{
    exit(0);
}

// function for replay function
char *replayCommand(int replayLine)
{
    FILE *fp;
    fp = fopen(COMMAND_HISTORY, "r"); // Read mode
    char *acquiredLine;
    int line;

    char *buffer;
    size_t bufferSize = 32;
    buffer = (char *)malloc(bufferSize * sizeof(char));

    if (fp == NULL)
    {
        printf("No history\n");
        return NULL;
    }

    for (int i = replayLine; i != 0; i--)
        getline(&buffer, &bufferSize, fp);

    acquiredLine = (char *)malloc(bufferSize * sizeof(char));

    for (int i = 0; i < strlen(buffer); i++)
        acquiredLine[i] = buffer[i + 3];

    free(buffer);
    return acquiredLine;
}

// function to handle an invalid command input
int invalidCommand()
{
    printf("Invalid Argument, Please try again\n");
    return 1;
}

// function to save the history of a command called
bool saveHistory(const char *command, int called)
{
    FILE *fp;
    fp = fopen(COMMAND_HISTORY, "a"); // Append mode

    if (fp == NULL)
        return false;

    fprintf(fp, "%d: %s\n", called, command);
    fclose(fp);
    return true;
}

// function for history command
bool showHistory()
{
    FILE *fp;
    fp = fopen(COMMAND_HISTORY, "r"); // Read mode
    if (fp == NULL)
        return false;

    char c = fgetc(fp);
    while (c != EOF)
    {
        printf("%c", c);
        c = fgetc(fp);
    }
    fclose(fp);
    return true;
}

// helper function to remove quotation marks from a string
char *removeQuotes(char *s1)
{
    if (s1 == NULL)
        return NULL;
    size_t len = strlen(s1);
    if (s1[0] == '"' && s1[len - 1] == '"')
    {
        s1[len - 1] = '\0';
        memmove(s1, s1 + 1, len - 1);
    }
    return s1;
}

// cfunction to clear the history when [-c] argument is passed along with history command
void clearHistory(void)
{
    FILE *fp;
    fp = fopen(COMMAND_HISTORY, "w");
    if (fp == NULL)
        return;
    fclose(fp);
}

// function to read an input line from the terminal
char *readInput(void)
{
    char *buffer;
    size_t bufferSize = 32;
    buffer = (char *)malloc(bufferSize * sizeof(char));
    if (buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
    getline(&buffer, &bufferSize, stdin);
    return buffer;
}

// function that takes in a string and breaks it up into tokens
char **tokenize(char *line)
{
    char **args = malloc(sizeof(char *) * 10);
    char *token;
    int counter = 0;

    // collects "start"
    token = strtok(line, " ");

    // collects arguments
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        args[counter] = removeQuotes(token);
        counter++;
    }

    args[counter] = NULL;

    return args;
}
// function that starts provided function
int startProgram(char **args)
{
    char cwd[256];
    pid_t pid;
    int status, success;

    if (args[0][0] != '/')
    {
        getcwd(cwd, sizeof(cwd));
        args[0] = cwd;
    }
    pid = fork();
    if (pid < 0)
    {
        printf("Error");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, &status, 0);
    }
    else
    {
        success = execv(args[0], args);
        if (success == -1)
            printf("Unable to start program\n");

        free(args);
        return 0;
    }

    free(args);

    return 0;
}

// adds a background process pid to the linked list
void addBProcess(node *backgroundList, int pid)
{
    if (backgroundList == NULL)
        return;

    node *temp = backgroundList;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = calloc(1, sizeof(node));
    temp->next->level = pid;
    backgroundList->size++;
}

// destroys a linked list
void destroyList(node *backgroundList)
{
    if (backgroundList == NULL)
        return;

    destroyList(backgroundList->next);
    free(backgroundList);

    return;
}

// terminates all background processes
void terminateAll(node *backgroundList)
{
    if (backgroundList == NULL)
        return;

    if (backgroundList->size == 0)
        printf("No processes to terminate.\n");
    else if (backgroundList->size == 1)
        printf("Terminating 1 process: ");
    else
        printf("Terminating %d processes: ", backgroundList->size);

    node *temp = backgroundList;
    while (temp->next != NULL)
    {
        if (temp->level != 0)
            printf("%d ", temp->level);
        pid_t pid = temp->level;
        // terminateBackgroundProgram(pid, backgroundList, true);
        temp = temp->next;
    }
    printf("%d\n", temp->level);
    destroyList(backgroundList->next);
    backgroundList->size = 0;
}

// function for backgorund program command
int backgroundProgram(char **args)
{
    char cwd[256];
    pid_t pid;
    int status, success, processPID;

    // background
    if (args[0][0] != '/')
    {
        getcwd(cwd, sizeof(cwd));
        args[0] = cwd;
    }

    pid = fork();
    if (pid != 0)
    {
        printf("PID: %d\n", pid);
        processPID = pid;
    }

    if (pid < 0)
    {
        printf("Error");
        exit(1);
    }
    else if (pid == 0)
    {
        execv(args[0], args);
        if (success == -1)
            printf("Unable to start program\n");
    }

    return processPID;
}


// function for repeat function
void repeatFunction(char *line, node *backgroundList)
{
    char **args;
    int repeatAmount;
    char *temp;
    int pid;

    args = tokenize(line);

    repeatAmount = atoi(args[0]);

    args += 1;

    for (int i = 0; i < repeatAmount; i++)
    {
        pid = backgroundProgram(args);
        addBProcess(backgroundList, pid);
    }
}

// function to terminate a background process
void terminateBackgroundProgram(pid_t pid, node *backgroundList, bool flag)
{
    if (kill(pid, SIGKILL) == 0)
    {
        if (!flag)
            printf("Program has been terminated.\n");
        node *temp = backgroundList;
        while (temp->next != NULL)
        {
            if (temp->next->level == pid)
            {
                if (temp->next->next == NULL)
                {
                    free(temp->next);
                    temp->next = NULL;
                }
                else
                {
                    node *temp2 = temp->next;
                    temp->next = temp->next->next;
                    free(temp2);
                }
            }
            else
                temp = temp->next;
        }
    }
    else
        printf("Unable to terminate the background process.\n");
}

void shellLoop(void)
{   
    char *line, *command;
    char byebye[] = "byebye";
    char history[] = "history";
    char historyClear[] = "history [-c]";
    char replay[] = "replay";
    char start[] = "start";
    char background[] = "background";
    char terminate[] = "terminate";
    char terminateall[] = "terminateall";
    char repeat[] = "repeat";
    int called = lineCount();
    bool replayed = false;

    node *backgroundList = calloc(1, sizeof(node));

    while (1)
    {
        if (!replayed)
        {
            printf("# ");
            line = readInput();
        }
        else if (replayed)
            replayed = false;

        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        called++;

        char lineCpy[strlen(line)];
        strcpy(lineCpy, line);
        command = strtok(lineCpy, " ");

        if (command == NULL)
            continue;

        if (strcmp(command, byebye) == 0)
        {
            saveHistory(line, called);
            byeBye();
        }
        else if (strcmp(line, historyClear) == 0)
        {
            clearHistory();
            showHistory();
            called = 0;
        }
        else if (strcmp(command, repeat) == 0)
        {

            repeatFunction(line, backgroundList);
            saveHistory(line, called);
        }
        else if (strcmp(command, history) == 0)
        {
            saveHistory(line, called);
            showHistory();
        }
        else if (strcmp(command, replay) == 0)
        {
            command = strtok(NULL, " ");
            if (command == NULL)
            {
                saveHistory(line, called);
                invalidCommand();
                continue;
            }

            saveHistory(line, called);
            char *ogLine = line;
            line = replayCommand(atoi(command));
            free(ogLine);
            replayed = true;
        }
        else if (strcmp(command, start) == 0)
        {
            command = strtok(NULL, " ");
            if (command == NULL)
            {
                saveHistory(line, called);
                invalidCommand();
                continue;
            }

            saveHistory(line, called);
            char **token = tokenize(line);
            startProgram(token);
        }
        else if (strcmp(command, background) == 0)
        {
            command = strtok(NULL, " ");
            if (command == NULL)
            {
                saveHistory(line, called);
                invalidCommand();
                continue;
            }

            saveHistory(line, called);
            char **token = tokenize(line);
            int pid = backgroundProgram(token);
            addBProcess(backgroundList, pid);
        }
        else if (strcmp(command, terminate) == 0)
        {
            command = strtok(NULL, " ");
            if (command == NULL)
            {
                saveHistory(line, called);
                invalidCommand();
                continue;
            }

            saveHistory(line, called);
            terminateBackgroundProgram(atoi(command), backgroundList, false);
        }
        else if (strcmp(line, terminateall) == 0)
        {
            saveHistory(line, called);
            terminateAll(backgroundList);
        }
        else
        {
            saveHistory(line, called);
            invalidCommand();
        }
        if (!replayed)
            free(line);
    }
}


int lineCount()
{
    FILE *fp;
    int count = 0; 
    char c;       

    // Open the file
    fp = fopen(COMMAND_HISTORY, "r");

    // Check if file exists
    if (fp == NULL)
    {
        return 0;
    }

    // Extract characters from file and store in character c
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

    // Close the file
    fclose(fp);
    return count;
}

// main function
int main(void)
{
    shellLoop();
    return 0;
}
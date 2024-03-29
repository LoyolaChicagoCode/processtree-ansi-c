#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "processtree.h"

/*
 * ProcInfo Functions: These functions are used to create ProcInfo records and do the work
 * of building the tree structure.
 */
ProcInfo* ProcInfo_new(int pid, int ppid, char* command) {
    ProcInfo *instance = (ProcInfo*) malloc(sizeof(ProcInfo));
    instance->pid = pid;
    instance->ppid = ppid;
    instance->command = malloc(strlen(command) * sizeof(char)+1);
    strcpy(instance->command, command);
    instance->parent = instance->child = instance->sibling = NULL;
    return instance;
}


void ProcInfo_add_parent(ProcInfo* process, ProcInfo* parent) {
    if (process != NULL)
        process->parent = parent;
}

void ProcInfo_add_child(ProcInfo* process, ProcInfo* child) {
    if (process->child == NULL)
        process->child = child;
    else {
        child->sibling = process->child;
        process->child = child;
    }
}

/*
 * ProcInfoArray: These functions work on the array of ProcInfo* structures. In our implementation,
 * we take advantage of arrays (which could eventually be resizable) so we can take advantage of
 * qsort(), which allows the ProcInfo tree itself to be built in linear time.
 */

int ProcInfoArray_find_by_pid(ProcInfo* piArray[], int n, int pid) {
    int min =0;
    int max=n-1;
    for(;;) {
        int mid = (min+max) / 2;
        if (pid > piArray[mid]->pid)
            min = mid + 1;
        else
            max = mid - 1;
        if (piArray[mid]->pid == pid)
            return mid;
        if (min > max)
            break;
    }
    return -1;
}

void ProcInfoArray_create_threaded_tree(ProcInfo* piArray[], int n) {
    int i;
    for (i=0; i < n; i++) {
        ProcInfo *pidInfo = piArray[i];
        ProcInfo *ppidInfo = NULL;
        int ppidPos = ProcInfoArray_find_by_pid(piArray, n, pidInfo->ppid);
        if (ppidPos >= 0) {
            ppidInfo = piArray[ppidPos];
            ProcInfo_add_child(ppidInfo, pidInfo);
        }
        ProcInfo_add_parent(pidInfo, ppidInfo);
    }
}

/*
 * These functions allow the ProcInfo* array to be printed. It is entirely possible that there are
 * multiple trees, especially on OSs like Minix where there are multiple "root" server processes.
 */
void ProcInfo_print_recursive(ProcInfo* pi, int level)
{
    int indent;
    ProcInfo* next;
    for (indent=0; indent < level; indent++)
        printf(" ");
    printf("%d: %s\n", pi->pid, pi->command);
    next = pi->child;
    while (next != NULL) {
        ProcInfo_print_recursive(next, level+1);
        next = next->sibling;
    }
}

void ProcInfoArray_print(ProcInfo* piArray[], int n)
{
    int i;
    printf("Printing pstree\n");
    for (i=0; i < n; i++) {
        if (piArray[i]->parent == NULL) { /* root or server process */
            printf("Printing tree for forest %d\n", piArray[i]->pid);
            ProcInfo_print_recursive(piArray[i], 0);
        }
    }
}

int ProcInfo_qsort_compare_pid(const void *a, const void *b) {
    ProcInfo **piA = (ProcInfo **)a;
    ProcInfo **piB = (ProcInfo **)b;
    return (*piA)->pid - (*piB)->pid;
}


/*
 * These two functions help us to hide some of the evil of working with
 * strtok(). getTokens() fills an array of tokens so they can be processed
 * on a per-field basis. findToken() is used to find the position of headers,
 * notably the "PID", "PPID", and "CMD" fields.
 */

int StringArray_find_token(char *tokens[], char *searchToken, int max_tokens) {
    int i;
    for (i=0; i < max_tokens; i++) {
        if (strcmp(tokens[i], searchToken) == 0) return i;
    }
    return -1;
}

int StringArray_parse_tokens(char *tokens[], char* buf, int max_tokens, int cmd_col) {
    char *buffer = buf;
    int i=0;
    for(;;) {
        if (i < cmd_col)
            tokens[i] = strtok(buffer, " \t\n");
        else
            tokens[i] = strtok(buffer, "\n");
        buffer = NULL;
        if (tokens[i] == NULL)
            break;
        i++;
        if (i >= max_tokens)
            break;
    }
    return i;
}


/* Make this global for proper initialization.
   TODO: Replace with dynamic allocation.
 */

ProcInfo *pi[MAX_PROCS];

/* TODO: Put in separate timing module. */
double getElapsedTime(struct timeval *t1, struct timeval *t2)
{
    double elapsedTime = (t2->tv_sec - t1->tv_sec) * 1000.0;
    elapsedTime += (t2->tv_usec - t1->tv_usec) / 1000.0;
    return elapsedTime;
}

void markTime(struct timeval *t)
{
    gettimeofday(t, NULL);
}


void pstree() {
    char buf[MAX_INPUT_LINE];
    char* tokens[MAX_TOKENS];
    int token_count, pidCol, ppidCol, cmdCol, commandCol, processTableCount = 0;
    FILE *pipe = stdin;
    struct timeval io1, io2, qs1, qs2, tree1, tree2, gen1, gen2;

    /* The first line of input must contain headers PID, PPID, and CMD.
     * If missing, we can't continue.
     */

    markTime(&io1);
    if (fgets(buf, MAX_INPUT_LINE, pipe) != NULL) {
        token_count = StringArray_parse_tokens(tokens, buf, MAX_TOKENS, MAX_TOKENS);
        pidCol = StringArray_find_token(tokens, "PID", token_count);
        ppidCol = StringArray_find_token(tokens, "PPID", token_count);
        cmdCol = StringArray_find_token(tokens, "CMD", token_count);
        commandCol = StringArray_find_token(tokens, "COMMAND", token_count);
    } else
        return;

    /* This is just some additional error checking. Must have CMD or COMMAND
     * header.
     */

    if (cmdCol < 0 && commandCol < 0) {
        fprintf(stderr, "Cannot find CMD or COMMAND header.\n");
        exit(1);
    }

    if (cmdCol < 0)
        cmdCol = commandCol;

    if (pidCol < 0) {
        fprintf(stderr, "Cannot find PID header.\n");
        exit(1);
    }

    if (ppidCol < 0) {
        fprintf(stderr, "Cannot find PPID header.\n");
        exit(1);
    }

    /*
     * Read all lines of input into a ProcInfo* array.
     */

    while (fgets(buf, MAX_INPUT_LINE, pipe) != NULL) {
        char *pid, *ppid, *cmd;
        token_count = StringArray_parse_tokens(tokens, buf, MAX_TOKENS, cmdCol);
        pid = tokens[pidCol];
        ppid = tokens[ppidCol];
        cmd = tokens[cmdCol];
        if (pid == NULL || ppid == NULL || cmd == NULL)
            continue;
        if (processTableCount < MAX_PROCS)
            pi[processTableCount++] = ProcInfo_new(atoi(pid), atoi(ppid), cmd);
        else {
            printf("fatal: Please recompile with MAX_PROCS > %d\n", MAX_PROCS);
            exit(1);
        }
    }

    markTime(&io2);
    /*
     * Sort by the ProcInfo.pid field.
     */
    markTime(&qs1);
    qsort(pi, processTableCount, sizeof(ProcInfo *), ProcInfo_qsort_compare_pid);
    markTime(&qs2);
    /*
     qsort(pi, processTableCount, sizeof(ProcInfo *), ProcInfo_qsort_compare_pid);
     */

    /*
     * Thread the ProcInfo* with parent, child, and sibling information.
     */

    markTime(&tree1);
    ProcInfoArray_create_threaded_tree(pi, processTableCount);
    markTime(&tree2);

    /*
     * Print the tree, recursively. Done!
     */
    markTime(&gen1);
    ProcInfoArray_print(pi, processTableCount);
    markTime(&gen2);

    fprintf(stderr, "I/O time %f\n", getElapsedTime(&io1, &io2));
    fprintf(stderr, "qsort time %f\n", getElapsedTime(&qs1, &qs2));
    fprintf(stderr, "tree building time %f\n", getElapsedTime(&tree1, &tree2));
    fprintf(stderr, "printing time %f\n", getElapsedTime(&gen1, &gen2));


}

int main(int argc, char* argv[])
{
    struct timeval t1, t2;
    markTime(&t1);
    pstree();
    markTime(&t2);
    fprintf(stderr, "Total elapsed time %f\n", getElapsedTime(&t1, &t2));
    return 0;
}

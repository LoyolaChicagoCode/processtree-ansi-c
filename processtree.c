#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processtree.h"

struct ProcInfo {
   int pid;
   int ppid;
   char *command;
   struct ProcInfo* parent;
   struct ProcInfo* child;
   struct ProcInfo* sibling;
};

typedef struct ProcInfo ProcInfo;

ProcInfo* ProcInfo_new(int pid, int ppid, char* command) {
  ProcInfo *instance = (ProcInfo*) malloc(sizeof(ProcInfo));
  instance->pid = pid;
  instance->ppid = ppid;
  instance->command = malloc(strlen(command) * sizeof(char));
  strcpy(instance->command, command);
  instance->parent = instance->child = instance->sibling = NULL;
  return instance;
}

/* 
   This is a binary search in the ProcInfo array table by "pid". I wish there
   were a generic version of this, much like we have for qsort(). Perhaps
   there is, and I just don't know it. :-)
*/
int ProcInfoArray_find(ProcInfo* piArray[], int n, int pid) {
  int min =0;
  int max=n-1;
  while (1) {
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

void ProcInfo_addParent(ProcInfo* process, ProcInfo* parent) {
  process->parent = parent;   
}

void ProcInfo_addChild(ProcInfo* process, ProcInfo* child) {
  if (process->child == NULL)
    process->child = child;
  else {
    child->sibling = process->child;
    process->child = child;
  }
}

void ProcInfoArray_threadPID(ProcInfo* piArray[], int n) {
  int i;
  for (i=0; i < n; i++) {
    ProcInfo *pidInfo = piArray[i];
    ProcInfo *ppidInfo = NULL;
    int ppidPos = ProcInfoArray_find(piArray, n, pidInfo->ppid);
    if (ppidPos >= 0) {
      ppidInfo = piArray[ppidPos];
      ProcInfo_addChild(ppidInfo, pidInfo);
    }
    ProcInfo_addParent(pidInfo, ppidInfo);
  }
}

/*
  These two functions are responsible for printing the actual tree, once it
  has been constructed. ProcInfoArray_print() is the driver, because it is
  possible that many processes can act as the root. This is especially true
  in Minix.
*/

void ProcInfo_printRecursive(ProcInfo* pi, int level)
{
  int indent;
  ProcInfo* next;
  for (indent=0; indent < level; indent++)
    printf(" ");
  printf("%d -> %s\n", pi->pid, pi->command);
  next = pi->child;
  while (next != NULL) {
    ProcInfo_printRecursive(next, level+1);
    next = next->sibling;
  }
}

void ProcInfoArray_print(ProcInfo* piArray[], int n)
{
  int i;
  printf("Printing pstree\n");
  for (i=0; i < n; i++) {
    if (piArray[i]->parent == NULL) /* root or server process */ {
      printf("Printing tree for forest %d\n", piArray[i]->pid);
      ProcInfo_printRecursive(piArray[i], 0);
    }
  }
}

/*
  This function is used to compare by ProcInfo.pid when using qsort(). I 
  sort the processes on pid to do the threaded tree construction on the
  fly.
*/

int ProcInfo_cmp(const void *a, const void *b) {
  ProcInfo **piA = (ProcInfo **)a;
  ProcInfo **piB = (ProcInfo **)b;
  return (*piA)->pid - (*piB)->pid;
}


/* 
   These two functions help us to hide some of the evil of working with
   strtok(). getTokens() fills an array of tokens so they can be processed
   on a per-field basis. findToken() is used to find the position of headers,
   notably the "PID", "PPID", and "CMD" fields.
*/

int findToken(char *tokens[], char *searchToken, int max_tokens) {
  int i;
  for (i=0; i < max_tokens; i++) {
    if (strcmp(tokens[i], searchToken) == 0) return i;
  }
  return -1;
}


int getTokens(char* buf, char *tokens[], int max_tokens) {
  char *buffer = buf, *nextToken;
  int i=0;
  for(;;) {
    tokens[i] = strtok(buffer, " \t\n");
    buffer = NULL;
    if (tokens[i] == NULL)
      break;
    i++;
    if (i >= max_tokens)
      break;
  }
  return i;
}

void doProcessTree() {
  ProcInfo *pi[MAX_PROCS];
  char buf[MAX_INPUT_LINE];
  char* tokens[MAX_TOKENS];
  int pos, i, token_count, pidCol, ppidCol, cmdCol, processTableCount = 0;
  FILE *pipe;
  pipe = popen("ps -ef", "r");

  /* Find the headers PID, PPID, and CMD. If missing, we can't continue. */
  if (fgets(buf, MAX_INPUT_LINE, pipe) != NULL) {
    token_count = getTokens(buf, tokens, MAX_TOKENS);
    pidCol = findToken(tokens, "PID", token_count);
    ppidCol = findToken(tokens, "PPID", token_count);
    cmdCol = findToken(tokens, "CMD", token_count);
  } else 
    return;

  /* 
     Build an array of the input from "ps". This allows us to use qsort()
     to sort by the pid field.
  */
  while (fgets(buf, MAX_INPUT_LINE, pipe) != NULL) {
    char *pid, *ppid, *cmd;
    token_count = getTokens(buf, tokens, MAX_TOKENS);
    pid = tokens[pidCol];
    ppid = tokens[ppidCol];
    cmd = tokens[cmdCol];
    if (pid == NULL || ppid == NULL || cmd == NULL)
      continue;
    pi[processTableCount++] = ProcInfo_new(atoi(pid), atoi(ppid), cmd);
  }
  qsort(pi, processTableCount, sizeof(ProcInfo *), ProcInfo_cmp);

  /* Create a process "tree" from this array. */
  ProcInfoArray_threadPID(pi, processTableCount);

  /*
    Print the process "tree". It is possible that more than one tree
    will be found, especially on Minix where there is a forest (separate
    trees 
  */
  ProcInfoArray_print(pi, processTableCount);
}

main()
{
  doProcessTree();
}

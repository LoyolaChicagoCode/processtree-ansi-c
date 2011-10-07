#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
   instance->command = strdup(command);
   instance->parent = instance->child = instance->sibling = NULL;
   return instance;
}


/* assumption: array is sorted by pid */
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
      /* only child */
      process->child = child;
   else {
      /* child with a new sibling/quick way of getting a list */
      child->sibling = process->child;
      process->child = child;
   }
}

void ProcInfoArray_threadPID(ProcInfo* piArray[], int n) {
   int i;
   for (i=0; i < n; i++) {
      ProcInfo *pidInfo = piArray[i];
      int ppidPos = ProcInfoArray_find(piArray, n, pidInfo->ppid);
      ProcInfo *ppidInfo = NULL;
      if (ppidPos >= 0) {
         ppidInfo = piArray[ppidPos];
         ProcInfo_addChild(ppidInfo, pidInfo);
      }
      ProcInfo_addParent(pidInfo, ppidInfo);
   }
}


void ProcInfo_printRecursive(ProcInfo* pi, int level)
{
   int indent;
   for (indent=0; indent < level; indent++)
      printf(" ");
   printf("%d -> %s\n", pi->pid, pi->command);
   ProcInfo* next = pi->child;
   while (next != NULL) {
      ProcInfo_printRecursive(next, level+1);
      next = next->sibling;
   }
}

void ProcInfoArray_print(ProcInfo* piArray[], int n)
{
   printf("Printing pstree\n");
   int i;
   for (i=0; i < n; i++) {
      if (piArray[i]->parent == NULL) /* root or server process */ {
         printf("Printing tree for forest %d\n", piArray[i]->pid);
         ProcInfo_printRecursive(piArray[i], 0);
      }
   }
}

int ProcInfo_cmp(const void *a, const void *b) {
   ProcInfo **piA = (ProcInfo **)a;
   ProcInfo **piB = (ProcInfo **)b;
   return (*piA)->pid - (*piB)->pid;
}

#define MAX_PROCS 100
#define MAX_INPUT_LINE 10000 
#define MAX_TOKENS 20

int findToken(char *tokens[], char *searchToken, int max_tokens) {
   int i;
   for (i=0; i < max_tokens; i++) {
      if (strcmp(tokens[i], searchToken) == 0) return i;
   }
   return -1;
}


int getTokens(char* buf, char *tokens[], int max_tokens) {
   char *buffer = buf;
   char *nextToken;
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
   return i; /* number of tokens found or max */
}

void quickBSTest() {
   int pos, i;
   ProcInfo *pi[MAX_PROCS];
   char buf[MAX_INPUT_LINE];
   char* tokens[MAX_TOKENS];
   FILE *pipe;
   int token_count;
   int pidCol, ppidCol, cmdCol;
   int processTableCount = 0;
   pipe = popen("ps -ef", "r");
 

   if (fgets(buf, MAX_INPUT_LINE, pipe) != NULL) {
      token_count = getTokens(buf, tokens, MAX_TOKENS);
      /* printf("tokens found = %d\n", token_count); */
      pidCol = findToken(tokens, "PID", token_count);
      ppidCol = findToken(tokens, "PPID", token_count);
      cmdCol = findToken(tokens, "CMD", token_count);
      /* printf("PID column %d PPID column %d CMD column %d\n", pidCol, ppidCol, cmdCol); */
   }  else 
      return;
   
   while (fgets(buf, MAX_INPUT_LINE, pipe) != NULL) {
      char *pid, *ppid, *cmd;
      token_count = getTokens(buf, tokens, MAX_TOKENS);
      pid = tokens[pidCol];
      ppid = tokens[ppidCol];
      cmd = tokens[cmdCol]; /* just the command name */
      if (pid == NULL || ppid == NULL || cmd == NULL)
         continue;
      /* printf("pid = %s ppid = %s cmd = %s\n", pid, ppid, cmd); */
      pi[processTableCount++] = ProcInfo_new(atoi(pid), atoi(ppid), cmd);
   }
   qsort(pi, processTableCount, sizeof(ProcInfo *), ProcInfo_cmp);

   for (i=0; i < processTableCount; i++) {
      pos = ProcInfoArray_find(pi, processTableCount, pi[i]->pid);
   }
   ProcInfoArray_threadPID(pi, processTableCount);
   ProcInfoArray_print(pi, processTableCount);
}


main()
{
   quickBSTest();
}

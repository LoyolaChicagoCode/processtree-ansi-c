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
         printf("dbg: linking %d as child of %d\n", pidInfo->pid, ppidInfo->pid);
      }
      ProcInfo_addParent(pidInfo, ppidInfo);
   }
}


void ProcInfo_printRecursive(ProcInfo* pi, int level)
{
   int indent;
   for (indent=0; indent < level; indent++)
      printf(" ");
   printf("PID %d\n", pi->pid);
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

void quickBSTest() {
   int pos, i;
   ProcInfo *pi[6];

   pi[0] = ProcInfo_new(2, 1, "ls -1");
   pi[1] = ProcInfo_new(8, 1, "ls -1");
   pi[2] = ProcInfo_new(12, 2, "ls -1");
   pi[3] = ProcInfo_new(3, 1, "ls -1");
   pi[4] = ProcInfo_new(10, 2, "ls -1");
   pi[5] = ProcInfo_new(13, 8, "ls -1");

   qsort(pi, 6, sizeof(ProcInfo *), ProcInfo_cmp);

   for (i=0; i < 6; i++) {
      printf("after sorting at pos %d = PID %d\n", i, pi[i]->pid);
   }
   pos = ProcInfoArray_find(pi, 6, 25);
   printf("Found %d at position %d (should be -1)\n", 25, pos);

   for (i=0; i < 6; i++) {
      pos = ProcInfoArray_find(pi, 6, pi[i]->pid);
      printf("Found %d at position %d (should be %d)\n", pi[i]->pid, pos, i);
   }
   ProcInfoArray_threadPID(pi, 6);
   ProcInfoArray_print(pi, 6);
}


main()
{
   quickBSTest();
}

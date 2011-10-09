#ifndef _PROCESSTREE_H_
#define _PROCESSTREE_H_

#define MAX_INPUT_LINE 5000
#define MAX_TOKENS 50
#define MAX_PROCS 32768

struct ProcInfo {
   int pid;
   int ppid;
   char *command;
   struct ProcInfo* parent;
   struct ProcInfo* child;
   struct ProcInfo* sibling;
};

typedef struct ProcInfo ProcInfo;

#endif

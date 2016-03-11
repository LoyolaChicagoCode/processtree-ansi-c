/* C version of fakeps.py
 * (gives a good idea of why you don't want your native collections interpreted)
 * Simply enumerates the edges.
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

  int n = atoi(argv[1]);
  int cpid;

  if (argc != 2) {
    printf("usage: fakeps n; where n is the number of processes you want");
    exit(1);
  }

  /* seed random number generator (not really needed, but ok) */
  srand(time(NULL));
  
  printf("%-10s %-10s %-10s\n", "PID", "PPID", "CMD");
  printf("%-10d %-10d Fake Process %d\n", 1, 0, 1);
  for (cpid = 2; cpid <= n; ++ cpid) {
    int ppid = 1 + rand() % (cpid - 1);
    printf("%-10d %-10d Fake Process %d\n", cpid, ppid, cpid);
  }
}

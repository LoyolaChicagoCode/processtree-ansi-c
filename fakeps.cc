/* C++ version of fakeps.py
 * (gives a good idea of why you don't want your native collections interpreted)
 */

#include <time.h>
#include <stdlib.h>

#include <unordered_map>
#include <vector>
#include <iostream>

using namespace std;

bool mapHasKey(unordered_map<int, vector<int> > &m, int key) 
{
   unordered_map<int, vector<int> >::iterator it = m.find(key);
   return it != m.end();
}

int main(int argc, char* argv[]) {

   if (argc < 2) {
      // usage
      printf("usage: fakeps n; where n is the number of processes you want");
      exit(1);
   }

   int n = atoi(argv[1]);
   vector<int> empty;
   unordered_map<int, vector<int> > ps;


   // Set up the initial processes 0 and 1 to be Unix like.
   ps[0] = empty;
   ps[0].push_back(1);
   ps[1] = empty;

   // seed random number generator (not really needed, but ok)

   srand(time(NULL));
   int i = 2;
   while (i <= n) {
     int nextPid = i;
     int randomKey;
     do {
       randomKey = 1 + rand() % ps.size();
     } while (randomKey == nextPid);
     if (!mapHasKey(ps, nextPid)) {
       ps[nextPid] = empty;
     }

     ps[randomKey].push_back(nextPid);

     //cout << "Creating PID " << randomKey << " with child " << nextPid << endl;
     i++;
   }

   printf("%-10s %-10s %-10s\n", "PID", "PPID", "CMD");
   for(unordered_map<int, vector<int> >::const_iterator it = ps.begin(); it != ps.end(); ++it)
   {
      int pid = it->first;
      for (int i=0; i < it->second.size(); i++) {
         int cpid = it->second[i];
         printf("%-10d %-10d Fake Process %d\n", cpid, pid, cpid);
      }
   }
}

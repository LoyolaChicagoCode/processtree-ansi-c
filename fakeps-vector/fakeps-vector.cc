/* C++ version of fakeps.py
 * (gives a good idea of why you don't want your native collections interpreted)
 * Preallocates vector of required size.
 */

#include <time.h>
#include <cstdlib>
#include <vector>
#include <cstdio>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("usage: fakeps n; where n is the number of processes you want");
        exit(1);
    }

    int n = atoi(argv[1]);

    vector< vector<int> > ps(n + 1);

    // Set up the initial processes 0 and 1 to be Unix like.
    ps[0].push_back(1);

    // seed random number generator (not really needed, but ok)
    srand(time(NULL));

    for (int nextPid = 2; nextPid <= n; ++ nextPid) {
        int randomKey = 1 + rand() % (nextPid - 1);
        ps[randomKey].push_back(nextPid);
    }

    printf("%-10s %-10s %-10s\n", "PID", "PPID", "CMD");
    for(int ppid = 0; ppid <= n; ++ ppid) {
        for (int i = 0; i < ps[ppid].size(); ++ i) {
            int cpid = ps[ppid][i];
            printf("%-10d %-10d Fake Process %d\n", cpid, ppid, cpid);
        }
    }
}

#include <time.h>
#include <stdlib.h>

#include <map>
#include <vector>
#include <iostream>

using namespace std;

bool mapHasKey(map<int, vector<int> > &m, int key) 
{
   map<int, vector<int> >::iterator it = m.find(key);
   if (it != m.end())
      cout << key << "is in map" << endl;
   else
      cout << key << "is not in map" << endl;
   return it != m.end();
}

int main(int argc, char* argv[]) {

   if (argc < 2) {
      // usage
      exit(1);
   }
   int n = atoi(argv[1]);
   vector<int> empty;
   map<int, vector<int> > ps;

   ps[0] = empty;
   ps[0].push_back(1);
   ps[0].push_back(2);
   ps[1].push_back(3);

   for(map<int, vector<int> >::const_iterator it = ps.begin(); it != ps.end(); ++it)
   {
      cout << "Iterating map entry: " << it->first << endl;
      for (int i=0; i < it->second.size(); i++) {
         cout << it->second[i] << endl;
      }
      cout << endl;
   }

   cout << mapHasKey(ps, 0) << endl;
   cout << mapHasKey(ps, 1) << endl;
   cout << mapHasKey(ps, 2) << endl;
   cout << mapHasKey(ps, 3) << endl;

   srand(time(NULL));
   int r = rand();
   cout << "Random number " << r << endl;
   r = rand();
   cout << "Random number " << r << endl;
}

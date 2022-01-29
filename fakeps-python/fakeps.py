#
# fakeps.py: A program that generates fake ps list for stress testing.
#

import random
import sys

n = int(sys.argv[1])
# usage: fakeps n (where n = number of process table entries)

ps = { 0 : [1], 1 : [] }

i = 2
while i <= n:
   nextPid = i
   psKeys = ps.keys()
   randomKey = psKeys[random.randint(1, len(psKeys)-1)]
   if not nextPid in ps:
      ps[nextPid] = []
   ps[randomKey].append(nextPid)
   i = i + 1

print "%-10s %-10s %-10s" % ("PID","PPID","CMD")
for (pid, children) in ps.items():
   for cpid in children:
      print "%-10s %-10s %s" % (cpid,pid,"Fake Process %d" % cpid)

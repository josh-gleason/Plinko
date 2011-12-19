#!/bin/bash

# ugly bash script by Josh G

c=0

for f in `find * -name '*.h' && find * -name '*.cpp' && find * -name '*.cc' && find * -name '*.hpp' && find * -name '*.c'`; do
  if [ -f "$f" ]; then
    x=`wc -l $f | tr " " "\n"`
    for w in $x; do
      if [ $w -eq $w 2> /dev/null ]; then
         c=$(($c+$w))
      fi
    done
  fi
done
echo "Total Line count is $c"

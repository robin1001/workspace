#!/bin/bash

begin=(176 481 569 641 671)
end=(200 500 600 670 692)

for((i=0; i<${#begin[*]}; i++)); do
{
    python rmrb.py ${begin[$i]} ${end[$i]} 
    #echo ${begin[$i]} ${end[$i]} 
}&
done

wait

echo "Done"

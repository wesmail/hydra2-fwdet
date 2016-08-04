
jobids=$(qstat -s r| cut -d " " -f 1)

declare -a dayarray


first=95
last=128

 
for (( i=$first;i<=$last;i++))
do
   day=$i
   dayarray[$day]=0
done

ct=0
for jobid in $jobids
do


     file=$(qstat -j $jobid 2>/dev/null | grep  job_args | sed 's/  */ /' | cut -d " " -f 2 | cut -d "," -f 3)
     
     
     if [ "$file" != "" ]
     then 
         name=$(basename $file)
         day=${name:4:3}
         
         day=$(echo $day | sed 's/^0//') 
         n=${dayarray[${day}]}
         ((n+=1))
         dayarray[$day]=$n
         ((ct+=1))
         printf "%5i : Job-Id: %i  runs  %s of %03i %i \n" $ct $jobid ${file} ${day} ${dayarray[${day}]}
     fi
done

echo "------------------------------------------------------------------"
echo " summary"

for (( i=$first;i<=$last;i++))
do
       if [ ${dayarray[${i}]} -gt 0 ]
       then
          printf "day : %03i => %i \n" ${i} ${dayarray[${i}]}
       fi
done


#/bin/bash

modF1=10000000
modF2=10000000
modF3=10000
modF4=10000000


nArg=$#





if [ $# -lt 2 ]
then
        echo "Usage : ../makeMultiList.sh  nevents file1stem [file12stem] [file3stem]"
else
  file2=""
  file3=""
  file4=""

  nfile=10
  file1=""

  case "$#" in

  2)  nfile=$1
      file1=$2
      ;;
  3)  nfile=$1
      file1=$2
      file2=$3
      ;;
  4)  nfile=$1
      file1=$2
      file2=$3
      file3=$4
      ;;
  5)  nfile=$1
      file1=$2
      file2=$3
      file3=$4
      file4=$5
      ;;
  *) echo "Unsupported number of arguments" 
     echo "Usage : makeMultiList.sh file1 [file2] [file3] [file4]"
     exit
     ;;
  esac

  f1=""
  f2=""
  f3=""
  f4=""
  countbad=0
  output=""
  
  
  for((ct=1;ct<=$nfile;ct++))
  do

  ct1=$ct
  ct2=$ct
  ct3=$ct
  ct4=$ct
  
  ((ct1=${ct}%${modF1}))
  if [ $ct1 -eq 0 ]
  then
    ct1=${modF1}
  fi 
  ((ct2=${ct}%${modF2}))
  if [ $ct2 -eq 0 ]
  then
    ct2=${modF2}
  fi 
  ((ct3=${ct}%${modF3}))
  if [ $ct3 -eq 0 ]
  then
    ct3=${modF3}
  fi 
  ((ct4=${ct}%${modF4}))
  if [ $ct4 -eq 0 ]
  then
    ct4=${modF4}
  fi 
  
  ok="yes"

  case "$nArg" in

  2)  f1=${file1}_${ct1}_1.root
      output="${f1}"
      if [ ! -f $f1 ]
      then
       ok="no"
      fi
      ;;
  3)  f1=${file1}_${ct1}_1.root;
      f2=${file2}_${ct2}_1.root;
      if [ ! -f $f1 ] || [ ! -f $f2 ]
      then
       ok="no"
      fi
      output="${f1}#${f2}";
      ;;
  4)  f1=${file1}_${ct1}_1.root
      f2=${file2}_${ct2}_1.root
      f3=${file3}_${ct3}_1.root
      if [ ! -f $f1 ] || [ ! -f $f2 ] || [ ! -f $f3 ]
      then
       ok="no"
      fi
      output="${f1}#${f2}#${f3}"
      ;;
  5)  f1=${file1}_${ct1}_1.root
      f2=${file2}_${ct2}_1.root
      f3=${file3}_${ct3}_1.root
      f4=${file4}_${ct4}_1.root
      if [ ! -f $f1 ] || [ ! -f $f2 ] || [ ! -f $f3 ] || [ ! -f $f4 ]
      then
       ok="no"
      fi
      output="${f1}#${f2}#${f3}#${f4}"
      ;;
  esac
 
 
      
      if [ $ok == 'yes' ]
      then
        echo  $output
      else
        ((countbad++))
        echo "${countbad} problem with $output" >&2
      fi
  done

fi















#/bin/bash


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
  ok="yes"

  case "$nArg" in

  2)  f1=${file1}_${ct}_1.root
      output="${f1}"
      if [ ! -f $f1 ]
      then
       ok="no"
      fi
      ;;
  3)  f1=${file1}_${ct}_1.root;
      f2=${file2}_${ct}_1.root;
      if [ ! -f $f1 ] || [ ! -f $f2 ]
      then
       ok="no"
      fi
      output="${f1}#${f2}";
      ;;
  4)  f1=${file1}_${ct}_1.root
      f2=${file2}_${ct}_1.root
      f3=${file3}_${ct}_1.root
      if [ ! -f $f1 ] || [ ! -f $f2 ] || [ ! -f $f3 ]
      then
       ok="no"
      fi
      output="${f1}#${f2}#${f3}"
      ;;
  5)  f1=${file1}_${ct}_1.root
      f2=${file2}_${ct}_1.root
      f3=${file3}_${ct}_1.root
      f4=${file4}_${ct}_1.root
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















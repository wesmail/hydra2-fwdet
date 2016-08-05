


dstdir=/hera/hades/dst/apr12/gen8
hlddir=/hera/hades/raw/apr12



if [ $# -ge 1 ]
then
  day=$1

   if [ $# -eq 2 ]
   then
      dstdir=$2
   fi

   if [ $# -eq 3 ]
   then
      hlddir=$3
   fi

   if [ -f lists/day_${day}.list ]
   then

      perl /misc/kempter/scripts/perl/matchFiles.pl -f lists/day_${day}.list -s "${dstdir}/${day}/filter/*.root" -m no -p 'be.*\.hld' -o first

   else
   
      perl /misc/kempter/scripts/perl/matchFiles.pl -f "${hlddir}/${day}/*.hld" -s "${dstdir}/${day}/filter/*.root" -m no -p 'be.*\.hld' -o first
   
   fi

else
  echo "usage: . ./missingfiles.sh day [dstdir] [hlddir]"
  echo "Default dstdir : /hera/hades/dst/apr12/gen6"
  echo "Default hlddir : /hera/hades/apr12"
  echo "The script assumes file lists in lists/day_${day}.list"
  echo "If the the file lists are not existing all input hld"
  echo "files form /hera/hades/apr12/$day will be taken as reference"
fi


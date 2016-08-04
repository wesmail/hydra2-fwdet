


dstdir=/hera/hades/dstsim/apr12/dst
mask="Au_Au_1230MeV_omega_lambda_1000evts_[0-9]*_"
if [ $# -ge 1 ]
then
  list=$1

   if [ $# -eq 2 ]
   then
      dstdir=$2
   fi

   if [ $# -eq 3 ]
   then
      mask=$2
   fi


   perl /misc/kempter/scripts/perl/matchFiles.pl -f $list -s "${dstdir}/*.root" -m no -p "${mask}" -o first

else
  echo "usage: . ./missingfiles.sh list [mask] [dstdir]"
  echo "Default dstdir : $dstdir"
  echo "Default mask   : \"$mask\""
fi


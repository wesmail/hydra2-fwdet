#set -xv

stampDate=$(date +%Y-%m-%d)
stampTime=$(date +%H:%M:%S)
range2=$(date +%s)
range1=$range2
((range1-=3600*24))


logusername=logUser
loggroupname=logGroup

outdir=/misc/kempter/GE_mon_log
picdir=$outdir/archive/${stampDate}
userlog=$outdir/archive/$logusername
grouplog=$outdir/archive/$loggroupname

webdir=/misc/kempter/web-docs/GEmon/


userlog=${userlog}_${stampDate}
grouplog=${grouplog}_${stampDate}

if [ ! -d $outdir ]
then
  mkdir -p $outdir
fi

if [ ! -d $outdir/archive/${stampDate} ]
then
  mkdir -p $outdir/archive/${stampDate} 
fi

if [ ! -d $webdir ]
then
  mkdir -p $webdir
fi

if [ ! -d $webdir/archive/${stampDate} ]
then
  mkdir -p $webdir/archive/${stampDate}
fi

olddir=$(pwd)

cd $outdir

./GE_mon.pl -u ${userlog}.dat -g ${grouplog}.dat  > GE_mon_out.txt
. /cvmfs/hades.gsi.de/install/5.34.01/hydra2-3.9/defall.sh

export ROOT_HIST=0
root -l -b<< EOF
.L plotHist.C++
plotHist("${userlog}.dat","${grouplog}.dat","${outdir}/archive/${stampDate}","${stampDate}","${stampTime}",${range1},${range2});
.q

EOF




if [ -f GE_mon_out.txt ]
then
   cp  GE_mon_out.txt $webdir
fi


if [ -f ${picdir}/${logusername}_${stampDate}_run.png ]
then
   cp  ${picdir}/${logusername}*_run.png $webdir/archive/${stampDate}
   cp  ${picdir}/${logusername}*_pend.png $webdir/archive/${stampDate}
   cp  ${picdir}/${logusername}*.png $outdir/archive/${stampDate}
   for file in $(ls -1 ${picdir}/${logusername}*.png)
   do
   	 filenew=$(basename $file | sed -e "s/_$stampDate//")
         cp  $file $webdir/$filenew 
   done
fi

if [ -f ${picdir}/${loggroupname}_${stampDate}_run.png ]
then
   cp  ${picdir}/${loggroupname}*_run.png $webdir/archive/${stampDate}
   cp  ${picdir}/${loggroupname}*_pend.png $webdir/archive/${stampDate}
   cp  ${picdir}/${loggroupname}*.png $outdir/archive/${stampDate}
   for file in $(ls -1 ${picdir}/${loggroupname}*.png)
   do
   	 filenew=$(basename $file | sed -e "s/_$stampDate//")
         cp  $file $webdir/$filenew 
   done
fi

cp index.html   $webdir

cat index_STAMP.html | sed "s/STAMP/$stampDate/g" >  $webdir/archive/index_$stampDate.html

cd $olddir
#set +xv

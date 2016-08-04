#!/bin/bash

#debug on
#set -xv

#-----------------------------------------------------------------
# USAGE :
# ./runStat.sh directory levelofprint
#
# This scripts perform a rekursive scan of a disk directory using
# the posix interface of the filesystem. All directories are stored
# inside a root file using HDiskCatalog. A rescan of the directory
# will perform check if the directory has changed since last scan
# and resacn only if needed. Otherwise the dir info is copied from
# the previous scan. This results in an  noticalbe performance gain.
#-----------------------------------------------------------------


#-----------------------------------------------------------------
#  TO ADD IN crontab -e
#SHELL=/bin/bash
#PATH=/usr/bin:/bin:/usr/bin/X11
#
#  run at 1 in the night each night
#0 1 * * *	./workingdir/runStat.sh /mydirectory leveltoprint > /workingdir/mydirectory/exec.log 2>&1
#-----------------------------------------------------------------

#sourcedir=$(pwd)           # location of diskspace.C (default is to run in checkout dir)
sourcedir=/misc/kempter/Diskspace_mon

#-----------------------------------------------------------------
# composite dir    :  select subdirs inside mother dir by
# include,exclude (commasepared list of TRegexp, exclude evaluated first) :
#            default include all ".*" , exact matching "^dir1$,^dir2$,^dir#$"
#            default exclude none ""
# for each subdir a discatalog and diskstat object will be created in an own file.
# A master catalog is build by number of subleveldirs copied from subdir catalogs

groupID=hades       # for quota
isCompositeDir=yes  # yes for composite dir
includedirs='.*'    # selection of subdirs    default ".*"
excludedirs=''      # exclude subdirs         default ""
#-----------------------------------------------------------------

#-----------------------------------------------------------------
# custom setup for special dirs
# all others will get level
# keys  like path with '/' replace by '_' (example : _hera_hades_user)
# log level 1,2,3 are produced for each subdir
unset map
unset scanned
declare -A scanned
declare -A map
# add dirs
map[_hera_hades]=1;
map[_hera_hades_user]=1;
map[_hera_hades_dst]=3;
map[_hera_hades_dstsim]=3;
map[_hera_hades_raw]=2;

#-----------------------------------------------------------------



dir=""
level=1

if [ $# -eq 2 ]
then
  dir=$1
  level=$2

  # setting environment vars
  # ROOTSYS + library dir
  . ${sourcedir}/defall.sh

  #-----------------------------------------------------------------
  # NAMES AND DIRECTORIES

  # for naming we replace all / by _
  dir=$(echo $dir | sed 's/\/$//')  
  dirrep=$(echo $dir | sed 's/\//_/g')

  baseoutdir=./out           # out dir for root object. subdirs for monitored dirs will be created inside
  basewebdir=/misc/kempter/web-docs/DiskSpace_mon  # out dir for web  object. subdirs for monitored dirs will be created inside

  baseoutdir=$(echo $baseoutdir | sed 's/\/$//')
  basewebdir=$(echo $basewebdir | sed 's/\/$//')

  outdir=$baseoutdir/$dir
  webdir=$basewebdir/$dir

  webdir=$(echo $webdir | sed 's/\/$//')
  outdir=$(echo $outdir | sed 's/\/$//') 
  #-----------------------------------------------------------------


  #-----------------------------------------------------------------
  # CREATE OUTPUT DIRS 
  if [ ! -d $outdir ]
  then
    mkdir -p $outdir
  fi

  if [ ! -d $webdir ]
  then
    mkdir -p $webdir
  fi
  #-----------------------------------------------------------------



  #-----------------------------------------------------------------
  # RUN DISKSCAN, disable root hist for this run
  # to not mock up the user history
  cd ${sourcedir}
  
  #-----------------------------------------------------------------
  # check quota for group
  if [ -f add_quota.txt ]
  then
     rm -f add_quota.txt
  fi
 
  res=$(lfs quota -g $groupID $dir 2>/dev/null)

  if [ $? -eq 0 ]
  then
     echo "<div align=\"center\">"   >  add_quota.txt
     echo "<h2 style=\"color:red\">" >> add_quota.txt
     echo "<pre>"                    >> add_quota.txt
     
     res=$(lfs quota -g $groupID $dir | grep -v Disk | grep -v Filesystem | sed 's/  +/ /g')

     filesys=$(echo $res | cut -d " " -f 1 )
        size=$(echo $res | cut -d " " -f 2 | sed 's/\*//g')
       quota=$(echo $res | cut -d " " -f 3 | sed 's/\*//g')
       limit=$(echo $res | cut -d " " -f 4 | sed 's/\*//g')
       grace=$(echo $res | cut -d " " -f 5 | sed 's/\*//g')

       ((toTera=1024*1024*1024))

       ((size=$size/$toTera))
       ((quota=$quota/$toTera))
       ((limit=$limit/$toTera))


     printf "filesys                             group ondisk  quota  limit [TB]      grace\n" >> add_quota.txt
     printf "%-30s %10s %6s %6s %6s     %11s\n" $filesys $groupID $size $quota $limit $grace   >> add_quota.txt
     echo "</pre>"                   >> add_quota.txt
     echo "</h2>"                    >> add_quota.txt
     echo "</div>"                   >> add_quota.txt
  else
     echo "<div align=\"center\">"   >  add_quota.txt
     echo "<h2 style=\"color:red\">" >> add_quota.txt
     echo "<pre>"                    >> add_quota.txt
     echo "no quotas for group $groupID on $dir"  >> add_quota.txt
     echo "</pre>"                   >> add_quota.txt
     echo "</h2>"                    >> add_quota.txt
     echo "</div>"                   >> add_quota.txt
  fi
  #-----------------------------------------------------------------
  
  export ROOT_HIST=0
  
if [ "${isCompositeDir}" == "yes" ]
then
  echo "exe root"
  root -l -b<< EOF
  .L diskspaceComposite.C+
  diskspaceComposite("${dir}","${outdir}",${level},"${excludedirs}","${includedirs}");
  .q

EOF
else
  echo "exe root"
  root -l -b<< EOF
  .L diskspace.C+
  diskspace("${dir}","${outdir}",${level});
  .q

EOF
fi
  #-----------------------------------------------------------------


  #-----------------------------------------------------------------
  # list of scanned dirs
  # all others dirs will be removed (clean up of old dirs)!
  # old webdir will be removed too!
   

   if [ "${isCompositeDir}" == "yes" ]
   then

      if [ -f $outdir/scanned_dirs.log ]
      then
          
          scanned[${dirrep}]=1;
          for item in  $(cat $outdir/scanned_dirs.log)
          do
              scanned[${dirrep}_${item}]=1;
          done
      
          for found in $(find $outdir -maxdepth 1 -name "dirdata_*.root" | xargs -i basename {} | grep -v old.root | sed 's/dirdata//' | sed 's/.root$//' | sed 's/\//_/' )
          do
              if [ "${scanned[${found}]}" == "" ]
              then
                 echo "will remove : $found!"
                   rm  $outdir/dirdata${found}.root
                   rm  $outdir/dirdata${found}_dir_summary*.log
                   rm  $outdir/dirdata${found}_size.png
                   rm  $outdir/dirdata${found}_nfiles.png
                   rm  $outdir/dirdata${found}_nsmallfiles.png
                   rm  $outdir/dirdata${found}_ratio.png

                   rm  $outdir/history_dirdata${found}_size.png
                   rm  $outdir/history_dirdata${found}_nfiles.png
                   rm  $outdir/history_dirdata${found}_nsmallfiles.png

                   rm  $outdir/history_dirdata${found}_mother_size.png
                   rm  $outdir/history_dirdata${found}_mother_nfiles.png
                   rm  $outdir/history_dirdata${found}_mother_nsmallfiles.png

                  olddir=$(echo ${found} | sed 's/_/\//g')
                  olddir=$(basename ${olddir})
                  if [ -d ${webdir}/${olddir} ]
                  then
                    echo "remove old dir ${webdir}/${olddir}"
                     rm -rf ${webdir}/${olddir}
                  fi
              
              else 
                  echo "keep on disk : $found!"
              fi
          done
      
      fi


   fi
  
  #-----------------------------------------------------------------

  #-----------------------------------------------------------------
  # COPY FIGURES AND LOGFILES TO WEBDIR
  
  cp  $outdir/dirdata${dirrep}_dir_summary.log                   $webdir/dirdata_summary.txt
  cp  $outdir/dirdata${dirrep}_size.png                          $webdir/dirdata_size.png
  cp  $outdir/dirdata${dirrep}_nfiles.png                        $webdir/dirdata_nfiles.png
  cp  $outdir/dirdata${dirrep}_nsmallfiles.png                   $webdir/dirdata_nsmallfiles.png
  cp  $outdir/dirdata${dirrep}_ratio.png                         $webdir/dirdata_ratio.png

  cp  $outdir/history_dirdata${dirrep}_size.png                  $webdir/history_size.png
  cp  $outdir/history_dirdata${dirrep}_nfiles.png                $webdir/history_nfiles.png
  cp  $outdir/history_dirdata${dirrep}_nsmallfiles.png           $webdir/history_nsmallfiles.png

  cp  $outdir/history_dirdata${dirrep}_mother_size.png           $webdir/history_mother_size.png
  cp  $outdir/history_dirdata${dirrep}_mother_nfiles.png         $webdir/history_mother_nfiles.png
  cp  $outdir/history_dirdata${dirrep}_mother_nsmallfiles.png    $webdir/history_mother_nsmallfiles.png


  cp index.html           $webdir/index.html
  cp index_daughters.html $webdir

  if [ -f add_links.txt ]
  then
     rm -f add_links.txt
  fi

  if [ "${isCompositeDir}" == "yes" ]
  then
     # produce add_links.txt to insert it into html later
     echo "<p>&nbsp;</p><p>&nbsp;</p>"      >> add_links.txt
     echo "<div align=\"center\">"          >> add_links.txt
     echo " <h2>Links to other pages </h2>" >> add_links.txt
    #-----------------------------------------------------------------
    # if run as diskspaceComposite.C each 1st level subdir will have
    # own outputs which should be copied to webout/subdir
    dirs=$(find $outdir -maxdepth 1 -name dirdata*.root | grep -v dirdata${dirrep}.root | grep -v _old.root)
    mylinks=" "
    for subdir in $dirs
    do
        subdir=$(basename $subdir | sed 's/dirdata//' | sed 's/.root$//' | sed "s/${dirrep}_//")
      
        if [ ! -d $webdir/$subdir ]
        then	
           mkdir -p $webdir/$subdir
        fi
        
        if [ "${map[${dirrep}_${subdir}]}" == "" ]
        then
            map[${dirrep}_${subdir}]=${level} 
            echo "using standard loglevel for ${dirrep}_${subdir} = ${map[${dirrep}_${subdir}]}"
        else 
            echo "using custom   loglevel for ${dirrep}_${subdir} = ${map[${dirrep}_${subdir}]}"
        fi
      
        loglevel=${map[${dirrep}_${subdir}]}
      

        cp  $outdir/dirdata${dirrep}_${subdir}_dir_summary_${loglevel}.log       $webdir/${subdir}/dirdata_summary.txt
      
        cp  $outdir/dirdata${dirrep}_${subdir}_size.png                          $webdir/${subdir}/dirdata_size.png
        cp  $outdir/dirdata${dirrep}_${subdir}_nfiles.png                        $webdir/${subdir}/dirdata_nfiles.png
        cp  $outdir/dirdata${dirrep}_${subdir}_nsmallfiles.png                   $webdir/${subdir}/dirdata_nsmallfiles.png
        cp  $outdir/dirdata${dirrep}_${subdir}_ratio.png                         $webdir/${subdir}/dirdata_ratio.png
       
        cp  $outdir/history_dirdata${dirrep}_${subdir}_size.png                  $webdir/${subdir}/history_size.png
        cp  $outdir/history_dirdata${dirrep}_${subdir}_nfiles.png                $webdir/${subdir}/history_nfiles.png
        cp  $outdir/history_dirdata${dirrep}_${subdir}_nsmallfiles.png           $webdir/${subdir}/history_nsmallfiles.png

        cp  $outdir/history_dirdata${dirrep}_${subdir}_mother_size.png           $webdir/${subdir}/history_mother_size.png
        cp  $outdir/history_dirdata${dirrep}_${subdir}_mother_nfiles.png         $webdir/${subdir}/history_mother_nfiles.png
        cp  $outdir/history_dirdata${dirrep}_${subdir}_mother_nsmallfiles.png    $webdir/${subdir}/history_mother_nsmallfiles.png


        cp index.html           $webdir/${subdir}
        cp index_daughters.html $webdir/${subdir}
      
        mylinks="${mylinks} <a href=\"${subdir}/index.html\">${subdir}</a>"
    done
    #-----------------------------------------------------------------
    echo $mylinks                           >> add_links.txt
    echo "</div>"                           >> add_links.txt
    sed '/<!-- ADD_LINKS -->/ r add_links.txt' index.html     >  index_tmp.html
    sed '/<!-- ADD_QUOTA -->/ r add_quota.txt' index_tmp.html >  index_tmp2.html
    # overide default index 
    cp index_tmp2.html           $webdir/index.html
  else
    sed '/<!-- ADD_QUOTA -->/ r add_quota.txt' index.html >  index_tmp.html
    cp index_tmp.html           $webdir/index.html
  
  fi

  if [ -f $outdir/exec.log ]
  then
     cp  $outdir/exec.log  $webdir/exe.txt
  fi
  
  #-----------------------------------------------------------------

else
   echo  "ERROR: wrong number of arguments!"
   echo  "USAGE: ./runStat.sh directory levelofprint"

fi
# debug off
#set +xv

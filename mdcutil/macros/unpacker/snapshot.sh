#! /usr/local/bin/bash
########## Function declaration ########

createDir ()
{
dir=$1
if [ ! -d $dir ] 
then
    mkdir -p $dir
fi
}

getLibName ()
{
libName=""
path=$1
makefilePath=$path/Makefile

if [ -f $makefile$Path ]
then
libName=lib`cat $makefilePath |\
        grep LIBNAME | \
	sed -e 's/LIBNAME./LIBNAME/g' | \
	sed -e 's/ //g' | \
 	grep 'LIBNAME=' | \
	cut -d "=" -f 2`.so  
fi
exit $libName
}

usage ()
{
exe=`basename $1`
}
########################################
##             Main Code             ###
########################################

    ## --------------------------------------------------
    ##  checking for right method to call the script
    ## --------------------------------------------------
   
    if [ `basename $0` = snapshot.sh ]
    then 
	echo
	echo use a leading " . " to call the `basename $0` script!
	echo -e "exiting ...\n"
	exit
    fi

    ## --------------------------------------------------
    ##  
    ## --------------------------------------------------

snapshotPath=$MYDEVHADDIR/snapshot/lib

createDir $snapshotPath
echo "snapshot path >>> $snapshotPath"

LIB_LIST=`set | grep HADDIR | grep LIB | grep lib | cut -d "="  -f 1`

for item in $LIB_LIST
do 
    if [ $item = HADDIR_BASEDIR ] 
    then 
	continue
    fi
    echo -n "$item ."

    srcPath=`set | grep HADDIR | grep ${item%%_LIB}= | cut -d "="  -f 2`
    libPath=`set | grep HADDIR | grep ${item}= | cut -d "="  -f 2`

    makefilePath=$srcPath/Makefile
    libName=lib`cat $makefilePath |\
	    grep LIBNAME | \
	    sed -e 's/LIBNAME./LIBNAME/g' | \
	    sed -e 's/ //g' | \
	    grep 'LIBNAME=' | \
	    cut -d "=" -f 2`.so  
    echo -n "."

    if [ -f $libPath/$libName ]
    then
	if [ -d $snapshotPath ]
	then
	echo -n "."
	    cp $libPath/$libName $snapshotPath/$libName
	fi
    fi
    echo " done"
done   

for item in $LIB_LIST
do 
    export $item=`eval echo $snapshotPath`
done

unset LIB_LIST
			    

		    
    
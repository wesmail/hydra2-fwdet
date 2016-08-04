# usage . ./makeDependList.sh  pathtolibs


ROOTLIB=$1

libs=$(ls -1 ${ROOTLIB}/*.so)

ct=0
alllibs=""

out=alllibs.txt
if [ -e ${out} ]
then
  rm $out
fi

for lib in ${libs}
do
    ((ct+=1))
    echo "#${ct} $lib"

    newlibs=$(ldd ${lib} | sed 's/ *//g' | sed 's/=>.*//' | sed 's/\t//g' | sed 's/(.*)//g' >> ${out} )
    alllibs="${alllibs} ${newlibs}"
    
done

alllibs=$(cat $out | sort -u)

echo "#------------------"
echo "# The above libs link the following libs:"
ct=0

for lib in ${alllibs}  
do
   ((ct+=1))
    echo "# ${ct} ${lib}"
    
done


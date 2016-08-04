
if [ -e cernlib_tarball ]
then
   if [ -e cernlib.tar.gz ]
   then 
      rm -f cernlib.tar.gz
   fi 
   
   find  cernlib_tarball -name "*~" | xargs rm
   tar -cvvzf  cernlib.tar.gz   cernlib_tarball
   chmod 755 cernlib.tar.gz
else
   echo "ERROR: cernlib_tarball dir does not exist! Cannot tar...."
fi 

if [ -e garfield_tarball ]
then 
   if [ -e garfield.tar.gz ]
   then 
      rm -f garfield.tar.gz
   fi
   
   find  garfield_tarball -name "*~" | xargs rm 
   tar -cvvzf  garfield.tar.gz  garfield_tarball
   chmod 755 garfield.tar.gz
else
   echo "ERROR: garfield_tarball dir does not exist! Cannot tar...."

fi


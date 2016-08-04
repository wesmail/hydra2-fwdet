
echo Checking for CERNLIB requirements

echo "------------------------------"
  echo "check imake"
result=$(which imake)

if [ "$result" != "" ]
then
   echo "imake  : ok"
else
   echo "imake  : not found"
   echo "install xutils-dev"
fi


echo "------------------------------"
  echo "check gmake"
result=$(which gmake)

if [ "$result" != "" ]
then
   echo "gmake  : ok"
else
   echo "gmake  : not found"
   echo "         add a softlink  ln -s /bin/make /bin/gmake (or somewhere in PATH)"
fi

echo "------------------------------"
  echo "check ed"
result=$(which imake)

if [ "$result" != "" ]
then
   echo "ed     : ok"
else
   echo "ed     : not found"
fi


echo "------------------------------"
  echo "check openmotiv-dev"

if [ -e /usr/include/Xm/Xm.h ]
then
   echo "opemotif-dev  : ok"
else
   echo "openmotif-dev : not found"
   echo "install libmotif-dev"
fi


echo "------------------------------"
  echo "check libxt-dev-dev"

if [ -e /usr/include/X11/StringDefs.h ]
then
   echo "libxt-dev  : ok"
else
   echo "libxt-dev : not found"
   echo "install libxt-dev"
fi

echo "------------------------------"
  echo "check libfreetype"

if [ -e /usr/include/freetype2 ]
then
   echo "libfreetype  : ok"
else
   echo "libfreetype : not found"
fi


echo "------------------------------"
  echo "check tcsh"

if [ -e /bin/tcsh ]
then
   echo "tcsh  : ok"
else
   echo "tcsh : not found"
   echo "inctall tcsh" 
   echo "because of problems with csh and cernlib"
   echo "make csh a softlink to the compatible tcsh"
   
   echo "add a softlink : ln -s /usr/bin/tcsh /usr/bin/csh"
   echo "remove any other csh link in /bin/csh"
fi




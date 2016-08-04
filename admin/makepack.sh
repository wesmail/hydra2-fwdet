#! /bin/ksh

#
# make a tar file from analysis directory $1
#
#
case $# in
0)  echo 'needs an argument' 1>&2; exit 1
esac

cp -R $1 tempdir
cd tempdir

# clean up first
rm */*.o
rm */*.d
rm */so_location
rm lib/*.so
cd docs/www
rm -r *
cd ../..

# make a tar file of source files 
tar rvf $1.tar .
mv $1.tar ../.
cd ..
tar tf $1.tar > $1.lst
gzip $1.tar
rm -r tempdir

# that's it

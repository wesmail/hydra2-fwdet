
part=p
nfiles=100


# standard
f1=/lustre/nyx/hades/dstsim/apr12/hgeant/bmax10/new/${part}/Au_Au_1230MeV_1000evts
f2=/lustre/nyx/hades/dstsim/apr12/hgeant/bmax10/new/deltaelectron/Au_Au_1230MeV_1000evts


. ./makeMultList.sh $nfiles $f1 $f2 




# urqmd+part+deltaelectron


#f1=/lustre/nyx/hades/dstsim/apr12/hgeant/bmax10/${part}/Au_Au_1230MeV_1000evts
#f2=/lustre/nyx/hades/dstsim/apr12/hgeant/bmax10/no_enhancement_gcalor/Au_Au_1230MeV_1000evts
#f3=/lustre/nyx/hades/dstsim/apr12/hgeant/bmax10/new/deltaelectron/Au_Au_1230MeV_1000evts


#. ./makeMultList.sh $nfiles $f1 $f2 $f3







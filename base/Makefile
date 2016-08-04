##############################################################################
#
#  Makefile for Hydra library libHydra.so and libHydra.a
#
#  This makefile contains all definitions local to this module. All
#  general definitions are included from makefiles named "hades.*.mk".
#
##############################################################################

LIB_NAME := Hydra

USES_RFIO ?= no
USES_XROOTD ?= yes

SOURCE_FILES :=	datasource/hdatasource.cc		\
		datasource/hgeantreader.cc		\
		datasource/hgeantsource.cc		\
		datasource/hgeantmergesource.cc		\
		datasource/hldbase.cc			\
		datasource/hldevt.cc			\
		datasource/hldfilevt.cc			\
		datasource/hldsource.cc			\
		datasource/hldfilesource.cc		\
		datasource/hldsubevt.cc			\
		datasource/hldfileoutput.cc		\
		datasource/hldunpack.cc			\
		datasource/hrootsource.cc		\
		datasource/hkinegeantreader.cc		\
		datasource/hsimulgeantreader.cc		\
		datasource/hdirectsource.cc		\
		datasource/hldfilesourcebase.cc		\
		datasource/hldremotesource.cc		\
		datasource/hldremotevt.cc		\
		datasource/hldgrepfilesource.cc		\
		datasource/htrbbaseunpacker.cc		\
		datasource/htrb2unpacker.cc		\
		datasource/htrb3unpacker.cc		\
		datasource/htrb3tdcmessage.cc		\
		datasource/htrb3tdciterator.cc		\
		datasource/htrb3tdcunpacker.cc		\
		datasource/htrblookup.cc		\
		datasource/htrbnetaddressmapping.cc	\
		datasource/htrb2correction.cc		\
		datasource/htrb3calpar.cc		\
		datasource/htrbnetunpacker.cc		\
		datasource/htrbnetdebuginfo.cc

SOURCE_FILES +=	datastruct/hcategory.cc			\
		datastruct/hfilter.cc			\
		datastruct/hindextable.cc		\
		datastruct/hiterator.cc			\
		datastruct/hlinearcategory.cc		\
		datastruct/hlinearcatiter.cc		\
		datastruct/hlocateddataobject.cc	\
		datastruct/hlocation.cc			\
		datastruct/hmatrixcategory.cc		\
		datastruct/hmatrixcatiter.cc		\
		datastruct/hrecobject.cc		\
		datastruct/htrack.cc			\
		datastruct/hlinkeddataobject.cc		\
		datastruct/hratree.cc			\
		datastruct/hratreeext.cc		

SOURCE_FILES +=	eventhandling/hades.cc			\
		eventhandling/hdetector.cc		\
		eventhandling/hevent.cc			\
		eventhandling/heventheader.cc		\
		eventhandling/hpartialevent.cc		\
		eventhandling/hrecevent.cc		\
		eventhandling/hparallelevent.cc		\
		eventhandling/hreconstructor.cc		\
		eventhandling/hspectrometer.cc		\
		eventhandling/htask.cc			\
		eventhandling/htaskset.cc		\
		eventhandling/htree.cc

SOURCE_FILES +=	geantutil/hgeantheader.cc		\
		geantutil/hgeantkine.cc			\
		geantutil/hgeantmdc.cc			\
		geantutil/hgeantrich.cc			\
		geantutil/hgeanttof.cc			\
		geantutil/hgeantshower.cc		\
		geantutil/hgeantwall.cc			\
		geantutil/hkinesim.cc			\
		geantutil/hgeantrpc.cc			\
		geantutil/hgeantemc.cc			\
		geantutil/hgeantstart.cc		\
		geantutil/hgeantfwdet.cc		\
		geantutil/hgeantmedia.cc

SOURCE_FILES +=	geometry/hgeomtransform.cc		\
		geometry/hgeomvector.cc			\
		geometry/hgeomrotation.cc		\
		geometry/hgeomvolume.cc			\
		geometry/hgeomcompositevolume.cc	\
		geometry/hspecgeompar.cc		\
		geometry/hdetgeompar.cc			\
		geometry/hgeomshapes.cc			\
		geometry/hgeombasicshape.cc		\
		geometry/hgeombrik.cc			\
		geometry/hgeomtrap.cc			\
		geometry/hgeomtrd1.cc			\
		geometry/hgeompgon.cc			\
		geometry/hgeompcon.cc			\
		geometry/hgeomtube.cc			\
		geometry/hgeomtubs.cc			\
		geometry/hgeomcone.cc			\
		geometry/hgeomcons.cc			\
		geometry/hgeomsphe.cc			\
		geometry/hgeomeltu.cc			\
		geometry/hgeomvertexfit.cc		\
		geometry/hgeommatrix.cc			\
		geometry/hgeomvertexfit2.cc		\
		geometry/hgeomvertexfit3.cc

SOURCE_FILES +=	runtimedb/hcontfact.cc			\
		runtimedb/hbasecontfact.cc		\
		runtimedb/hruntimedb.cc			\
		runtimedb/hparset.cc			\
		runtimedb/hparcond.cc			\
		runtimedb/hparamlist.cc			\
		runtimedb/hrun.cc			\
		runtimedb/hpario.cc			\
		runtimedb/hdetpario.cc			\
		runtimedb/hparrootfileio.cc		\
		runtimedb/hdetparrootfileio.cc		\
		runtimedb/hcondparrootfileio.cc		\
		runtimedb/hspecparrootfileio.cc		\
		runtimedb/hparasciifileio.cc		\
		runtimedb/hdetparasciifileio.cc		\
		runtimedb/hcondparasciifileio.cc	\
		runtimedb/hspecparasciifileio.cc	\
		runtimedb/hparamfilegenerator.cc	\
		runtimedb/hruninfo.cc			\
		runtimedb/hfileinfo.cc			\
		runtimedb/hmagnetpar.cc                 \
		runtimedb/hslowpar.cc			\
		runtimedb/hemcsimulpar.cc

SOURCE_FILES +=	util/hdebug.cc				\
		util/hobjtable.cc			\
		util/hrunanalizer.cc			\
		util/hsymmat.cc				\
		util/hmessagemgr.cc			\
		util/hntuple.cc	   		        \
		util/hntuplemap.cc    		        \
		util/harray.cc    		        \
		util/hhistmap.cc    		        \
		util/hflex.cc    		        \
		util/hflexfiller.cc    		        \
		util/hphysicsconstants.cc		\
		util/htool.cc				\
		util/hhistconverter.cc			\
		util/hbitman.cc				\
		util/hstltool.cc			\
		util/hcategorymanager.cc       		\
		util/hloop.cc		       		\
	        util/hzip.cc		       		\
	        util/hdiskspace.cc     	       		\
	        util/htime.cc		       		\
	        util/hsrckeeper.cc     	       		\
	        util/hseed.cc     	       		\
	        util/hmultipagepdf.cc 	       		\
	        util/hsuexemacro.cc    			\
		util/hsuprogress.cc			\
		util/hsudummyrec.cc    			\
		util/hgeantfilter.cc

# addtional source files related to RFIO
ifeq ($(strip $(USES_RFIO)),yes)
	SOURCE_FILES +=	datasource/hrfiorootsource.cc	\
			datasource/hldrfiofilesource.cc	\
			datasource/hldrfiofilevt.cc	\
			datasource/hrfiofile.cc
endif

# additional source files needed on a Linux system, only
SOURCE_FILES +=	datasource/linux_server_clnt.cc	\
		datasource/linux_server_xdr.cc

# additional source files needed on non Linux systems, only
#SOURCE_FILES += datasource/osf_server_clnt.c	\
#		datasource/osf_server_xdr.c

# directories removed from Makefile, but not from CVS
#		correlation
#		evtserver

include $(HADDIR)/hades.def.mk
include $(HADDIR)/hades.module.mk

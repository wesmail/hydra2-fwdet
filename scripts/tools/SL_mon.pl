#!/usr/bin/perl
use File::Find;
use warnings;
use strict;
use Data::Dumper;
use IO::Handle;
#use IO::POSIX;
use Getopt::Std;
use FileHandle;


my $ct = 0;

my $project;
my $dummy;

my $jobid;
my $user;
my $state;
my $partition;
my $isArray;
my $nCPU;



my $name;
my $pass;
my $uid;
my $gid;
my $quota;
my $comment;
my $gcos;
my $dir;
my $shell;
my $expire;

my $help = "usage : GE_mon.pl [ -u userlogfile ] [ -g grouplogfile ]
           \t  -u userlogfile  : number of jobs per user will be logged
           \t  -g grouplogfile : number of jobs per group will be logged
    \n";

our %userjobs;      # hash  user => jobstates  (key for each statetype -> counter for that type)
our %groups;        # hash  groupname ==> number of jobs
our %ugroup;        # hash  user ==> group
our %userpart;      # hash  user  => partition jobs)
our %grouppart;     # hash  group => partition jobs)

our $logfileUser;   # logfile for user history
our $logfileGroup;  # logfile for group history

our $wLogUser  = FileHandle->new;
our $wLogGroup = FileHandle->new;

my $NARGS = $#ARGV ; #  $#ARGV will be rest by getopt()!!!

my %args;
getopt('ug', \%args);


$logfileUser  = $args{"u"} ? $args{"u"} : "" ;
$logfileGroup = $args{"g"} ? $args{"g"} : "" ;

my %parameters = ("logfileUser"  => $logfileUser,
                  "logfileGroup" => $logfileGroup
                 );

if($NARGS > 3){
    print $help;
    print $NARGS ,"\n";
    print STDERR Dumper \%parameters;
    exit(1);
}


my $partlist = `sinfo -h --format="%P"`; 
$partlist=~s/\*//;
my @ar_part = split(/\n/,$partlist);


my $timestamp = time();
my $stat=`squeue -h -r --format="%.18i %.9P %.20u %.2t %C"`;  # jobid partition user state
my @array = split(/\n/,$stat);


if($logfileUser ne ""){
    $wLogUser->open(">>${logfileUser}");
    if ( ! defined $wLogUser){
        print "Cannot open output file: ${logfileUser}!";
        undef $wLogUser;
        exit(1);
    }


} else { undef $wLogUser; }

if($logfileGroup ne ""){
    $wLogGroup->open(">>${logfileGroup}");
    if ( ! defined $wLogGroup){
        print "Cannot open output file: ${logfileGroup}!";
        undef $wLogGroup;
        exit(1);
    }
} else { undef $wLogGroup; }


foreach my $line (@array)
{
    $ct++;

    $partition  = "no_partition";

    #format default "%.18i %.9P %.8j %.8u %.2t %.10M %.6D %R"
    #JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
    #   8553984      main Au10AuUr    dblau  R    3:59:52      1 lxbk0195
    #   8553968      main Au10AuUr    dblau  R    4:00:04      1 lxbk0182
    #   8553958      main Au10AuUr    dblau  R    4:00:10      1 lxbk0135
    #   8553936      main Au10AuUr    dblau  R    4:00:27      1 lxbk0126
    #   8554097      main Au10AuUr    dblau  R    3:59:07      1 lxbk0156
    #   8554094      main Au10AuUr    dblau  R    3:59:09      1 lxbk0186
    #   8554091      main Au10AuUr    dblau  R    3:59:10      1 lxbk0141
    #   8554070      main Au10AuUr    dblau  R    3:59:17      1 lxbk0091
    #   8554041      main Au10AuUr    dblau  R    3:59:20      1 lxbk0159
    #   8554037      main Au10AuUr    dblau  R    3:59:23      1 lxbk0052


    #squeue --format="%.18i %.9P %.20u %.2t"`;  # jobid partition user state

    $line=~s/ +/ /g;
    ($jobid,$partition,$user,$state,$nCPU) = split(" ",$line) ;

    $jobid=~s/ //g;
    $user=~s/ //g;
    $state=~s/ //g;
    $partition=~s/ //g;

    if($jobid=~m/_/){   # format : jobid_taskid
        $isArray = 1;
    } else {
        $isArray = 0;
    }


    #--------------------------------------------------------------------------------
    if(not exists $userjobs{$user}){         # first time the user appears
        $userjobs{$user}{"array"} =0; 
        foreach my $p (@ar_part)
        {
            $userpart{$user}{$p}=0;
        }

    }
    #--------------------------------------------------------------------------------


    #--------------------------------------------------------------------------------
    # user accounting

    #  Job state, compact form: PD (pending),
    #                           R  (running),
    #                           CA (cancelled),
    #                           CF(configuring),
    #                           CG (completing),
    #                           CD (completed),
    #                           F (failed),
    #                           TO (timeout),
    #                           NF (node failure)
    #                       and SE (special exit state).


    $userjobs{$user}{$state}  += $nCPU;
    $userjobs{$user}{"total"} += $nCPU;

    if($isArray eq 1){
        $userjobs{$user}{"array"} += $nCPU;
    }
    if($state eq "R"){
        $userpart{$user}{$partition}  += $nCPU;
    }
    #--------------------------------------------------------------------------------

    #--------------------------------------------------------------------------------
    # get groupname
    if(not exists $ugroup{$user}){
        ($name, $pass, $uid, $gid, $quota, $comment, $gcos, $dir, $shell, $expire) = getpwnam($user);

        my $groupname =  getgrgid($gid);
        $ugroup{$user}=$groupname;
        }        #--------------------------------------------------------------------------------

    if(not exists $groups{$ugroup{$user}}){
        $groups{$ugroup{$user}}{"array"} = 0;
        foreach my $p (@ar_part)
        {
            $grouppart{$ugroup{$user}}{$p}=0;
        }
    }
    #--------------------------------------------------------------------------------
    # group accounting
    $groups{$ugroup{$user}}{$state}+= $nCPU;
    $groups{$ugroup{$user}}{"total"}+= $nCPU;
    if($isArray eq 1){
        $groups{$ugroup{$user}}{"array"} += $nCPU;
    }
    if($state eq "R"){
        $grouppart{$ugroup{$user}}{$partition}  += $nCPU;
    }
    #--------------------------------------------------------------------------------
}

#--------------------------------------------------------------------------------
# create entries for running jobs
# for each user if not existing
for my $uname (sort (keys(%userjobs))) {
    if(not exists $userjobs{$uname}{"R"}){
        $userjobs{$uname}{"R"} = 0;
    }
}

for my $groupname (sort (keys(%groups))) {
    if(not exists $groups{$groupname}{"R"}){
        $groups{$groupname}{"R"} = 0;
    }
}
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# user jobs
print "--------------------------------------------------------------------------------\n";
#print "JOBS per USER :  run    total     array       group          \n";
print "JOBS per USER :  run    total     array       group ";
foreach my $p (@ar_part)
{
    printf " %6s",$p;
}
print "\n";

my $sumjobs    = 0;
my $sumjobstot = 0;

if(defined $wLogUser) {printf $wLogUser $timestamp, " "; }

for my $uname (sort (keys(%userjobs))) {
    $sumjobs    += $userjobs{$uname}{"R"};
    $sumjobstot += $userjobs{$uname}{"total"};
    printf "%11s : %6i , %6i ,  %6i  %10s ",$uname,$userjobs{$uname}{"R"},$userjobs{$uname}{"total"},$userjobs{$uname}{"array"},$ugroup{$uname};
    foreach my $p (@ar_part)
    {
        printf " %6i",$userpart{$uname}{$p};
    }
    print "\n";
    
    
    if(defined $wLogUser) { printf $wLogUser " %s:%i:%i:%s:%s", $uname,$userjobs{$uname}{"R"},$userjobs{$uname}{"total"},$ugroup{$uname}; }
}
if(defined $wLogUser) { printf $wLogUser " \n";}

printf "        SUM : %6i , %6i \n",$sumjobs,$sumjobstot;
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# group jobs
print "--------------------------------------------------------------------------------\n";
#print "JOBS per GROUP : run    total     array  \n";
print "JOBS per GROUP : run    total     array ";
foreach my $p (@ar_part)
{
    printf " %6s",$p;
}
print "\n";
$sumjobs    = 0;
$sumjobstot = 0;

if(defined $wLogGroup) { printf $wLogGroup $timestamp, " "; }

for my $group (sort (keys(%groups))) {
    $sumjobs    += $groups{$group}{"R"};
    $sumjobstot += $groups{$group}{"total"};
    printf "%11s : %6i , %6i ,  %6i ",$group,$groups{$group}{"R"},$groups{$group}{"total"},$groups{$group}{"array"};
    foreach my $p (@ar_part)
    {
        printf " %6i",$grouppart{$group}{$p};
    }
    print "\n";
    
    if(defined $wLogGroup) { printf $wLogGroup " %s:%i:%i", $group,$groups{$group}{"R"},$groups{$group}{"total"}; }
}
if(defined $wLogGroup) { printf $wLogGroup " \n"; }

printf "        SUM : %6i , %6i \n",$sumjobs,$sumjobstot;
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# cluster status
print "--------------------------------------------------------------------------------\n";
print `sinfo -h --format="%10P max time %10l default time %10L mem %8m Mb number of nodes %D"`;


#print `sinfo`;


if(defined $wLogUser) { undef $wLogUser; }
if(defined $wLogGroup){ undef$wLogGroup; }



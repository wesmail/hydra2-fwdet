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
my $priority;
my $script;
my $user;
my $state;
my $date;
my $time;
my $queue;
my $slots;
my $taskid;

my $array_size;



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
our %user_project;  # hash  user => project  (key for each statetype -> counter for that type)
our %groups;        # hash  groupname ==> number of jobs
our %ugroup;        # hash  user ==> group

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

my $timestamp = time();
my $stat=`qstat -u "*"`;
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
    if($line=~/job-ID/  || $line=~/-----------------------/ || $line=~/Eqw/)
    {
        #print "skip\n";
    } else {

        $ct++;

        $slots      = 0;
        $taskid     = 0;
        $queue      = 0;
        $array_size = 0;
        #job-ID  prior   name       user         state submit/start at     queue                          slots ja-task-ID
        #-----------------------------------------------------------------------------------------------------------------
        # 307033 0.04055 jobScript. cbehnke      r     01/14/2014 13:05:13 default@lxb812.gsi.de              1 4353                 10args
        # 307128 0.25309 h11c_rest1 fischer      r     01/14/2014 11:13:56 default@lxb1042.gsi.de            12                       9args
        # 324129 0.00071 ntupleJobS harabasz     qw    01/14/2014 11:52:08                                    1                       8args
        # 327140 0.00007 sschuchm_P sschuchm     qw    01/14/2014 12:28:52                                    1 1-45:1                9args

        ($jobid,$priority,$script,$user,$state,$date,$time,$queue,$slots,$taskid) = unpack("A7 A8 A11 A12 A6 A12 A9 a33 a4 a21",$line) ;


        $jobid=~s/ //g;
        $priority=~s/ //g;
        $script=~s/ //g;
        $user=~s/ //g;
        $state=~s/ //g;
        $date=~s/ //g;
        $time=~s/ //g;
        $queue=~s/ //g;
        $slots=~s/ //g;
        $taskid=~s/ //g;

        if( $queue eq "") {
            $queue = "no_queue";
        }
        if( $taskid eq "" ){
            $taskid = "no_array";
        }

        if($taskid ne "no_array"){

            if ($taskid=~m/:1/){
                $taskid=~m/(\d+)-(\d+):1/;
                my $first=$1;
                my $last=$2;
                $array_size = $last-$first+1;
            }
        }

        #--------------------------------------------------------------------------------
        if(not exists $userjobs{$user}){         # first time the user appears

            if(not exists $user_project{$user}){ # check the project for the first job only
                                                 # to slow to do it for each job
                my $project_tmp = `qstat -j $jobid | grep project:`;
                if( $project_tmp ne "" ){
                    $project_tmp=~s/\s+/ /g;
                    ($dummy,$project) = split(/ /,$project_tmp);
                } else {
                    $project = "no_project";
                }
                $user_project{$user}=$project;
            }
            $userjobs{$user}{"array"} =0;



        }
        #--------------------------------------------------------------------------------


        #--------------------------------------------------------------------------------
        # user accounting
        if( $state eq "r" ){        # parallel jobs accounting
            $userjobs{$user}{$state}  += $slots;
            $userjobs{$user}{"total"} += $slots;

            if($taskid ne "no_array"){
                $userjobs{$user}{"array"} += $slots;
            }

        } else {

            if($array_size > 0){
                $userjobs{$user}{$state}+=$array_size;
                $userjobs{$user}{"total"}+=$array_size;
                $userjobs{$user}{"array"}+=$array_size;

            } else {
                $userjobs{$user}{$state}++;
                $userjobs{$user}{"total"}++;
            }
        }
        #--------------------------------------------------------------------------------

        #--------------------------------------------------------------------------------
        # get groupname
        if(not exists $ugroup{$user}){
            my $group;
            my $group_tmp = `qstat -j $jobid | grep group:`;
            if( $group_tmp ne "" ){
                $group_tmp=~s/\s+/ /g;
                ($dummy,$group) = split(/ /,$group_tmp);
                $ugroup{$user}=$group;
            }
        }
        #if(not exists $ugroup{$user}){
        #    ($name, $pass, $uid, $gid, $quota, $comment, $gcos, $dir, $shell, $expire) = getpwnam($user);
        #    my $groupname =  getgrgid($gid);
        #    $ugroup{$user}=$groupname;
        #}
        #--------------------------------------------------------------------------------

        if(not exists $groups{$ugroup{$user}}){
           $groups{$ugroup{$user}}{"array"} = 0;
           $groups{$ugroup{$user}}{"project"} = 0;
        }
        #--------------------------------------------------------------------------------
        # group accounting
        if( $state eq "r" ){        # parallel jobs accounting
            $groups{$ugroup{$user}}{$state}+= $slots;
            $groups{$ugroup{$user}}{"total"}+= $slots;
            if($taskid ne "no_array"){
                $groups{$ugroup{$user}}{"array"}+= $slots;
            }

            if ($user_project{$user} ne "no_project"){
                $groups{$ugroup{$user}}{"project"}+= $slots;
            }
        } else {
            if($array_size > 0){
                $groups{$ugroup{$user}}{$state}+=$array_size;
                $groups{$ugroup{$user}}{"total"}+=$array_size;
                $groups{$ugroup{$user}}{"array"}+=$array_size;

                if ($user_project{$user} ne "no_project"){
                    $groups{$ugroup{$user}}{"project"}+= $array_size;
                }


            } else {
                $groups{$ugroup{$user}}{$state}++;
                $groups{$ugroup{$user}}{"total"}++;

                if ($user_project{$user} ne "no_project"){
                    $groups{$ugroup{$user}}{"project"}++;
                }

            }
        }
        #--------------------------------------------------------------------------------
    }
}

#--------------------------------------------------------------------------------
# create entries for running jobs
# for each user if not existing
for my $uname (sort (keys(%userjobs))) {
    if(not exists $userjobs{$uname}{"r"}){
        $userjobs{$uname}{"r"} = 0;
    }
    if(not exists $userjobs{$uname}{"s"}){
        $userjobs{$uname}{"s"} = 0;
    }
}

for my $groupname (sort (keys(%groups))) {
    if(not exists $groups{$groupname}{"r"}){
        $groups{$groupname}{"r"} = 0;
    }
    if(not exists $groups{$groupname}{"s"}){
        $groups{$groupname}{"s"} = 0;
    }
}
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# user jobs
print "--------------------------------------------------------------------------------\n";
print "JOBS per USER :  run     susp     total   array       group              project\n";

my $sumjobs    = 0;
my $sumjobsSusp= 0;
my $sumjobstot = 0;

if(defined $wLogUser) {printf $wLogUser $timestamp, " "; }

for my $uname (sort (keys(%userjobs))) {
    $sumjobs    += $userjobs{$uname}{"r"};
    $sumjobsSusp+= $userjobs{$uname}{"s"};
    $sumjobstot += $userjobs{$uname}{"total"};
    printf "%11s : %6i , %6i ,  %6i  %6i  %10s %20s\n",$uname,$userjobs{$uname}{"r"},$userjobs{$uname}{"s"},$userjobs{$uname}{"total"},$userjobs{$uname}{"array"},$ugroup{$uname},$user_project{$uname};
    if(defined $wLogUser) { printf $wLogUser " %s:%i:%i:%s:%s", $uname,$userjobs{$uname}{"r"},$userjobs{$uname}{"total"},$ugroup{$uname},$user_project{$uname}; }
}
if(defined $wLogUser) { printf $wLogUser " \n";}

printf "        SUM : %6i , %6i ,  %6i \n",$sumjobs,$sumjobsSusp,$sumjobstot;
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# group jobs
print "--------------------------------------------------------------------------------\n";
print "JOBS per GROUP : run     susp     total  array  project\n";
$sumjobs    = 0;
$sumjobsSusp= 0;
$sumjobstot = 0;

if(defined $wLogGroup) { printf $wLogGroup $timestamp, " "; }

for my $group (sort (keys(%groups))) {
    $sumjobs    += $groups{$group}{"r"};
    $sumjobsSusp+= $groups{$group}{"s"};
    $sumjobstot += $groups{$group}{"total"};
    printf "%11s : %6i , %6i ,  %6i %6i   %6i\n",$group,$groups{$group}{"r"},$groups{$group}{"s"},$groups{$group}{"total"},$groups{$group}{"array"},$groups{$group}{"project"};
    if(defined $wLogGroup) { printf $wLogGroup " %s:%i:%i", $group,$groups{$group}{"r"},$groups{$group}{"total"}; }
}
if(defined $wLogGroup) { printf $wLogGroup " \n"; }

printf "        SUM : %6i , %6i ,  %6i \n",$sumjobs,$sumjobsSusp,$sumjobstot;
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# cluster status
print "--------------------------------------------------------------------------------\n";
print `qstat -g c`;


if(defined $wLogUser) { undef $wLogUser; }
if(defined $wLogGroup){ undef$wLogGroup; }



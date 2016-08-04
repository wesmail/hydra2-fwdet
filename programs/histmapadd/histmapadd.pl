#!/usr/bin/perl

#Quick parallel merging of root files using multiple instances of hadd
#Number of parallel tasks and files merged per task can be adjusted by variables below
#optimized for up to 5000 individual files
#Jan Michel, Oct. 2012

use warnings;
use strict;
use Data::Dumper;

my $max_parallel_tasks = 6;
my $max_files_per_job = 50;


if(!defined $ARGV[0] || !defined $ARGV[1] || $ARGV[0] =~ /help$/) {
  print "histmapadd.pl - wrapper for histmapadd for high input file count.\n";
  print "usage: histmapadd.pl  outputfile  inputfiles\n\n";
  print "Quick parallel merging of root files using multiple instances of histmapadd\n";
  print "Number of parallel tasks and files merged per task can be adjusted by variables below\n";
  print "optimized for up to 5000 individual files\n";
  print "Jan Michel,  10.2012\n\n";
  exit;
  }

my ($output, @input) = @ARGV;
my $num_of_files = scalar @input;

my $cmd = "ls -1 delme_histmapadd_temporary_files_${output}_*.root 2>/dev/null";
my @o = qx($cmd);
if (scalar @o > 0) {
  print "Found files with name 'delme_histmapadd_temporary_files_${output}_*.root' that may interfere with operation of this tool. Remove them.\n"; 
  exit -1;
  }

$cmd = "ls -1 $output 2>/dev/null";
@o = qx($cmd);
if (scalar @o > 0) {
  print "Found a file with name $output that will interfere with your output file. Remove it.\n"; 
  exit -1;
  }

print ("Found ".($num_of_files)." input files\n");

my $filespertask = int(($num_of_files)/$max_parallel_tasks) + 1;
my @pids; 

for(my $i = 0; $i<$max_parallel_tasks;$i++) {
  my $p = fork();
  if(!$p) {
    print "Starting task $i...\n";
    my $fp = $i*$filespertask;
    my $r = 0;
    my $lastfile = $fp+$filespertask-1;
    if ($lastfile >= $num_of_files) {$lastfile = $num_of_files -1;}
    
    while($fp < $lastfile) {
      my $ofile = "delme_histmapadd_temporary_files_${output}_".$i."_$r".".root";
      my $cmd = "histmapadd -o $ofile -i ";
      for(my $f = 0;$fp <= $lastfile && $f < $max_files_per_job;$fp++,$f++) {
        $cmd .= $input[$fp].",";
      }
      system($cmd);
      $r++;
      }
    exit 1;
    }
  else {
    push (@pids,$p);
    }
  }

foreach my $p (@pids) {
  waitpid($p,0);
  }

print "Joining temporary files\n";
$cmd = "histmapadd -o $output -i \"delme_histmapadd_temporary_files_${output}_*.root\"";
system($cmd);
$cmd = "rm delme_histmapadd_temporary_files_${output}_*.root";
system($cmd);

print "Done;\n\n";


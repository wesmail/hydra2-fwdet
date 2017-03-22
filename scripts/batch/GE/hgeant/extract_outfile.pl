#!/usr/bin/perl 
use strict;
use warnings;
use IO::File;
use Getopt::Std;
use Data::Dumper;


my $help = "usage : extract_outfile.pl -i input1\n";

my $inputFile1      = "";

my $NARGS = $#ARGV ; #  $#ARGV will be rest by getopt()!!!

my %args;
getopt('i', \%args);

$inputFile1 = $args{"i"} ? $args{"i"} : "" ;

my %parameters = (
                  "inputFile1"  => $inputFile1
                 );

#print Dumper \%parameters;

if($NARGS < 1 || $inputFile1 eq ""
  ){
    print $help;
    exit(1);
}

open (rFile,"$inputFile1")
    or die "Cannot open input file: $inputFile1!";

my $outfile="";

while(<rFile>)
{
    my $line = $_;
    
    if($line=~/^\//){  # all lines starting with /
        if(!($line=~/^\/\//)){ # skipp the comented line
            $outfile = $line;            # the last matching line is the outfile
            $outfile=~s/_.root$/_1.root/;
        }
    }
}
close rFile;

print $outfile;


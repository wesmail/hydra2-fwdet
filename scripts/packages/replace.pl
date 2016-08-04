#!/usr/bin/perl 
use strict;
use warnings;
use IO::File;
use Getopt::Std;
use Data::Dumper;


my $help = "usage : replace.pl -i inputfile -o outputfile [-k keywords] [-l lines] [-d yes]
    \t This script replaces the keywords or line give by -k or -l,
    \t in inputfile creating outputfile. Both files can be identical.
    \t The list of keywords have the format \"key1#replace1,key2#replace2\". 
    \t Only the first occurence of the keywords will be replaced in the line.
    \t The list of lines have the format \"key1#replace1,key2#replace2\". 
    \t The full line matching the key at the beginning (ignoring whitespace)
    \t will be replaced with replace line.
    \t keywords and line replacement cannot be used simultaneously.
    \t
    \t example :  replace.pl -i Makefile -o Makefile -l \"export USES_ORACLE#export USES_ORACLE:=no\" 
    \t example :  replace.pl -i Makefile -o Makefile -k \"USES_ORACLE:=yes#USES_ORACLE:=no,USES_RFIO:=yes#USES_RFIO:=no\"
    \n";

my $lines        = "";
my $keywords     = "";
my $inputFile    = "";
my $outputFile   = "";
my $debug        = 0;

my $NARGS = $#ARGV ; #  $#ARGV will be rest by getopt()!!!

my %args;
getopt('iokld', \%args);

$lines      = $args{"l"} ? $args{"l"} : "" ;
$keywords   = $args{"k"} ? $args{"k"} : "" ;
$inputFile  = $args{"i"} ? $args{"i"} : "" ;
$outputFile = $args{"o"} ? $args{"o"} : "" ;
$debug      = $args{"d"} ? 1 : 0 ;

my %parameters = (
                  "inputFile"  => $inputFile,
                  "outputFile" => $outputFile,
                  "lines"      => $lines,
                  "keywords"   => $keywords
                 );

print Dumper \%parameters;

if($NARGS < 2 || 
   $inputFile  eq "" ||
   $outputFile eq "" ||
   ($lines ne "" && $keywords ne "")
  ){
    print $help;
    exit(1);
}

my %Lines;
my %Keys;



if($keywords ne ""){

    my @arlines = split(/,/,$keywords);
    
    foreach my $l (@arlines) { 
        my $key;
        my $line;
        
        ($key,$line) = split(/#/,$l);
        
        if(not exists $Keys{$key}){
            $Keys{$key} = $line;
        } else {
            print "Error: Key $key exists already! \n";
        }
    } 
    if($debug == 1){ 
        print "Keys : ", Dumper \%Keys;
    }
}



if($lines ne ""){

    my @arlines = split(/,/,$lines);
    
    foreach my $l (@arlines) { 
        print $l,"\n";
        my $key;
        my $line;
        
        ($key,$line) = split(/#/,$l);
        chomp($line);
        if(not exists $Lines{$key}){
            $Lines{$key} = "$line\n";
        } else {
            print "Error: Key $key exists already! \n";
        }
    }
    if($debug == 1){
        print "Lines : ", Dumper \%Lines;
    }     
}



 
open (rFile,"$inputFile")
    or die "Cannot open input file: $inputFile!";

open (wFile,">","$outputFile")
    or die "Cannot open output file: $outputFile!";


while(<rFile>)
{   
    my $inLine = $_;
    
    # check the line for all replace keys
    foreach my $k (keys (%Keys)) { 
        $inLine =~s/$k/$Keys{$k}/;
    }
    # check the line for all replace lines
    foreach my $k (keys (%Lines)) { 
        if( $inLine =~m/^\s*$k/){
          $inLine = $Lines{$k};
        }
    }
    print wFile "$inLine";
}

close rFile;
close wFile;

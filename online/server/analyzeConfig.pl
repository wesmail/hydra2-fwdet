use IO::File;



# expects clientconfig xml file as input
# prints  host and port to standardout
# if host ist not found "notfound" will 
# be returned for the port number 
# respectively -1
my $config=$ARGV[0];
 
open (rFile,"$config")
    or die "Cannot open initial ini file: $!";


my $host="notfound";
my $port=-1;


while(<rFile>)
{
    my $line = $_;
    
    if($line=~m/<port>\s*(\d+)\s*<\/port>/){
        $port=$1;
    }
    if($line=~m/<host>\s*(\w+\.\w+\.\w+)\s*<\/host>/){
        $host=$1;
    }
    if($line=~m/<host>\s*(\w+)\s*<\/host>/){  # with out gsi.de
        $host=$1;
    }
    
}

print $host ," ", $port, "\n";

close rFile;

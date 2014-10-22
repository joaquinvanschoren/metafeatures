package netlock;
# module to provide very basic filename-level
# locks.  No fancy systems calls.  In theory,
# directory info is sync'd over NFS.  Not
# stress tested.

# this code is take from "The Perl Cookbook" by Tom Christiansen
# abd Nathan Torkington, O'Reilly, 1998.
# Changes to original version:
#   - adapt to local use
#   - return 0 if locking successful, lockee otherwise

use Exporter;
use vars qw(@ISA @EXPORT);
@ISA      = qw(Exporter);
@EXPORT   = qw(nflock nunflock);

use vars qw($Debug $Check);
$Debug  ||= 0;  # may be predefined

use Cwd;
use Fcntl;
use Sys::Hostname;
use File::Basename;
use File::stat;
use Carp;

my %Locked_Files = ();

# usage: nflock(FILE)
sub nflock($) {
    my $pathname = shift;
    my $lockname = name2lock($pathname);
    my $whosegot = "$lockname/owner";
    my $lockee   = "";
    local *OWNER;

    # if locking what I've already locked, return
    if ($Locked_Files{$pathname}) {
        carp "$pathname already locked";
        return 1
    }

    if (!-w dirname($pathname)) {
        croak "can't write to directory of $pathname";
    }

    unless(mkdir($lockname, 0777)) {
      if (open(OWNER, "< $whosegot")) {
	$lockee = <OWNER>;
	chomp($lockee);
	close OWNER;
      } else {
	$lockee = "Unknown: cannot open $whosegot for reading";
      }
      return $lockee;
    }
    sysopen(OWNER, $whosegot, O_WRONLY|O_CREAT|O_EXCL)
                            or croak "can't create $whosegot: $!";
    printf OWNER "$0\[$$] on %s since %s\n",
            hostname(), scalar(localtime);
    close(OWNER)                
        or croak "close $whosegot: $!";
    $Locked_Files{$pathname}++;
    return 0;
}

# free the locked file
sub nunflock($) {
    my $pathname = shift;
    my $lockname = name2lock($pathname);
    my $whosegot = "$lockname/owner";
    unlink($whosegot);
    carp "releasing lock on $lockname" if $Debug;
    delete $Locked_Files{$pathname};
    return rmdir($lockname);
}

# helper function
sub name2lock($) {
    my $pathname = shift;
    my $dir  = dirname($pathname);
    my $file = basename($pathname);
    $dir = getcwd() if $dir eq '.';
    my $lockname = "$dir/$file.LOCKDIR";
    return $lockname;
}

# anything forgotten?
END {
    for my $pathname (keys %Locked_Files) {
        my $lockname = name2lock($pathname);
        my $whosegot = "$lockname/owner";
        carp "releasing forgotten $lockname";
        unlink($whosegot);
        return rmdir($lockname);
    }
}

1;

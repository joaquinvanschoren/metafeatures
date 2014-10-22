# run_lib.pm: common routines and code for all run_???_* scripts
# Author: Johann Petrak 


# NOTE: the 'portable' way of limiting the elapsed time doesnt seem
# to work: it seems as if the childprocess started by the pipe 
# cannot be terminated in all circumstances. Anyway, we did
# not find a reliable solution yet, so good old ulimit system
# command will be used ...
# I cannot reconstruct that problem with a simple collection of scripts???


$libversion = '$Revision: 3.0.2.2 $';
$pgmdate    = '$Date: 2002/10/17 22:04:13 $';
$rcsid      = '$Id: run_lib.pm,v 3.0.2.2 2002/10/17 22:04:13 johann Exp $';
$dummy      = '$';   # just here to satisfy the dumb emacs fontifyer

# CHANGES:
# 2001-07-16 2.5: added parm -kmodel xxx to request that the model
#   file be copied to the specified file
# 2001-07-10 2.4: changed output marker so that instead of run_xxxx
#   we just show xxxx. but we add the suffix!
# 2001-07-10 2.3: accept but ignore the -asuf xxx flag that will
#   be used in run_exp to specify algorithm-specific suffixes
# 2000-12-18 2.2: changed bug when cpulimit is set to 0; now
#   limit command will be completely suppressed
# 2000-12-15 2.1: changed default CPU timeout to 43200 (12 hours)

#use strict;
#use diagnostics;
#use BSD::Resource qw(times);
use IPC::Open3;
use File::Copy;
use vars qw($starttime $stoptime $totaltime $run $la $verbose
	    $pgmname $pgmpath $trainargs $resubsterror $error $args $debug
	    $testargs $traintime $size $predfile $testtime $filestem
	    $methodpath $seed $opt_k
	    $opt_nopgm $opt_d $opt_v $opt_istem $opt_i 
	    $opt_ostem $opt_o $opt_h $opt_m $opt_s $opt_tmppath
	    $opt_a $opt_at $opt_ae $version $libversion 
	    $opt_cpulimit $opt_portable $opt_noportable 
	    $ostype $limitcmd $timecmd $default_cpulimit $usertime
	    $lastusertime $have_alarm  $algtype $exit $opt_asuf
	    %k
	   );
# common statements for all run_???_* scripts
# these variables are reserved!!!!
$portable = 0;
%k = ();
$default_cpulimit = 43200;
$exit = 0;
$| = 1;

$starttime = 0; $stoptime = 0;
$args = ""; $trainargs = ""; $testargs = "";
# this will start timing and open filehandle <IN> for reading output
# of the command
sub startCMD {
  my $arg = $_[0];
  printMsg("time: $timecmd, limit: $limitcmd, arg: $arg\n") if $opt_d;
  my $cmd = "$limitcmd $timecmd $_[0]"; # we did it like this earlier
  my $tmp1 = $timecmd;
  $tmp1 =~ s/%c/$arg/g;
  printMsg("timesubst: $tmp1\n") if $opt_d;
  my $tmp2 = $limitcmd;
  printMsg("vorlimitsubst: $tmp2\n") if $opt_d;
  $tmp2 =~ s/%c/$tmp1/g;
  printMsg("limitsubst: $tmp2\n") if $opt_d;
  $cmd = $tmp2;
  $lastusertime = "?"; # only needed if timing is done with timex (portable=no)
  $usertime = 0; # only used with timex
  $starttime = 0;$stoptime = 0;
  printMsg("Starttime: ".join("/",times)."\n") if $verbose;
  $starttime = (times())[$ostype eq "windows" ? 0 : 2] if $portable;
  if (!$opt_nopgm) {
    printMsg("command: $cmd\n");
    openPipe($cmd);
  } else {
    printMsg("suppressed command: $cmd\n");
  }
}

sub addCMD {
  my $cmd = "$limitcmd $timecmd $_[0]";
  my $tmp1 = $timecmd;
  $tmp1 =~ s/%c/$_[0]/g;
  my $tmp2 = $limitcmd;
  $tmp2 =~ s/%c/$tmp1/g;
  $cmd = $tmp2;
  if (!$opt_nopgm) {
    printMsg("command: $cmd\n");
    if (!$opt_nopgm) {
      close(IN);
      printMsg("WARNING: Problem closing pipe: $?\n") if $?;
    }
    if ($lastusertime ne "?") {
      # add to usertime, but only if not already reset
      $usertime += $lastusertime unless ($usertime eq "?");
    } else {
      # if there is no lastusertime but it should, reset usertime
      $usertime = "?"; 
    }
    $lastusertime = "?";
    openPipe($cmd);
  } else {
    printMsg("suppressed command: $cmd\n");
  }
}

# stop timing, close the filehandle and return CPU time of child
sub stopCMD {
  my $t1; my $t2; my $t3;
  if (!$opt_nopgm) {
    close(IN);
    printMsg("WARNING: Problem closing pipe: $?\n") if $?;
  }
  printMsg("Stoptime: ".join("/",times)."\n") if $verbose;
  $stoptime = (times())[$ostype eq "windows" ? 0 : 2] if $portable; # (user,sys,childuser,childys)
  if ($lastusertime ne "?") {
    # add to usertime, but only if not already reset
    $usertime += $lastusertime unless ($usertime eq "?");
  } else {
    # if there is no lastusertime but it should, reset usertime
    $usertime = "?"; 
  }
  if ($portable) {
    return ($stoptime - $starttime);
  } else {
    return $usertime;
  }
}

sub copyCMD {
  if ($debug) { 
    while(<IN>) {
      printMsg($_);
    }
  }
}


# remove a file
sub rmFile {
  my $filename = $_[0];
  if ($opt_k) {
    printMsg("Kept file $filename\n") if $verbose;
  } else {
    unlink $filename or printMsg("WARNING: Couldn't unlink $filename: $!\n");
    printMsg("Deleted file $filename\n") if $verbose;
  }
}

# terminate with error message
sub exitErr {
  my $msg = $_[0];
  dummyKeys();
  print "$run: ERROR: $msg";
  exit(255);
}

# print a message
sub printMsg {
  my $msg = $_[0];
  print "$run: $msg";
}

sub processOptions {
  GetOptions("istem=s", "i=s", "ostem=s", "m=s","o=s","s=s", "a:s", 
	     "at=s", "ae=s", "ao=s", "tmppath=s",
	     "d", "v", "h", "k",
	     "nopgm", "cpulimit=i", "portable", "noportable",
             "asuf=s", "kmodel=s",
	    ) or run_la_showusage();
  if ($opt_h) {
    run_la_showusage();
  }
  $run = $run . $opt_asuf;
  $debug = $opt_d;
  $verbose = $opt_v || $opt_d;
  $filestem = $opt_istem;
  printMsg("Filestem:  $filestem\n") if $opt_v;
  printMsg("Infile:    $opt_i\n") if $opt_v;
  printMsg("Outstem:   $opt_ostem\n") if $opt_v;
  $predfile = $opt_o;
  printMsg("Outfile:  $predfile\n") if $opt_v;
  $methodpath = $opt_m || $pgmpath;
  printMsg("Methods:   $methodpath\n") if $opt_v;
  $seed = $opt_s || 1;
  printMsg("Seed:      $seed\n") if $opt_v;
  $args = $opt_a || "";
  printMsg("Args:      $args\n") if $opt_v;
  $trainargs = $opt_at || "";
  printMsg("Trainargs: $trainargs\n") if $opt_v;
  $testargs = $opt_ae || "";
  printMsg("Testargs:  $testargs\n") if $opt_v;
  printMsg("Other args:  $opt_ao\n") if $opt_v;
  $opt_cpulimit = $default_cpulimit unless defined($opt_cpulimit);
  printMsg("CPU limit:  $opt_cpulimit\n") if $opt_v;
  $portable = $opt_portable if (defined($opt_portable));
  $portable = 0 if (defined($opt_noportable));
  printMsg("Suffix is $opt_asuf\n") if $opt_v;
}

sub printVal {
  my $key = $_[0];
  my $val = $_[1];
  printMsg("==== $key: $val\n");
}


sub run_la_showusage {
  my $portablestate = $portable ?  "on" : "off";
print STDERR <<USAGE
Usage: $0 [options]
-istem stem: file stem for input data/names/test files to use
-i stem: filename of input file file to use
-ostem file stem for output data/names/test files to use
-o outputfile: filename of outputfile (predictions, measures etc.)
-tmppath path: path where to store temporary data
-s seed: random seed
-m path: path to were this and related files are stored (default: $pgmpath)
-a args: additional arguments to pass to the algorithm (train + test)
-at args: additional arguments for training phase
-ae args: additional arguments for evaluation(testing) phase
-ao args: other args 
-asuf suffix: algorithm specific suffix to be used for prediction file
  (this is not used in this program but the calling program but will
  not cause an error)
-nopgm: suppress all external programs for testing
-portable: dont use open3, or the system commands timex and ulimit,
  do everything in perl, even when on a system where these 
  things were possible (default: $portablestate)
-noportable use all of above even if portable would be default
-cpulimit n: n seconds CPU time limit (default: $default_cpulimit)
  0=no limit. Note: not supproted on all systems!
-kmodel modelfile: copy model to the given file 
-k: keep temporary datasets
-v: verbose 
-d: debug (implies -v)
-h: show this help
NOTE: not all options might be supperted by all interface scripts!
Program version: $version (lib: $libversion)
USAGE
;
exit(1);
}


# this must be called as first function with the name of the 
# learning algorithm as argument
sub beginLA {
  $algtype = 'la';
  beginALG($_[0],$_[1]);
  $k{"Error"} = '?';
  $k{"Resubsterror"} = '?';
  $k{"Size"} = '?';
  $k{"Traintime"} = '?';
  $k{"Testtime"} = '?';
  $k{"Totaltime"} = '?';
}
sub beginRA {
  $algtype = 'la';
  beginALG($_[0],$_[1]);
  $k{"Error_MSE"} = '?';
  $k{"Resubsterror_MSE"} = '?';
  $k{"Size"} = '?';
  $k{"Traintime"} = '?';
  $k{"Testtime"} = '?';
  $k{"Totaltime"} = '?';
}

sub beginMA {
  $algtype = 'ma';
  beginALG($_[0],$_[1]);
}
sub beginPA {
  $algtype = 'pa';
  beginALG($_[0],$_[1]);
}
sub beginALG {
  $la = $_[0];
  $version = $_[1];
  $run = $la;
  &processOptions();
  $run = $la .$opt_asuf;
  printMsg "version $version (lib: $libversion)\n";
  &setupOS();
  printMsg("Running in portable mode\n") if $portable;
  if ($opt_cpulimit > 0) {
    $limitcmd =~ s/%t/$opt_cpulimit/g unless $portable;
    if ($portable && $opt_cpulimit) {
      # we cannot limit CPU time portably, but at least we can
      # try to dispatch a timer-interrupt
      $SIG{ALRM} = \&handler;
      alarm($opt_cpulimit) if $have_alarm;
    }
  } else {
    # clear limit command if CPU limit is 0 
    # (no effect if already cleared because of portable)
    $limitcmd = "%c";   
  }
}

sub handler {
  print "HANDLER in process $$\n";
  alarm(0);
  print "$run: ERROR: time limit exceeded! ($opt_cpulimit sec elapsed)\n"; 
  kill('KILL', -$$); 
  kill('KILL', $$); 
};

sub getLine() {
  my $line = <IN>;
  if ($line=~/Cpu Limit Exceeded/ || 
      $line=~/Command terminated abnormally/ ||
      $line=~/Command terminated by signal 24/) {
    exitErr("time limit exceeded! ($opt_cpulimit sec CPU)\n");
  } elsif (/^real / || /^user / || /^sys /) {
    printMsg($line) if $verbose;
    if ($line =~ /^user\s+([0-9:\.]+)/) {
      $lastusertime = TimeToSecs($1);
    }
  } elsif ($debug) {
    printMsg($line);
  }
  return $line;
}
# this must be called as last function!
sub endLA {
  if ($portable && $opt_cpulimit) {
    # reset the timer
    alarm(0) if $have_alarm;
  }
  if ($k{"Error"} ne "?") {
    if ($k{"Traintime"} ne "?" && $k{"Testtime"} ne "?") {
      $k{"Totaltime"} = $k{"Traintime"} + $k{"Testtime"};
    } else {
      $k{"Totaltime"} = "?"
    }
    foreach $key (sort keys %k) {
      printVal("$key", $k{"$key"});
    }
    printMsg("!!!! finished\n");
  } else {
    dummyKeys();
    printMsg("no values written, since classification error is missing\n");
  }
}
sub endRA {
  if ($portable && $opt_cpulimit) {
    # reset the timer
    alarm(0) if $have_alarm;
  }
  if ($k{"Traintime"} ne "?" && $k{"Testtime"} ne "?") {
      $k{"Totaltime"} = $k{"Traintime"} + $k{"Testtime"};
  } 
  foreach $key (sort keys %k) {
      printVal("$key", $k{"$key"});
  }
  printMsg("!!!! finished\n");
}

sub endPA {
  endMA();
}
sub endMA {
  if ($portable && $opt_cpulimit) {
    # reset the timer
    alarm(0) if $have_alarm;
  }
  foreach $key (sort keys %k) {
    printVal("$key", $k{"$key"});
  }
  printMsg("!!!! finished\n");
}

sub dummyKeys {
  foreach $key (sort keys %k) {
    printVal("$key", "?");
  }
}

sub fileSize {
  my $file = $_[0];
  my $count = 0;
  open(TMP1, "< $file") or exitErr("Could not open $file for counting: $!");
  $count++ while  <TMP1>;
  close(TMP1);
  return $count;
}

sub setupOS {
  my $os = $^O;
  $limitcmd = "%c";
  $timecmd = "%c";
  $have_alarm = 1;
  printMsg("Setting up for OS $os\n") if $opt_d;
  if ($os =~ /win32/i || $os =~ /cygwin/i) {
    $ostype = "windows";
    $portable = 1;    # even if not given, we need that on windows
    eval { alarm(0) };
    if ($@) {
      printMsg($@) if $opt_v;
      $have_alarm = 0;
    }
  } elsif ($os eq "solaris") {
    $ostype = "unix";
    unless ($portable) {
      $limitcmd = "ulimit -t %t; %c";
      $timecmd = "timex %c";
    }
  } elsif($os eq "linux") {
    $ostype = "unix";
    unless ($portable) {
      $timecmd = "/usr/bin/time -p %c";
      $limitcmd = "csh -f -c \"limit cputime %t; %c\"";
    }
  }else {
    printMsg("potential problem - unknown OS: $os\n");
    unless ($portable) {
      printMsg("Portability mode turned on for unknown OS\n");
      $portable = 1;
    }
    $ostype = "unix";
  }
  printMsg("OsType is $ostype, timecmd is $timecmd, limitcmd is $limitcmd\n") if $opt_d;
}
sub openPipe {
  if ($portable || $ostype eq "windows") {
    open(IN, "$_[0] |") or exitErr("WARNING: Could not create pipe: $_[0]: $!\n");
  } else {
    open3(DUMMY,IN,IN,$_[0]) or exitErr("WARNING: Could not create pipe: $_[0]: $!\n");
  }
}

sub TimeToSecs {
  my $arg1 = $_[0];
  my $h = 0; my $m = 0; my $s = 0.0;
  if ($arg1 =~ /([0-9]+):([0-9]+):([0-9\.]+)/) {
    $h = $1; $m = $2; $s = $3;
  } elsif  ($arg1=~/([0-9]+):([0-9\.]+)/) {
    $m = $1; $s = $2;
  } elsif  ($arg1=~/([0-9\.]+)/) {
    $s = $1;
  } else {
    return '?';
  }
  return (3600 * $h + 60 * $m + $s);
}


sub getparm {
  my $key = $_[0];
  my $string = $_[1];
  my $k;
  my $v;
  foreach $pair ( split(",",$string) ) {
    ($k,$v) = split("=",$pair);
    if ($key eq $k) {
      return $v;
    }}
}

# some comments on the use of open3, time, and the OS commands 
# ulimit and timex:
# perl will only obtain reasonable child timings with time, if
# the pipe uses open, not open3. If open3 is used, timex has to be
# used too.
# The ulimit command will terminate the child and on Solaris
# there will be the Message "Cpu Limit Exceeded" on the standard ERROR
# This can only be captured when using open3.
# open3 is very brittle, not portable and sometimes messes up
# a stream, so we dont wont to use it. But if we want to 
# get standard error we DO have to use it, oh well ....
# The above code tries to provide both alternatives:
# is open3 is used, timex is used also to get the times.
# if open is used, the builtin time function is used.
# Finally the BSD::Resource package provides an alternative time
# function that might have a better resultion. Did not test this though ...

1;

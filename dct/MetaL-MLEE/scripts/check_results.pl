#!/usr/local/bin/perl

my $version = '$Revision: 3.0 $';
my $v_date  = '$Date: 2002/06/30 19:05:19 $';
my $rcsid   = '$Id: check_results.pl,v 3.0 2002/06/30 19:05:19 johann Exp $';

# check_results
# =============
#
# check the output files for an experiment
# 
# Author: Johann Petrak (OeFAI)
#

# TODO:
# a lot can be still done, this is pretty alpha!
# - check log file for "command not found" error messages

# CHANGES:
# 20020221 1.0: initial version

#use diagnostics;
require 5.000;



use Getopt::Long;
use File::Basename;
use File::Copy;

setupOS();

use vars 
  qw{ $opt_f  $opt_l $opt_v $opt_h $opt_d
};

# find out where we call the program from: this path will also be
# the default method path!
my $pgmname = $0;
my $pgmpath = dirname($pgmname);
push(@INC,$pgmpath);
require config;


sub showusage  {
  my $error = $_[0];
print "Error: ",$error,"\n" if ($error);
print STDERR <<USAGE
Usage: $0 -f filestemincludingseed [-N n] [-l alg1] [-l alg2] ...
-f stem: the filestem including the seed -- i.e. the part of
  the path to all result files that is common to all. 
-N n: specify that the results should contain n folds (of not
  specified, will guess from what is there)
-l alg: can be specified repeatedly and will be used as the
  list of algorithms that should have been executed. If none
  specified, the list will be guessed from what is there.
-v: verbose output - add additional, maybe redundant messages
-d: debug (implies -v and -k). for full debugging also specify -lad
-dd: even more debug messages
-h: help -- show this usage information

Program version/date: $version/$v_date
USAGE
;
print "\nError: ",$error if ($error);
exit(1);
}

my $parmlist = join(" ",@ARGV);

GetOptions("f=s","v", "h", "d", "l=s\@","N=i","dd",
	  ) or showusage();

if ($opt_h) {
  showusage();
  exit(0);
}
$debug = 1 if (defined($opt_d));
$opt_v = 1 if (defined($opt_d));

if ($opt_f eq "") {
  showusage("Filestem required but missing!\n");
}

$haveproblems = 0;

my $filepref = $opt_f;

# STEP 1: analyze logfile
# list of algs that were tried to be executed for fold n

my %loglarun = ();
my %loglas = ();
my %logfolds = ();

my @logrundates = ();
my @logfilestems = ();
my @logseeds = ();
my $havelogfile = 1;

# process the log file
my $logfile = $filepref . ".log";
if (open(IN,$logfile)) {
  while(<IN>) {
    if (/Running experiments on file stem ([^ ]+) with seed ([^ ]+)/) {
      $filestem = $1; $seed = $2;
      push @logfilestems,$filestem;
      push @logseeds,$seed;
      print "LOG: Found file stem: $filestem\n" if $opt_d;
      print "LOG: Found seed: $seed\n" if $opt_d;
    } elsif (/On/ && /user/ && /host/) {
      print "LOG: Date: $_" if $opt_d;
      push @logrundates,$_;
    } elsif (/Running ([^ ,]+), iteration ([^ ,])/) {
      $logfolds{$2} = 1;
      $loglas{$1} = 1;
      $loglarun{"$1,$2"} = 1;
    }
  }
  close(IN);
  print "LOG: Folds: ",join(",",(sort keys %logfolds)),"\n" if $opt_d;
  print "LOG: LAs:   ",join(",",(sort keys %loglas)),"\n" if $opt_d;
} else {
  print "ERROR: log file $logfile not found\n" if $opt_d;
  $havelogfile = 0;
}

# STEP 2: analyze stats file
my %statslarun = ();
my %statslas = ();
my %statsfolds = ();
my $havestatsfile = 1;
my %statslacompleted = ();
my $statsfile = $filepref . ".stats";
if (open(IN,$statsfile)) {
  while(<IN>) {
    chomp;
    if (/^ErrorSSE ([^ ]+) 0 ([0-9]+): ([^ ]+)/) {
      $tmpla = $1; $tmpfold = $2; $tmperr = $3;
      $statslas{$tmpla} = 1;
      $statsfolds{$tmpfold} = 1;
      $statslarun{"$tmpla,$tmpfold"} = 1;
    } elsif (/^Error ([^ ]+) 0 ([0-9]+): ([^ ]+)/) {
      $tmpla = $1; $tmpfold = $2; $tmperr = $3;
      $statslas{$tmpla} = 1;
      $statsfolds{$tmpfold} = 1;
      $statslarun{"$tmpla,$tmpfold"} = 1;
    } elsif (/^Error ([^ ]+): ([^ ]+)/) {
      $tmpla = $1;  $tmperr = $2;
      $statslacompleted{$tmpla} = 1;
    } elsif (/^ErrorSSE ([^ ]+): ([^ ]+)/) {
      $tmpla = $1;  $tmperr = $2;
      $statslacompleted{$tmpla} = 1;
    }
  }
} else {
  print "ERROR: stats file $statsfile not found\n" if $opt_d;
  $havestatsfile = 0;
}

# STEP 3: analyze results file
my %resultslarun = ();
my %resultslas = ();
my %resultslas_spec = ();
my %resultslas_status = ();
my %resultsfolds_spec = ();
my %resultsfolds = ();
my $haveresultsfile = 1;
my $resultsfile = $filepref . ".results";
if (open(IN,$resultsfile)) {
  while(<IN>) {
    chomp;
    if (/XVAL_folds: ([^ ]+)/) {
      $resultsfolds_spec{$1} = 1;
    } elsif (/Learner: ([^ ]+)/) {
      $resultslas_spec{$1} = 1;
    } elsif (/Status ([^ ]+): ([^ ]+)/) {
      $resultslas_status{$1} = $2;
    }
  }
} else {
  print "ERROR: results file $resultsfile not found\n" if $opt_d;
  $haveresultsfile = 0;
}

# make a list of all LAs and find out mximum number of folds
my %alllas = ();
foreach $l ( keys %loglas ) {
  $alllas{$l} = 1;
}
foreach $l ( keys %statslas ) {
  $alllas{$l} = 1;
}
foreach $l ( keys %resultslas_spec ) {
  $alllas{$l} = 1;
}

# if number of folds was not specified, find out from data
my $maxfolds = 0;
foreach $f ( keys %logfolds ) {
  if ($f > $maxfolds) {
    $maxfolds = $f;
  }
}
foreach $f ( keys %resultsfolds ) {
  if ($f > $maxfolds) {
    $maxfolds = $f;
  }
}
foreach $f ( keys %resultsfolds_spec ) {
  if ($f > $maxfolds) {
    $maxfolds = $f;
  }
}
foreach $f ( keys %statssfolds ) {
  if ($f > $maxfolds) {
    $maxfolds = $f;
  }
}

my @targetlas = ();
my @alllas = ();
@alllas = sort keys %alllas;
# if las not specified: set targetlas to alllas, otherwise
# compare 
if (defined(@opt_l)) {
  @targetlas = sort @opt_l;
  my $foundlas = join(",",@alllas);
  my $speclas  = join(",",@targetlas);
  print "LAs finally found: ",$foundlas,"\n" if $opt_d;
  print "LAs specified:     ",$speclas,"\n" if $opt_d;
  if ($speclas ne $foundlas) {
    print "ERROR: las found ($foundlas) and specified ($speclas) differ\n";
    $haveproblems += 1;
  }
} else {
  @targetlas = @alllas;
  print "LAs finally found: ",join(",",@alllas),"\n" if $opt_d;

}

my $targetfolds = 0;
if ($opt_N ne "") {
  if ($maxfolds != $opt_N) {
    print "ERROR: folds found ($maxfolds) and specified ($opt_N) differ\n";
    $haveproblems += 1;
  }
  $targetfolds = $opt_N;
} else {
  $targetfolds = $maxfolds;
}

######################################################
### Print the grand summary of problems
unless ($havelogfile) {
  print "ERROR: *** logfile $logfile not found\n";
  $haveproblems += 1;
}
unless ($haveresultsfile) {
  print "ERROR: *** resultsfile $resultsfile not found\n";
  $haveproblems += 1;
}
unless ($havestatsfile) {
  print "ERROR: *** statsfile $statsfile not found\n";
  $haveproblems += 1;
}

my %havetargets = ();
my %missingtargets = ();
my %havepred = ();
my %missingpred = ();
# Check target and prediction files for all algorithms and folds
# TODO: repetitions not yet handled!
for($f=0;$f<$targetfolds;$f++) {
  print "Fold: $f\n" if $opt_d;
  $ntargs = fileSize($filepref . "_" . $f . ".targets");
  if ($ntargs eq "MISSING" || $ntargs eq 0) {
    print "Fold $f: targets file missing or empty!\n" if $opt_v;
    $missingtargets{$f} = 1;
  } else {
    $havetargets{$f} = 1;
    print "Filesize of targets: $ntargs\n" if $opt_d;
    foreach $l ( @targetlas) {
      $npreds = fileSize($filepref . "_" . $f . "_" . $l . ".pred");
      if ($npreds eq "MISSING") {
	print "Fold $f: pred file for $l missing\n" if $opt_v;
	$missingpred{"$f,$l"} = 1;
      } else {
	print "Filesize of pred for $l: $npreds\n" if $opt_d;
	if ($npreds != $ntargs) {
	  print "Fold $f, la $l: number of targs and preds differ ($ntargs/$npreds)\n" if $opt_v;
	  $missingpred{"$f,$l"} = 1;
	} else {
	  $havepred{"$f,$l"} = 1;
	}
      }
    }
  }
}




# what is the first fold that didnt work for a LA
%firstfailed = ();
foreach $f ( sort keys %havetargets ) {
  foreach $l ( @targetlas ) {
    if($missingpred{"$f,$l"}) {
      unless (defined($firstfailed{$l})) {
	$firstfailed{$l} = $f;
      }
    }
  }
}

foreach $f ( 1 .. $targetfolds ) {
  if ($missingtargets{$f}) {
    print "ERROR: target file missing for fold $f\n";
    $haveproblems += 1;
  }
}
foreach $l ( @targetlas ) {
  if (defined($firstfailed=$firstfailed{$l})) {
    unless ($status = defined($resultslas_status{$l})) {
      $status = "unknown";
    }
    print "ERROR: first failed fold for LA $l is $firstfailed (status $status)\n";
    $haveproblems += 1;
  }
  if ($haveresultsfile) {
    if (($tmp = $resultslas_status{$l}) ne "ok") {
      unless(defined($firstfailed)) {
	print "ERROR: latest status for $l in results file not ok but $tmp\n";
	$haveproblems += 1;
      }}
    unless ($resultslas_spec{$l}) {
      print "ERROR: algorithm $l has not been included in experiment\n";
    }
  }
  if ($havestatsfile) {
    unless ($statslacompleted{$l}) {
      unless(defined($firstfailed)) {
	print "ERROR: error for algorithm $l missing in stats file\n";
      }
    }
  }
}


if ($haveproblems) {
  print "***** $haveproblems Problems found for $opt_f\n";
} else {
  print "***** $opt_f seems OK\n";
}



sub fileSize {
  my $file = $_[0];
  my $count = 0;
  if ($ostype eq "unix") {
    if (-f $file) {
      $count = runCmd("wc -l < $file");
      $count =~ s/ //g;
      return $count;
    } else {
      return "MISSING";
    }
  } else {
    if (open(TMP1, "< $file")) {
      $count++ while  <TMP1>;
      close(TMP1);
      return $count;
    } else {
      return "MISSING";
    }
}
}
sub setupOS {
  my $os = $^O;
  printLOG("Setting up for OS $os\n") if $debug;
  if ($os eq "WIN32") {
    $nullfile = "NUL";
    $pwdcmd = "cd";
    $ostype = "windows";
  } elsif ($os eq "solaris") {
    $nullfile = "/dev/null";
    $pwdcmd = "pwd";
    $ostype = "unix";
  } else {
    print "WARNING: unknown OS $os, assuming UNIX-like\n" if $opt_d;
    $nullfile = "/dev/null";
    $pwdcmd = "pwd";
    $ostype = "unix";
  }
}
sub runCmd {
  my $cmd = $_[0];
  print "Running: $cmd\n" if $opt_dd;
  my $ret = `$cmd`;
  chomp($ret);
  print "Returned: $ret\n" if $opt_dd;
  return $ret;
}



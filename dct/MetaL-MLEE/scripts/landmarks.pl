#!/usr/local/bin/perl -w

my $version = '$Revision: 3.0 $';
my $vdate   = '$Date: 2002/06/30 19:05:19 $';
my $rcsid   = '$Id: landmarks.pl,v 3.0 2002/06/30 19:05:19 johann Exp $';

# program for computing landmarks for MetaL

# it computes 

# Version 1.1
# J. Fuernkranz
# May 2001

# BUGS: (?)
# if you make a decision stump for a dataset with a very skewed data
# distribution, both nodes will have the majority class at each leaf node
# for every possible split. Hence all attributes will look the same!!!

$| = 1;

use File::Basename;
use Getopt::Long;

#use lib '/westbahn/juffi/lib/';
#use MLutils;

# set default values

my $file_stem;

local $opt_grdiff = 0.1;  # default value

# check for valid options and presence of an argument (filestem)
# unless (GetOptions( qw(train! test! keep! double! debug! 
#                      labels=s options=s ripperpreds)) &&
# GetOptions() && ($file_stem = shift)
GetOptions (qw(grdiff=s))
  && ($file_stem = shift)
  or die <<usage;
landmarks.pl <file_stem>
  - grdiff   gain ratio difference for best attributes (default: 0.1)
V1.1, Johannes Fürnkranz, 2001
usage

# check if names and data files exist
die "Could not find $file_stem.names" if (not -e "$file_stem.names");
die "Could not find $file_stem.data"  if (not -e "$file_stem.data");

# get the path and stem
my ($true_stem, $path) = fileparse($file_stem,'\..*');
$file_stem = $path . $true_stem; # this drops the suffix

# read names file
print "Reading in $true_stem.names...\n";
my @names = read_c45_names("$file_stem.names");

# read training data (also for testing mode only)
my @data = ();
print "Reading in $true_stem.data...\n";
@data = read_c45_file("$file_stem.data");
print scalar(@data), " examples read.\n";

# read in test data if available/desired
#my @test = ();
#if ($opt_test && not -e "$file_stem.test") {
#  warn "Test file $file_stem.test not found. Ignoring...";
#  $opt_test = 0;
#
#if ($opt_test) {
#  print "Reading in $true_stem.test...\n";
#  @test = read_c45_file("$file_stem.test");
#  print scalar(@test), " examples read.\n";
#}

my %class_freqs = class_frequencies(@data);
# compute default accuracy
my $default_accuracy = 0;
foreach (keys %class_freqs) {
  $default_accuracy = $class_freqs{$_} if $class_freqs{$_} > $default_accuracy;
}
# compute the information content
my $info = entropy(values %class_freqs);
# now check all attributes except class
my @info;
my @splits;
foreach $i (1..$#names) {
  my %split;
  undef %split;
  if ($names[$i][1] eq 'continuous') {
    # handle continuous attribute
    # look for best threshold
    
    my @example_index;
    my %class_frequencies_missing;
    for $j (0..$#data) {
      if ($data[$j][$i] eq '?') { # treat missing values as a separate value
        $class_frequencies_missing{$data[$j][0]}++;
      }
      else {
        push @example_index, $j;
      }
    }
    
    # sort attribute by values and class values
    @example_index = sort { $data[$a][$i] <=> $data[$b][$i] ||
                            $data[$a][0] cmp $data[$b][0] }
                          @example_index;
    
    my %class_frequencies_ge = class_frequencies(@data);
    foreach (keys %class_frequencies_missing) {
      $class_frequencies_ge{$_} -= $class_frequencies_missing{$_};
    }
    my %class_frequencies_lt;
    
    # print "EXAMPLES: @example_index\n";

    my $best_ig;
    my $best_val;
    my %best_split;
    my $first_ex = shift @example_index;
    my $class = $data[$first_ex][0];
    my $val = $data[$first_ex][$i];
    $class_frequencies_ge{$class}--;
    $class_frequencies_lt{$class}++;
    foreach my $ex_ref (@example_index) {
      my $last_class = $class;
      $class = $data[$ex_ref][0];
      my $last_val = $val;
      $val = $data[$ex_ref][$i];
      # print "CF_LT:\n";
      # foreach (keys %class_frequencies_lt) {
      #   print "  $_: $class_frequencies_lt{$_}\n";
      # }
      # print "CF_GE:\n";
      # foreach (keys %class_frequencies_ge) {
      #   print "  $_: $class_frequencies_ge{$_}\n";
      # }
      # class change and value change
      if (($class ne $last_class) and ($val != $last_val)) {
        # a potential split point
        my %split = ("< $val" => \%class_frequencies_lt,
                     ">= $val" => \%class_frequencies_ge,
                     '?' => \%class_frequencies_missing);
        my $ig = average_entropy(%split);
        if (not(defined $best_ig) or ($ig < $best_ig)) {
          $best_ig = $ig;
          $best_val = $val;
          # copy the hash
          undef %best_split;
          foreach (keys %split) {
            # this does a copy????
            my %newhash = %{ $split{$_} };
            $best_split{$_} = \%newhash;
#            print "S: $split{$_} N: ", \%newhash, "\n";
          }
        }
      }
      $class_frequencies_ge{$class}--;
      $class_frequencies_lt{$class}++;
    }
    %split = %best_split;
  }
  else {
    %split = attribute_class_frequencies($i,@data);
#    print "Attribute $names[$a][0]\n";
#    printf "Information Content: %f\n", info_gain(%freqs);
#    foreach $v (keys %freqs) {
#      print "  Value $v\n";
#      foreach $c (keys %{$freqs{$v}}) {
#        print "    CLASS $c: $freqs{$v}{$c}\n";
#      }
#    }
  }
  print "\nAttribute  $names[$i][0]\n";
  # print "Split: \n";
  # foreach $s (keys %split) {
  #  print "$s ";
  #  foreach (keys %{ $split{$s} }) {
  #    print "$_:$split{$s}{$_} ";
  #  }
  #  print "\n";
  #} 
  print "Accuracy: ", correct_predictions(%split) || $default_accuracy, "\n";
  my $ig = $info - average_entropy(%split);
  my $si = split_info(%split);
  # printf "Information gain: %f\n", $ig;
  # printf "Split Info: %f\n", $si;
  $info[$i] = $si ? $ig / $si : 0;
  $splits[$i] = \%split;
  printf "Gain Ratio: %f\n", $info[$i];  
}
print "\n\n";

# remove the 0 elements!
shift @info;
shift @splits;

my $n = scalar(@data) / 100;
my @acc = map { (correct_predictions(%{$_}) || $default_accuracy) / $n } @splits;
my @sort_by_gain_ratio = sort {$info[$b] <=> $info[$a]} (0..$#info);
my @sort_by_accuracy = sort {$acc[$b] <=> $acc[$a] } (0..$#info);

# write out attributes within gain-ration difference
my $best_gr_att = $sort_by_gain_ratio[0];
print "Attributes within $opt_grdiff gain ratio difference: ";
foreach (@sort_by_gain_ratio) {
  last if ($info[$best_gr_att]) - $info[$_] > $opt_grdiff;
  print $_ + 1,  ' ';
#  print ($info[$_] - $info[$best_gr_att]), "\n";
}
print "\n";

# write out best node
printf "Best Node ($names[$best_gr_att+1][0]) Training Accuracy: %5.2f\n", 
  $acc[$sort_by_gain_ratio[0]];
# write out worst node
printf "Worst Node ($names[$sort_by_gain_ratio[$#info]+1][0]) Training Accuracy: %5.2f\n", 
  $acc[$sort_by_gain_ratio[$#info]];
# write out random nod
my $rn = int rand @info;
printf "Random Node ($names[$rn+1][0]) Training Accuracy: %5.2f\n", $acc[$rn];
# write out node with best accuracy
printf "Most Accurate Node ($names[$sort_by_accuracy[0]+1][0]) Training Accuracy: %5.2f\n", 
  $acc[$sort_by_accuracy[0]];
# write out node with worst accuracy
printf "Least Accurate Node ($names[$sort_by_accuracy[$#info]+1][0]) Training Accuracy: %5.2f\n", 
  $acc[$sort_by_accuracy[$#info]];
# write out average accuracy
my $avg = 0;
$avg += $_ foreach (@acc);
$avg /= @acc;
printf "Average Node Training Accuracy: %5.2f\n", $avg;

print "\n";

exit;

sub log_2 {
  return (log($_[0]) / log(2));
}

sub entropy {
  my $sum = 0;
  $sum += $_ foreach @_;
  my $ent = 0;
  foreach (@_) {
    local $_ = $_ / $sum;
    $ent -= $_ * log_2($_) if $_ > 0 and $_ < 1; 
  }
#  print "ENTROPY @_: $ent\n";
  return $ent;
}

sub average_entropy {
  my %info_values = @_;
  # compute weighted average entropy from a contingency table
  
  my $total = 0;
  my $e = 0;
  foreach $k (keys %info_values) {
    my $sum = 0;
    $sum += $_ foreach values %{ $info_values{$k} };
    $total += $sum;
    my $info = entropy(values(%{ $info_values{$k} }));
    $e += $sum * $info;
  }
  $e /= $total if $total;
  return $e;
}

sub split_info {
  my %info_values = @_;
  # computes the normalization term for the gain ratio
  my %sum;
  foreach $k (keys %info_values) {
    $sum{$k} += $_ foreach values %{ $info_values{$k} };
  }
  return entropy(values %sum);
}

sub correct_predictions {
  # compute the correct predictions in a decision node 
  # represented by its %split
  my %info_values = @_;
  my $sum = 0;
  foreach $k (keys %info_values) {
    my $max = 0;
    foreach (values %{ $info_values{$k} }) {
      $max = $_ if $_ > $max;
    }
    $sum += $max;
  }
  return $sum;
}

# imported from MLutils.pm

sub attribute_frequencies ($@) {
  # collect the frequencies of values of attribute no. $_[0]
  my %values;
  my $att_no = shift;
  foreach $ex_ref (@_) {
    if (defined $values{$$ex_ref[$att_no]}) {
      $values{$$ex_ref[$att_no]}++;
    }
    else {
      $values{$$ex_ref[$att_no]} = 1;
    }
  }
  return %values;
}

sub class_frequencies (@) {
  return attribute_frequencies(0,@_);
}

sub attribute_class_frequencies ($@) {
  # compute attribute frequencies by class frequencies
  my %values;
  my $att_no = shift;
  foreach $ex_ref (@_) {
    if (defined $values{$$ex_ref[$att_no]}{$$ex_ref[0]}) {
      $values{$$ex_ref[$att_no]}{$$ex_ref[0]}++;
    }
    else {
      $values{$$ex_ref[$att_no]}{$$ex_ref[0]} = 1;
    }
  }
  return %values;
}  
  
sub c45_to_array {
  # read one example in C4.5 format
  my @line_array;
  # strip comments at the end
  s/\s*\|.*\n//;
  # strip whitespace at the beginning
  s/^\s*//;
  # split at commas
  @line_array = split /\s*\,\s*/;
  my $class = pop @line_array;
  # strip optional period at the end
  $class =~ s/\s*(\.)?\s*$//;
  unshift @line_array, $class;
  return @line_array;
}
	
sub read_c45_file {
  # read a C4.5 file into an array
  open IN,"< $_[0]" or die "Could not open file $_[0]: $!";
  my @array;
  while (<IN>) {
    my @result = c45_to_array $_;
    if (@result > 1) {
      push @array, \@result;
    }
  }
  return @array;
  close IN;
}

sub read_c45_names {
  # read a C4.5 names file
  open IN,"< $_[0]" or die "Could not open file $_[0]: $!";
  my @names = "";
  while (<IN>) {
    while (s/,\s*\n$/,/) {
      # if line ends with a comma, attach next line
      $_ .= <IN>;
    }
    push @names, $_;
  }
  close IN;
  $seen_classes = 0;
  my @names_array;
  foreach (@names) {
    # remove comments 
    # note that Ripper comments (start with %) are not handled!
    s/\s*\|.*\n//;
    # strip whitespace at the beginning
    s/^\s*//;
    if (!/^\s*$/) {
      if (!$seen_classes) {
	$seen_classes = 1;
	# strip optional period at the end
	s/\s*(\.)?\s*$//;
	# read
	my @values = split /\s*,\s*/;
	# push dummy identifier for class variable.
	unshift @values, '_CLASS_VARIABLE_';
	push @names_array, \@values;
      }
      else {
	# strip optional period at the end
	s/\s*\.?\s*\n//;
	# replace the first colon with a comma
	s/:/,/;
	# read
	my @values = split /\s*,\s*/;
	push @names_array, \@values;
      }
    }
  }
  return @names_array;
}

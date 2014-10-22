#!/usr/local/bin/perl

my $version     = '$Revision: 3.0.2.1 $';
my $versiondate = '$Date: 2002/08/19 20:55:31 $';
my $rcsid       = '$Id: check_database.pl,v 3.0.2.1 2002/08/19 20:55:31 johann Exp $';

# check_database.pl
# CREATED: 14/06/00
# BY: Carlos Soares
#
# SYNTAX: check_database.pl -f FILESTEM [-regr] [-limit MAX_ERRORS] [-max MAX_LINES] [-dbg] [-o]
#
# COMMENTS:
# CHANGES:
# 2001/11/29 1.3 (carlos): checks for repeated attribute names
# 2001/07/20 1.2 (JP): added check for empty lines in names file
#   added check for missing new line character in last line of names file
# 2001/01/10 1.1 (JP): corrected what appears to be a bug: changed all
#   the "chop" to "chomp" to make sure that only newlines get removed
#   from the end of a line. Otherwise, we get a problem, when the
#   last line is not terminated by a NL and consequently, the
#   trailing dot is removed.
#   Also corrected a bug when printing the error message for
#   missing dot: $1 wont survive another subst command!
# (johann & carlos 30/11/00) v1.0: cleaned up code and removed bug in scientific notation
# (carlos 17/11/00) cleaned up code and error messages; check class values; in regression, check whether prediction attribute exists
# (johann 16/11/00) removed false alarm when class value is a substring of attribute value; added options to limit the number of lines analyzed and number of errors encountered.
# (carlos 16/11/00) handles regression datasets; improved use.
# (carlos 31/10/00) corrected bug caused by single-char values; detects whether class value is used in any attribute.
#
# TODO:
# -correct detection of numerical values in scientific notation
#

use Getopt::Long;

handle_pars();
$nr_msgs = 0;

if (defined($opt_o))
{
    dbg_print("Writing output to $opt_f.check_metal\n");

    open OUTFILE, ">$opt_f.check_metal";
}


if ($opt_regr)
{
    dbg_print("Checking dataset for regression\n");
}
else
{
    dbg_print("Checking dataset for classification\n");
}

#
# Check .names and set verification patterns for all attributes and class
#

dbg_print("\n >>> CHECKING $opt_f.names\n\n");

open(fpNames, "$opt_f.names");

# Prediction attribute 
if ($opt_regr)
{
    $wkPredictionAttribute = <fpNames>;
    chomp($wkPredictionAttribute);
    if ( ! ($wkPredictionAttribute =~ m/^([A-Za-z_]\w*)[.]$/) )
    {
	print_error("*** Invalid prediction attribute name: $wkPredictionAttribute\n");
    }
    $wkPredictionAttribute =~ s/(.*)[.]$/$1/;

    dbg_print("\tprediction attribute is $wkPredictionAttribute\n");
}
else
{
    $wkClassPattern = <fpNames>;
    chomp($wkClassPattern);
    dbg_print("\tclass is one of: $wkClassPattern");
    $wkClassPattern =~ s/\s//g;
    $wkClassPattern =~ s/,/|/g;
    $wkClassPattern =~ s/[.]//;

    # Check class values
    foreach $wkValue (split(/\|/, $wkClassPattern))
    {
	dbg_print("\t\tchecking $wkValue\n");
	if ( ! ($wkValue =~ m/^([A-Za-z]\w*)$/ ))
	{
	    print_error("*** Invalid class value ($wkValue)\n");
	}
    }
}

$wkNbAttributes = 0;
for (; <fpNames>; $wkNbAttributes++)
{
    # each line must be ended by a NL -- really reasonable of course
    # just for the last line :)
    if (!/\n$/) {
      print_error("*** need a new line character at end of line: $_\n");
    }
    chomp;
    # check for empty lines
    if (/^\s*$/) {
      print_error("*** Empty lines not allowed in names file\n");
    }
    # Check name of attribute
    elsif ( ! m/^([A-Za-z]\w*):(.*)/ )
    {
	print_error("*** Invalid attribute name: $_\n");
    }
    else
    {   
        my $tmp = $_;
	my $wkAttributeName = $1;
	dbg_print("\tchecking attribute $wkAttributeNames[$wkNbAttributes]\n");

	foreach (@wkAttributeNames)
	{
	    print_error("*** Duplicate attribute name: $wkAttributeName\n")
		if ($wkAttributeName eq $_);
	}
	
	$wkAttributeNames[$wkNbAttributes] = $1;

	# Check termination of line
	$_ = $2;
	s/\s//g;
	print_error ("*** attribute declaration must be terminated with a '.': $tmp\n")
	    if (! s/[.]$//);

	# Check possible values and determine mask for value verification
	if ($_ eq "continuous")
	{
	    $wkAttributePatterns[$wkNbAttributes] = "-?[\\d.]+[eE]?-?\\d*|[?]";

	    $wkPositionPrediction = $wkNbAttributes
		if ($opt_regr and $wkPredictionAttribute eq $wkAttributeNames[$wkNbAttributes]);
	    
	    dbg_print("\t$wkAttributeNames[$wkNbAttributes] is continuous\n")
	    }
	else
	{
	    if ($opt_regr and $wkPredictionAttribute eq $wkAttributeNames[$wkNbAttributes])
	    {
		print_error("*** In regression, prediction attribute must be continuous: $_\n");
		
		$wkPositionPrediction = $wkNbAttributes;
	    }
	    
	    $wkAttributePatterns[$wkNbAttributes] = $_;
	    
	    foreach $wkValue (split(/,/, $_))
	    {
		dbg_print("\t\tchecking $wkValue\n");
		if ( ! ($wkValue =~ m/^([A-Za-z]\w*)$/) )
		{
		    print_error("*** Invalid value ($wkValue) declared in attribute $wkAttributeNames[$wkNbAttributes]\n");
		}
		else
		{
		    if ( (! $opt_regr) and ($wkValue =~ m/^($wkClassPattern)$/) )
		    {
			print_error("*** Class value ($wkValue) declared in attribute $wkAttributeNames[$wkNbAttributes]\n");
		    }
		}
	    }
	    
	    dbg_print("\t$wkAttributeNames[$wkNbAttributes] is one of: $_\n");
	    
	    $wkAttributePatterns[$wkNbAttributes] =~ s/,/|/g;
	    $wkAttributePatterns[$wkNbAttributes] = "$wkAttributePatterns[$wkNbAttributes]|[?]";
	}
	
	dbg_print("\tpattern for $wkAttributeNames[$wkNbAttributes]: $wkAttributePatterns[$wkNbAttributes]\n");
    }
}

if (! $opt_regr)
{
    $wkAttributeNames[$wkNbAttributes] = "class";
    $wkAttributePatterns[$wkNbAttributes] = $wkClassPattern;
    dbg_print("\tpattern for $wkAttributeNames[$wkNbAttributes]: $wkAttributePatterns[$wkNbAttributes]\n");
}
else
{
    print_error("*** Prediction attribute ($wkPredictionAttribute) undeclared!\n")
	if (! $wkPositionPrediction);
}

close(fpNames);

# 
# Check values
#

dbg_print("\n >>> CHECKING $opt_f.data\n\n");

open(fpData, "$opt_f.data");
$wkLine = 1;
for ( ; <fpData> ; $wkLine++)
{
    chomp;
    dbg_print("line $wkLine\n");
    if ($opt_max && ($wkLine >= $opt_max)) {
      dbg_print("Maximum number of data record reached, terminating\n");
      exit;
    } 
    $wkAttribute = 0;
    foreach (split(/,/))
    {
	if ( ! m/^($wkAttributePatterns[$wkAttribute])$/ )
	{
	    chomp; # JP: needed to suppress new line in $_ in error message
	    print_error("*** (line $wkLine) invalid value ($_) in attribute $wkAttributeNames[$wkAttribute]\n");
	}
	
	$wkAttribute++;
    }
    print_error("*** (line $wkLine) $wkNbAttributes attributes expected and $wkAttribute found\n")
	if (($opt_regr and ($wkAttribute != $wkNbAttributes)) or ((!$opt_regr) and ($wkAttribute != $wkNbAttributes + 1)));
}
close(fpData);

if (defined($opt_o))
{
    close OUTFILE;
}

### END MAIN

#
# print_error MSG
#

sub print_error()
{   
    $nr_msgs++; 
    ($inMsg) = @_;
    print $inMsg;
    print OUTFILE $inMsg
	if (defined($opt_o));
    if ($opt_limit && ($nr_msgs >= $opt_limit)) {
      print OUTFILE "Error limit $opt_limit reached, terminating\n" if (defined($opt_o));
      dbg_print("Error limit $opt_limit reached, terminating\n");
      exit 1;
    }
}


#
# handle_pars
#

sub handle_pars()
{
GetOptions("max=i", "limit=i", "f=s", "o", "regr", "dbg");

# Files
if (! defined($opt_f))
{
    print "Must provide filestem!\n";
    show_usage();
    exit;
}

if ((! -e "$opt_f.data") or (! -e "$opt_f.names"))
{
    print "$opt_f.data and $opt_f.names must be provided!\n";
    show_usage();
    exit;
}
}

#
# dbg_print MESSAGE
#

sub dbg_print()
{
    my $inMessage = $_[0];
    print $inMessage
	if ($opt_dbg);
}

#
# show_usage
#

sub show_usage()
{
    print "check_database.pl -f FILESTEM [-regr] [-limit MAX_ERRORS] [-max MAX_LINES] [-dbg] [-o] \
        -regr: regression dataset \
        -limit MAX_ERRORS: stop after MAX_ERRORS errors \
        -max MAX_LINES: stop after MAX_LINES lines \
        -o: saves errors in FILESTEM.check_metal \
        -dbg: debug mode\n  \
        Version: $version ($versiondate)\n";
}








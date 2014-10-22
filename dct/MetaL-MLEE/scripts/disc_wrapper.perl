#!/usr/bin/perl
#
$version = '$Revision: 3.0.2.2 $';
$versiondate = '$Date: 2002/08/28 13:12:52 $';
$rcsid = '$Id: disc_wrapper.perl,v 3.0.2.2 2002/08/28 13:12:52 johann Exp $'; 

# disc_wrapper.perl
#
# Rui Pereira
# 07/07/2000
#
# Perl script used as a wrapper for the discretiser program
# It converts the experimental dataset file format to the format used by 
# the Discretiser program, runs the program and then converts the result 
# back to the original format
#
# CHANGES:
# david 2001/08/30  bug correction: problem with too many class labels
# david 2001/08/09  has new option "-n" which causes wrapper to output the  
#                   numerical values of the attributes created by the 
#                   discretizer. If ommited, the values of the attribues
#                   will remain with the "continuous" value. 
# johann 2001/08/08 1.2: suppress trailing dot and balnks in both output
#   data and output test files.
# (carlos 06/10/00) Generates .gdo file that contains information describes
#                   the intervals used and uses it to process .test file.
#                   Joins intervals using the mean. Assumes attributes are
#                   in the same order in files .gdo and .names. Assumes
#                   intervals are in format [LOW,HIGH], in ascending order.
#                   Only processes .test file if it exists.
#



print "\nDiscretiser Wrapper\n";
print "-------------------\n";
print "Rui Pereira, LIACC, July of 2000\n\n";

if (@ARGV<2) {
    print "Perl script used as a wrapper for the discretiser program.\n";
    print "It converts the experimental dataset file format to the format used by\nthe Discretiser program, runs the program and then converts the result\nback to the original format\n\n";
    print "Syntax: $0 InputStem OutputStem [-d] [-c] [-n] [-p \"parameters\"]\n";
    print "\n\n"; 
    print "where:\n";
    print "-d    Delete temporary files used as I/O by the Discretiser program\n";
    print "-c    Convert input files only. Do not call Discretiser\n";
    print "-p    Overrides the default parameters for Discretiser with the supplied ones\n";
    print "      (everything but the file names)\n";
    print "-n    Attribute types defined as list of numbers instead of continuous\n";
    print "Version: $version ($versiondate)\n";
    exit;
}

for($i=2; $i<@ARGV; $i++) {
    if ($ARGV[$i] eq "-d") {
	$d_flag=1;
    }
    elsif ($ARGV[$i] eq "-c") {
	$c_flag=1;
    }
    elsif ($ARGV[$i] eq "-p") {
	$p_flag=1;
	$parameters=$ARGV[$i+1];
    }
    elsif ( $ARGV[$i] eq "-n" ) {
	$n_flag=1;
    }

}


# Input and Output file names produced from the file stems
$names_input_file=$ARGV[0].'.names';
$names_output_file=$ARGV[1].'.names';
$data_input_file=$ARGV[0].'.data';
$data_output_file=$ARGV[1].'.data';
$gdo_file=$ARGV[1].'.gdo';

# Input and Output temp file names produced from the file stems
# These are the files related to the discretiser program
$names_temp_input_file=$ARGV[0].'.names.temp';
$names_temp_output_file=$ARGV[1].'.names.temp';
$data_temp_input_file=$ARGV[0].'.data.temp';
$data_temp_output_file=$ARGV[1].'.data.temp';


# The name of the class
$class='class';


# Opening input files
open(INNAMES, $names_input_file) or (print "Can't open $names_input_file: $!\n" and exit);
open(INDATA,  $data_input_file)  or (print "Can't open $data_input_file: $!\n" and exit);
open(INNAMES_TEMP, ">$names_temp_input_file") or (print "Can't open $names_temp_input_file: $!\n" and exit);
open(INDATA_TEMP,  ">$data_temp_input_file")  or (print "Can't open $data_temp_input_file: $!\n" and exit);




#####################
# Input Domain file


print "Processing the input domain file...\n";

# First line, always use 'class' as the attribute name
$line=<INNAMES>;
chop $line;
@classes=split (/\s*,\s*/ , $line);
print INNAMES_TEMP "dom($class)={";
for($i=0; $i<@classes-1; ++$i) {
    print INNAMES_TEMP "$classes[$i],";
} 
$classes[@classes-1] =~ s/([^\s\.]*)[\s\.]*/\1/;
print INNAMES_TEMP "$classes[@classes-1]};\n";


# There are only two possible situations
# The attrbute is either continuous or has the list of possible discrete values
foreach $line (<INNAMES>) {
    chop $line;

    # If it's not an empty line
    if($line) {
	$line =~ /([^:\s]+)\s*:\s*([^\.]+)/;

	# Saves each attribute name to name the columns in the data file
	push(@col, $1);

	print INNAMES_TEMP "dom($1)=";
	if ($2 eq 'continuous') {
	    print INNAMES_TEMP 'IR;';
	    push(@continuous_attrib,$1);
	}
	else {
	    @d=split(/\s*,\s*/,$2);
	    print INNAMES_TEMP "{";
	    for($i=0; $i<@d-1; ++$i) {
		print INNAMES_TEMP "$d[$i],";
	    }
            $d[@d-1] =~ s/([^\s\.]*)[\s\.]*/\1/;
	    print INNAMES_TEMP "$d[@d-1]};";
	}
	print INNAMES_TEMP "\n";
    }
}


####################
# Data file

print "Processing the data file...\n";
print INDATA_TEMP "@col";
print INDATA_TEMP " $class\n";
foreach $line (<INDATA>) {
    $line =~ tr/,/ /;
    print INDATA_TEMP $line;
}

# First part completed, so close all files
close(INNAMES);
close(INNAMES_TEMP);
close(INDATA);
close(INDATA_TEMP);

print "Converted input domain file written as $names_temp_input_file\n";
print "Converted input data file written as $data_temp_input_file\n";

if($c_flag) {
    print "Convert only selected. Finished.\n";
    exit;
}

# Calls the discretiser program
if ($p_flag) {
    $commandline="discretiser $parameters -g $data_temp_input_file $names_temp_input_file $data_temp_output_file $names_temp_output_file $gdo_file";
}
else {
    $commandline="discretiser fia:class -c fiad_ -g $data_temp_input_file $names_temp_input_file $data_temp_output_file $names_temp_output_file $gdo_file";
}

$commandline=$commandline." @continuous_attrib";


print "Executing Discretiser....\n\n";
print "Using command line:\n";
print "$commandline";
print "\n\n";

if ((system($commandline) >> 8) != 0) {
    print "\n\nDiscretiser program execution failed! Exiting...\n";
    exit;
}


print "\nDiscretiser program execution complete\n\n";


# Opening output files
open(OUTNAMES_TEMP, $names_temp_output_file) or (print "can't open $names_temp_output_file: $!\n" and exit);
open(OUTDATA_TEMP, $data_temp_output_file) or (print "can't open $data_temp_output_file: $!\n" and exit);
open(OUTNAMES, ">$names_output_file") or (print "can't open $names_output_file: $!\n" and exit);
open(OUTDATA, ">$data_output_file") or (print "can't open $data_output_file: $!\n" and exit);

# The temp input file is needed again to check for unknown values 
open(INDATA_TEMP, $data_temp_input_file) or (print "can't open $data_temp_input_file: $!\n" and exit);




#####################
# Output Domain file


print "Processing the domain file\n";
print "Making first sweep...\n";


# First sweep. It searches and stores the discretised lines
# @new is an array where each three elements are class:lowerlimit:upperlimit 
foreach $line (<OUTNAMES_TEMP>) {
    if ($line =~ /dom\(([^\)]*).+IN\s*\[([0-9]+)[^0-9]*([0-9]+).*/) {
	push(@new,$1);
	push(@new,$2);
	push(@new,$3);
    }
}
close OUTNAMES_TEMP;
print "Making second sweep...\n";

# Second sweep. It replaces the file format and substitutes the continuous
# lines by the discretized ones
open(OUTNAMES_TEMP, $names_temp_output_file);

# The index for the @new array
$new_counter=0;

# Counts how many distinct columns there are 
$attrib_counter=0;

# The index for the @swap array
$swap_index=1;

# Ignore the first 4 lines (just junk anyway...)
$line=<OUTNAMES_TEMP>;
$line=<OUTNAMES_TEMP>;
$line=<OUTNAMES_TEMP>;
$line=<OUTNAMES_TEMP>;


# First line corresponds to 'class'

####### Patch by davper and rleite #######

$buf="" ; 
do {
    $line = <OUTNAMES_TEMP> ;
    chomp $line;
    $buf=$buf . $line;
} while ($line !~ /\;/);

$line=$buf;
$line =~ s/ //g;
$line =~ /{(.*)}/;
print OUTNAMES $1.".\n";

###### end patch ######


# In each line found, four situations can occur: continuous (which has 
# been discretized), discretized, an attribute with the 
# corresponding bin definitions and nominal
# In each line found, four situations can occur

# patch rleite {
$buf="";
# } patch rleite    

foreach $line (<OUTNAMES_TEMP>) {
    # patch rleite {
    $line =~ s/\s*//;
    if ($line !~ /^dom/) {
	chomp $line;
	
	if ($line =~ /[;]/) {
	    
	    $buf=$buf . $line;
	    $line=$buf;
	    $buf="";
	} else {
	    
	    $buf=$buf . $line;
	    next;
	}
    }
    
    # } patch rleite    
    
    # Case 1: the line is a continuous attribute that has been discretised
    # Replace the previously continuous with the discretised one
    if ($line =~ /dom\(([^\)]*).*IR.*/) {
	
	# The name of the discretised attribute
	print OUTNAMES "$new[$new_counter]:\t";
	
	######## Patch by David Pereira #########
	if( $n_flag ) {
	    # The sequence of possible values
	    for($i=$new[$new_counter+1]; $i<$new[$new_counter+2]; ++$i) {
		print OUTNAMES $i.", ";
	    }
	    print OUTNAMES $new[$new_counter+2]." .\n";
	}
	else {
	    print OUTNAMES "continuous.\n" ;
	}
	######### End of patch #######
	# Updates the index by 3
	$new_counter=$new_counter+3;
	
	# This counts as an valid attribut
	$attrib_counter++;
	
	# Puts the position of the discretised attribute in @swap
	push(@swap,$swap_index);
	$swap_index++;
    }
    
    # Case 2: the line is one of the new discretised
    # The new attributes are already taken care of, so
    elsif  ($line =~ /dom\(([^\)]*).+=.+/) {
	# Do Nothing
    }
    
    # Case 3: the line was always discrete (no change)
    elsif  ($line =~ /dom\(([^\)]*).+/) {
	print OUTNAMES "$1:\t\t";
	
	# It counts as an attribute
	$attrib_counter++;
	
	# It's not swappable
	push(@swap,0);
    }
    # Case 4: if a line has only discrete symbols, it's the second part
    # of a case 3 line, so just copy it
    elsif ($line =~ /\s*{\s*(.*)\s+}\s*/) {
	print OUTNAMES $1.".\n";
    } 
    
    # If not one of these cases, then it's unexpected
    else {
	print "Unrecognized line!\n".$buf."\n";
    }
    $buf=""; # restart the line buffer
}


print "Processing the data file\n";
$line=<OUTDATA_TEMP>;
$cline=<INDATA_TEMP>;
foreach $line (<OUTDATA_TEMP>) {
    $cline=<INDATA_TEMP>;

    @values=split (/\s+/ , $line);
    @cvalues=split (/\s+/ , $cline);

    for($i=0; $i<$attrib_counter; ++$i) {
	if($swap[$i]!=0) {
	    if($cvalues[$i] eq "?") {
		$values[1+$i]="?";
	    }
	    else {
		$values[1+$i]=$values[$attrib_counter+$swap[$i]];
	    }
	}
    }
    for($i=0; $i<$attrib_counter; ++$i) {
        $mytmp = $values[1+$i];
        $mytmp =~ s/ //g;
	print OUTDATA $mytmp.",";
    }
    $mytmp = $values[0];
    $mytmp =~ s/ //g;
    print OUTDATA "$mytmp\n";
}


# Second part completed, so close all files
close(OUTNAMES);
close(OUTNAMES_TEMP);
close(OUTDATA);
close(OUTDATA_TEMP);

print "Output domain file written as $names_output_file\n";
print "Output data file written as $data_output_file\n";

#
# Discretizing the test file using the bins obtained with the data file,
# which are stored in the .gdo file
# 

# Check if .test exists
$test_input_file=$ARGV[0].'.test';
if (-s $test_input_file)
{
    print "Processing the test file\n";
    $test_output_file=$ARGV[1].'.test';
    
    # Read intervals from the .gdo file for each attribute, joining them
    # using the mean
    # Values outside the limits are placed in the first/last bin, so the
    # lowest/highest limits are dumped 
    open(fpGDO,  $gdo_file) or die "Can't open $gdo_file: $!\n";
    $wkAttribute = 0;
    while (<fpGDO>)
    {
	# Read intervals for current attribute
	$wkHigherLimits[$wkAttribute] = ();
	$wkPreviousHigh = undef;
	while (m/\[([^,]+),([^,]+)\]/g)
	{
	    if (! $wkPreviousHigh)
	    {
		$wkPreviousHigh = $2;
	    }
	    else
	    {
		push(@{$wkHigherLimits[$wkAttribute]}, $wkPreviousHigh + ($1 - $wkPreviousHigh) / 2);
		$wkPreviousHigh = $2;
	    }
	}
	$wkAttribute++;
    }
    close(fpGDO);
    
    # Discretize .test file
    open(INTEST,  $test_input_file)  or (print "Can't open $test_input_file: $!\n" and exit);
    open(OUTTEST, ">$test_output_file") or (print "can't open $test_output_file: $!\n" and exit);
    while (<INTEST>)
    {
	chop;
	@values=split/,/;
	$wkAttribute = 0;
	
	for($i=0; $i<$attrib_counter; ++$i) {
	    if($swap[$i]!=0)
	    {
		if ($values[$i] ne "?") 
		{
		    # Find bin
		    $wkInterval = 0;
		    while ($wkHigherLimits[$wkAttribute]->[$wkInterval] && ($values[$i] > $wkHigherLimits[$wkAttribute]->[$wkInterval]))
		    {
			$wkInterval++;
		    }
		    $values[$i] = $wkInterval;
		}
		$wkAttribute++;
	    }
	}
	for($i=0; $i<$attrib_counter; $i++) {
	    print OUTTEST $values[$i].",";
	}
	print OUTTEST "@values[$attrib_counter]\n";
    }
    close(INTEST);
    close(OUTTEST);

    print "Output test file written as $test_output_file\n";
}
else
{
    print "Test file not found: ignoring it\n";
}

if ($d_flag) {
    system("rm $names_temp_input_file $data_temp_input_file $names_temp_output_file $data_temp_output_file $gdo_file > /dev/null");
    print "Temporary files deleted\n";
}

print "\nProcess complete.\n";








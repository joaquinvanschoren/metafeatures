# config.pm: site-specific settings to be used by all the 
# scripts in this directory. 

# the directory where the program "md5sum" is located or empty if
# it can be found in the binary search path
# (do not add a trailing slash except for the root directory)
$MD5BIN = "";

$CUTBIN = "";

# The default list of learners to run. You can always specify manually
# a list using the -l option (multiple times) for run_exp.
# Uncomment, add, or delete to fit your installation.
@classlearners = (
 # "basedef",
 # "basedef200",
 # "baserand",
 # "c50boost",
 # "c50rules",
 # "c50tree",
 # "lindiscr",
 # "ltree",
 # "mlcnb",
  "c45tree",
  "c45rules",
 # "ripper",
 # "mlcib1",
 # "clemMLP",
 # "clemRBFN"
);

# The default list of regression learners
@regrlearners = (
# "rtplt",
# "cart",
# "cubist",
# "kernel",
# "lr",
# "baggedrt",
# "mars",
# "clemMLP",
# "clemRBFN",
# "svmtorch"
);


# dont change anything below this!

# the suffix of the data characteristics (dc) script that is used by
# default. If the suffix is xxx the full name of the script used
# is run_dc_xxx
$DCPGM = "dct";

# the suffix of the statistics program that is used by default
# if the suffix is xxx the full name of the program should be
# run_stats_xxx
$STATSPGM = "xlsp";


1;

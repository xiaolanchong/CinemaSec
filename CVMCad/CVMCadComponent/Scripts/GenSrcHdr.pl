#
#	
#	Copyright(C) 2004-2005 Yanakov Leonid
#	All rights reserved.
#
#	$Filename: Common.hpp
#	$Author: Yanakov Leonid
#	$Date: 05\03\05
#	$Version: 
#
#	$Purpose: 
#		Generates source and header files with project comments
###############################################################################
use strict;
use POSIX;

my $SRC_DIR = "..\\Sources";
my $HDR_DIR = "..\\Sources";
my $DEF_PROJECT				= "CVMCad";
my $DEF_CR				= "Copyright(C) 2005 Elvees";
my $DEF_VER				= "1.0";
my $DEF_AUTHOR				= "Yanakov Leonid";
my $DEF_ACCESS				= "Public";
my $DEF_HDR_EXT				= "hpp";
my $DEF_CPP_EXT				= "cpp";
my $DEF_DESC				= "This is sample description";

###############################################################################
my $file_header = <<EOD;
/*
*	\$Project 
*	\$Copyrights
*	All rights reserved.
*
*	\$Filename
*	\$Author
*	\$Date
*	\$Version
*	\$Access
*
*	\$Description
*
*
*/
EOD
###############################################################################
my $program_input = $ARGV[0];
my $current_filename = "$program_input.$DEF_HDR_EXT";
my $default_author = $DEF_AUTHOR;
my $current_date = strftime("%Y-%m-%d", localtime);
my $current_version = "$DEF_VER";
my $default_access = $DEF_ACCESS;
my $default_description = $DEF_DESC;
my $include_guard = $current_filename;

$include_guard =~tr/./_/;
$include_guard = uc("__$include_guard\__");


if( length($program_input) == 0 ){ die "Error: Must be at least one parameter\n"; }

open( FH, "+>$HDR_DIR\\$program_input.$DEF_HDR_EXT" )
	or die "Error: Can't create file\n";

$file_header =~ s/\$Project/$DEF_PROJECT/;
$file_header =~ s/\$Copyrights/$DEF_CR/;
$file_header =~ s/\$Filename/\$Filename: $current_filename/;
$file_header =~ s/\$Author/\$Author:   $default_author/;
$file_header =~ s/\$Date/\$Date:     $current_date/;
$file_header =~ s/\$Version/\$Version:  $current_version/;
$file_header =~ s/\$Access/\$Access:   $default_access/;
$file_header =~ s/\$Description/\$Description: $default_description/;

print {*FH} "$file_header";
print {*FH} "#pragma once\n";
print {*FH} "#ifndef $include_guard\n";
print {*FH} "#define $include_guard\n\n\n\n\n";
print {*FH} "#endif //$include_guard\n";
close *FH;


open( FH2, "+>$SRC_DIR\\$program_input.$DEF_CPP_EXT" )
	or die "Error: Can't create file\n";

$file_header =~ s/\$Filename: $program_input.$DEF_HDR_EXT/\$Filename: $program_input.$DEF_CPP_EXT/;

print {*FH2} "$file_header";
print {*FH2} "#include \"$program_input.$DEF_HDR_EXT\"";

close *FH2;





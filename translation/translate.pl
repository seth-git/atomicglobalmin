#!/usr/bin/perl
# Purpose: This file opens a pso output file (.out) and extracts the iteration number
# and energy value from each line, and saves these in graph.csv, which can be
# opened in excel.  To run the program type:
# ./graph.pl someOutputFile.out
# Author: Seth Call
# Note: This is free software and may be modified and/or redistributed under
#    the terms of the GNU General Public License (Version 3).
#    Copyright 2007 Seth Call.

use strict;

my $startLanguage = "en";
my $language;
my $inputPropertiesFile = "input_$startLanguage.properties";
my @lines;
my $line;
my $id;
my @ids;
my %lineParams;

my $arbFile;
my %newLines;
my $outFile;
my %oldIds;

print "Reading: $inputPropertiesFile\n";
open INPUT_FILE, ">", $inputPropertiesFile or die $!;
@lines = <INPUT_FILE>;
chomp(@lines);
close INPUT_FILE;

foreach (@lines)
{
	if (/^input.(\w+)\s*=\s*(\S+(\s+\S+)*)\s*$/) {
		$id = $1;
		$line = $2;
		push(@ids,$id);
		while ($line =~ /(\%(\d)\$\S+)/) {
			$lineParams{$id}{$2} = $1;
			$line =~ s/\%\d\$\S+//;
		}
	}
}

opendir (DIR, ".") or die $!;
while ($arbFile = readdir(DIR)) {
	if ($arbFile !~ /^input_(\w+)\.arb$/ || $1 eq $startLanguage) {
		next;
	}
	$language = $1;
	print "Reading: $arbFile\n";
	open INPUT_FILE, "<", $arbFile or die $!;
	@lines = <INPUT_FILE>;
	chomp(@lines);
	close INPUT_FILE;
	%newLines = ();
	foreach (@lines)
	{
		if (/^\s*"([^"\@])":\s*"(.*)",?\s*$/) {
			$newLines{$1} = $2;
		}
	}
	$outFile = "input_$language.properties";
	%oldIds = ();
	if (open (INPUT_FILE, "<", $outFile)) {
		print "Reading: $outFile\n";
		@lines = <INPUT_FILE>;
		chomp(@lines);
		close INPUT_FILE;
		foreach (@lines) {
			if (/^input.(\w+)\s*=.*$/) {
				$oldIds{$1} = 1;
			}
		}
	}
}
print "Done!\n";


#!/usr/bin/perl
# This file opens a pso output file (.out) and extracts the iteration number
# and energy value from each line, and saves these in graph.csv, which can be
# opened in excel.  To run the program type:
# ./graph.pl someOutputFile.out

use strict;

my @lines;
my $outFile = "graph.csv";

if ($ARGV[0] eq "") {
	print "Please specify a file to extract the energy from.\n";
	exit(0);
}

if ($ARGV[1] ne "") {
	$outFile = $ARGV[1];
}

open (INPUT_FILE, "<$ARGV[0]") or die "Unable to open file: $ARGV[0]";
@lines = <INPUT_FILE>;
chomp(@lines);
close INPUT_FILE;

open (OUT_FILE, ">$outFile") or die "Unable to open file: $outFile";
print OUT_FILE "Iteration,Best Energy\n";
foreach (@lines)
{
	if (/^It: (\d+) Best Energy: (-?\d+\.\d*) /) {
		print OUT_FILE "$1,$2\n";
	}
}
close OUT_FILE;
print "Done!\n";


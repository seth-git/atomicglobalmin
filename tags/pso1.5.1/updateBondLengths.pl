#!/usr/bin/perl
# Purpose: This program updates the bondLenths.txt file.
# Author: Seth Call
# Note: This is free software and may be modified and/or redistributed under
#    the terms of the GNU General Public License (Version 3).
#    Copyright 2007 Seth Call.

use strict;

my $bondLengthsFile = "bondLengths.txt";
my $averageSingleBondFile = "averageSingleBondLengthsInAngstroms.txt";
my $averageMultipleBondFile = "averageMultipleBondLengthsInAngstroms.txt";
my @lines;
my $line;
my @values;
my $key;
my %bondRanges; # A range of distances for various bonds
my %averageBondLengths;
my $bondType;
my $defaultPercentRange = 0.15; # 15 percent
my $lower;
my $upper;

if (open (INPUT_FILE, "<$bondLengthsFile")) { # or die "Unable to open file: $bondLengthsFile";
	@lines = <INPUT_FILE>;
	chomp(@lines);
	close INPUT_FILE;
}

foreach $line (@lines) {
	@values = split(/,/, $line);
	if (@values != 4) {
		next;
	}
	if ($values[0] lt $values[1]) {
		$key = $values[0]."\t".$values[1];
	} else {
		$key = $values[1]."\t".$values[0];
	}
	$bondRanges{$key}{$values[2]} = $values[3];
}

open (INPUT_FILE, "<$averageSingleBondFile") or die "Unable to open file: $averageSingleBondFile";
@lines = <INPUT_FILE>;
chomp(@lines);
close INPUT_FILE;

foreach $line (@lines) {
	@values = split(/,/, $line);
	if ($values[0] lt $values[1]) {
		$key = $values[0]."\t".$values[1];
	} else {
		$key = $values[1]."\t".$values[0];
	}
	$averageBondLengths{$key}{"s"} = $values[2];
}

open (INPUT_FILE, "<$averageMultipleBondFile") or die "Unable to open file: $averageMultipleBondFile";
@lines = <INPUT_FILE>;
chomp(@lines);
close INPUT_FILE;

foreach $line (@lines) {
	@values = split(/,/, $line);
	if ($values[0] lt $values[1]) {
		$key = $values[0]."\t".$values[1];
	} else {
		$key = $values[1]."\t".$values[0];
	}
	$averageBondLengths{$key}{$values[2]} = $values[3];
}

foreach $key (sort keys %averageBondLengths) {
	if (defined $averageBondLengths{$key}{"s"}) {
		$upper = $averageBondLengths{$key}{"s"} + ($averageBondLengths{$key}{"s"} * $defaultPercentRange);
		if (defined $averageBondLengths{$key}{"d"}) {
			$lower = ($averageBondLengths{$key}{"s"}-$averageBondLengths{$key}{"d"})/3 + $averageBondLengths{$key}{"d"};
		} elsif (defined $averageBondLengths{$key}{"t"}) {
			$lower = ($averageBondLengths{$key}{"s"}-$averageBondLengths{$key}{"t"})/3 + $averageBondLengths{$key}{"t"};
		} else {
			$lower = $averageBondLengths{$key}{"s"} - ($averageBondLengths{$key}{"s"} * $defaultPercentRange);
		}
		$bondRanges{$key}{"s"} = "$lower-$upper";
	}
	if (defined $averageBondLengths{$key}{"d"}) {
		if (defined $averageBondLengths{$key}{"s"}) {
			$upper = ($averageBondLengths{$key}{"s"}-$averageBondLengths{$key}{"d"})/3 + $averageBondLengths{$key}{"d"};
		} else {
			$upper = $averageBondLengths{$key}{"d"} + ($averageBondLengths{$key}{"d"} * $defaultPercentRange);
		}
		if (defined $averageBondLengths{$key}{"t"}) {
			$lower = ($averageBondLengths{$key}{"d"}-$averageBondLengths{$key}{"t"})/3 + $averageBondLengths{$key}{"t"};
		} else {
			$lower = $averageBondLengths{$key}{"d"} - ($averageBondLengths{$key}{"d"} * $defaultPercentRange);
		}
		$bondRanges{$key}{"d"} = "$lower-$upper";
	}
	if (defined $averageBondLengths{$key}{"t"}) {
		if (defined $averageBondLengths{$key}{"d"}) {
			$upper = ($averageBondLengths{$key}{"d"}-$averageBondLengths{$key}{"t"})/3 + $averageBondLengths{$key}{"t"};
		} elsif (defined $averageBondLengths{$key}{"t"}) {
			$upper = ($averageBondLengths{$key}{"s"}-$averageBondLengths{$key}{"t"})/3 + $averageBondLengths{$key}{"t"};
		} else {
			$upper = $averageBondLengths{$key}{"t"} + ($averageBondLengths{$key}{"t"} * $defaultPercentRange);
		}
		$lower = $averageBondLengths{$key}{"t"} - ($averageBondLengths{$key}{"t"} * $defaultPercentRange);
		$bondRanges{$key}{"t"} = "$lower-$upper";
	}
}

open (OUT_FILE, ">$bondLengthsFile") or die "Unable to open file: $bondLengthsFile";
foreach $key (sort keys %bondRanges)
{
	foreach $bondType (sort keys %{$bondRanges{$key}}) {
		print OUT_FILE "$key\t$bondType\t$bondRanges{$key}{$bondType}\n";
	}
}

close OUT_FILE;


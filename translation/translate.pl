#!/usr/bin/perl
# Purpose: This program is useful for updating language properties files with google translate. Simply run it for menu options.
# Author: Seth Call
# Note: This is free software and may be modified and/or redistributed under
#    the terms of the GNU General Public License (Version 3).
#    Copyright 2007 Seth Call.

use strict;
use utf8;

print "Options:\n";
print "1) Create an arb file from a properties file for use with google translate.\n";
print "2) Add missing strings in property files from their associated arb files.\n";
print "Please enter a number by one of the options above: ";
my $option = <>;
chomp($option);
if ($option eq 1) {
	createArb();
} elsif ($option eq 2) {
	updateFromARB();
} else {
	print "Please enter 1 or 2.\n";
}

sub createArb
{
	print "Please enter the properties file name (or press enter for English): ";
	my $inputPropertiesFile = <>;
	chomp($inputPropertiesFile);
	if ($inputPropertiesFile eq "") {
		$inputPropertiesFile = "input_en.properties";
	}
	if ($inputPropertiesFile !~ /^input_(\w+).properties$/) {
		die "The properties file must have a name that looks like this: input_code.properties\n";
	}
	my $language = $1;
	if ($language eq "") {
		die "The language code in the file name can't be blank.\n";
	}
	my $arbFile = "input_$language.arb";
	my @lines;
	my $line;
	my $id;

	print "Reading: $inputPropertiesFile\n";
	open INPUT_FILE, "<", $inputPropertiesFile or die $!;
	@lines = <INPUT_FILE>;
	chomp(@lines);
	close INPUT_FILE;

	print "Writing: $arbFile\n";
	open OUT_FILE, ">", $arbFile or die $!;
	print OUT_FILE "arb.register(\"agml\", {\n";
	print OUT_FILE "   \"\@\@locale\": \"en_US\",\n";

	my $firstParam = 1;
	foreach $line (@lines)
	{
		if ($line =~ /^input.(\w+)\s*=\s*(\S+(\s+\S+)*)\s*$/) {
			$id = $1;
			$line = $2;
			$line =~ s/\%(\d+)\$\S*[a-zA-Z]/\{\1\}/g;
			$line =~ s/"/\\"/g;
			if ($firstParam == 1) {
				print OUT_FILE "\n";
				$firstParam = 0;
			} else {
				print OUT_FILE ",\n";
			}
			print OUT_FILE "   \"$id\": \"$line\"";
		} elsif ($line =~ /^input/) {
			die "Unrecognized input on line: $line\n";
		}
	}
	print OUT_FILE "\n";
	print OUT_FILE "});\n";
	print "Done!\n";
	print "Use google translate to create an input_code.arb file from $arbFile, where \"code\" is the two digit language code of your desired language.\n";
	print "Create input_code.arb by uploading $arbFile to http://www.google.com/url?source=transpromo&rs=rsmf&q=http://translate.google.com/toolkit\%3Fhl\%3Den\n";
	print "After logging in and uploading your file, select 'Download' under the 'Share' menu.\n";
	print "Save input_code.arb to this directory, then run this program again and select option 2.\n";
	print "\n";
}

sub updateFromARB 
{
	print "Please enter the name of the original property file you added your strings to (or press enter if you're translating from English): ";
	my $inputPropertiesFile = <>;
	chomp($inputPropertiesFile);
	if ($inputPropertiesFile eq "") {
		$inputPropertiesFile = "input_en.properties";
	}
	if ($inputPropertiesFile !~ /^input_(\w+).properties$/) {
		die "The properties file must have a name that looks like this: input_code.properties\n";
	}
	my $startLanguage = $1;
	if ($startLanguage eq "") {
		die "The language code in the file name can't be blank.\n";
	}
	my $language;
	my @lines;
	my $line;
	my $id;
	my @ids;
	my %lineParams;

	my $arbFile;
	my %newLines;
	my $outFile;
	my %oldLines;
	my $addCount;

	print "Reading: $inputPropertiesFile\n";
	open INPUT_FILE, "<", $inputPropertiesFile or die $!;
	@lines = <INPUT_FILE>;
	chomp(@lines);
	close INPUT_FILE;

	foreach (@lines)
	{
		if (/^input.(\w+)\s*=\s*(\S+(\s+\S+)*)\s*$/) {
			$id = $1;
			$line = $2;
			push(@ids,$id);
			while ($line =~ /(\%(\d+)\$\S*[a-zA-Z])/) {
				$lineParams{$id}{$2} = $1;
				$line =~ s/\%\d+\$\S+//;
			}
		}
	}

	opendir (DIR, ".") or die $!;
	my $fileCount = 0;
	while ($arbFile = readdir(DIR)) {
		if ($arbFile !~ /^input_(\w+)\.arb$/) {
			next;
		}
		if ($1 eq $startLanguage) {
			next;
		}
		++$fileCount;
		$language = $1;
		print "Reading: $arbFile\n";
		open INPUT_FILE, "<", $arbFile or die $!;
		@lines = <INPUT_FILE>;
		chomp(@lines);
		close INPUT_FILE;
		%newLines = ();
		foreach (@lines)
		{
			if (/^\s*"([^"\@]+)":\s*"(.*)",?\s*$/) {
				$id = $1;
				$line = $2;
				my $number;
				while ($line =~ /\{(\d+)\}/) {
					$number = $1;
					$line =~ s/\{(\d+)\}/$lineParams{$id}{$number}/;
				}
				$newLines{$id} = $line;
			}
		}
		$outFile = "input_$language.properties";
		%oldLines = ();
		if (open (INPUT_FILE, "<", $outFile)) {
			print "Reading: $outFile\n";
			@lines = <INPUT_FILE>;
			chomp(@lines);
			close INPUT_FILE;
			foreach (@lines) {
				if (/^input.(\w+)\s*=\s*(\S+(\s+\S+)*)\s*$/) {
					$oldLines{$1} = $2;
				}
			}
		} 
		print "Writing: $outFile\n";
		open OUT_FILE, ">", $outFile or die $!;
		$addCount = 0;
		foreach $id (@ids) {
			print OUT_FILE "input.$id = ";
			if (defined $oldLines{$id}) {
				print OUT_FILE $oldLines{$id};
			} else {
				print OUT_FILE $newLines{$id};
				++$addCount;
			}
			print OUT_FILE "\n";
		}
		close(OUT_FILE);
		print "Added $addCount strings to $outFile.\n";
	}
	if ($fileCount == 0) {
		print "No files were updated. You must first create an arb file using option 1.\n\n";
	} else {
		print "Done!\n\n";
	}
}


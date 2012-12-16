#!/usr/bin/perl
# Purpose: This program provides utilities for setting up xsd parsing and
#    moving strings to input_en.properties.  When adding element or attribute
#    readers, this program needs a load method to be declared inside the .cc
#    file.
# Author: Seth Call
# Note: This is free software and may be modified and/or redistributed under
#    the terms of the GNU General Public License (Version 3).
#    Copyright 2007 Seth Call.

use strict;
use utf8;
use Storable qw(dclone);

my $stringsHeaderFile = '../translation/strings.h';
my $stringsCCFile = '../translation/strings.cc';
my $stringsPropertiesFile = '../translation/input_en.properties';

print "Options:\n";
print "1. Add element reader code to a C++ class and related files.\n";
print "2. Add attribute reader code to a C++ class and related files.\n";
print "3. Remove hard coded strings from print statements in a C++ class.\n";
print "Please enter a number by one of the options above: ";
my $option = <>;
chomp($option);
if ($option == 1) {
	addElementReader();
} elsif ($option == 2) {
	addAttributeReader();
} elsif ($option == 3) {
	addStringsFromCCFile();
} else {
	print "Unrecognized option.\n";
}

sub trim {
	foreach (@_) {
		s/^\s+//;
		s/\s+$//;
	}
}

sub addElementReader {
	print "What's the parent element name: ";
	my $parentElem = <>;
	chomp($parentElem);
	print "How are these child elements arranged in the parent element? Please select one of the following:\n";
	print "1. Use the All xsd indicator.\n";
	print "2. Use the Choice xsd indicator.\n";
	print "3. Use the Sequence xsd indicator.\n";
	my $indicator = <>;
	chomp($indicator);
	if ($indicator < 1 || $indicator > 3) {
		print "Unrecogniged indicator.\n";
		return;
	}
	print "What .cc file will these go in? ";
	my $ccFile = <>;
	chomp($ccFile);
	my $hFile = $ccFile;
	$hFile =~ s/.cc/.h/;
	print "Please enter the child element names with commas in between: ";
	my $names = <>;
	chomp($names);
	my @names = split(/,/, $names,-1);
	trim(@names);
	
	my $parElemVar = 'p'.ucfirst($parentElem).'Elem';
	my $parElemHandle = 'h'.ucfirst($parentElem);
	my @ufNames = map(ucfirst, @names);
	my @nameVars;
	foreach (@ufNames) {
		push(@nameVars, "m_sx$_");
	}
	my @nameStrs;
	foreach (@nameVars) {
		push(@nameStrs, "messages->$_.c_str()");
	}
	
	my $minOccurs;
	my @minOccurs;
	my $maxOccurs;
	my @maxOccurs;
	
	my %operations;
	
	my $pattern;
	my $replacement;
	my $text;
	$" = ', ';
	if ($indicator == 1) {
		print "Please enter the minOccurs indicators (0 or 1) for each of these elements with commas in between: ";
		$minOccurs = <>;
		chomp($minOccurs);
		
		$text = '\1\tstatic const unsigned int s_'.$parentElem."MinOccurs[];\n";
		$pattern = "(\t*)private:\n";
		$replacement = '\1private:\n'.$text;
		push(@{$operations{$hFile}}, [$pattern, $replacement, 0]);
		
		$text = 'const unsigned int \2::s_'.$parentElem."MinOccurs[] = {$minOccurs};\n";
		$pattern = '(bool (\w+)::load)';
		$replacement = $text.'\n\1';
		push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
		
		$text = "\tTiXmlHandle $parElemHandle(0);\n".
		        "\t$parElemHandle=TiXmlHandle($parElemVar);\n".
		        "\tconst char* ".$parentElem."ElemNames[] = {@nameStrs};\n".
		        "\tXsdElementUtil ".$parentElem."ElemUtil($parElemVar->Value(), XSD_ALL, ".$parentElem."ElemNames, s_".$parentElem."MinOccurs);\n".
		        "\tif (!".$parentElem."ElemUtil.process($parElemHandle)) {\n".
		        "\t\treturn false;\n".
		        "\t}\n".
		        "\tTiXmlElement** ".$parentElem."Elements = ".$parentElem."ElemUtil.getAllElements();\n\n";
		$pattern = '(bool \w+::load\w*\s*\([^\)]+\)\s*{\n)';
		$replacement = '\1'.$text;
		push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
	} elsif ($indicator == 2) {
		$text = "\tTiXmlHandle $parElemHandle(0);\n".
		        "\t$parElemHandle=TiXmlHandle($parElemVar);\n".
		        "\tconst char* ".$parentElem."ElemNames[] = {@nameStrs};\n".
		        "\tXsdElementUtil ".$parentElem."ElemUtil($parElemVar->Value(), XSD_CHOICE, ".$parentElem."ElemNames);\n".
		        "\tif (!".$parentElem."ElemUtil.process($parElemHandle)) {\n".
		        "\t\treturn false;\n".
		        "\t}\n".
		        "\tunsigned int ".$parentElem."ChoiceIndex = ".$parentElem."ElemUtil.getChoiceElementIndex();\n".
		        "\tTiXmlElement* ".$parentElem."ChoiceElem = ".$parentElem."ElemUtil.getChoiceElement();\n\n";
		$pattern = '(bool \w+::load\w*\s*\([^\)]+\)\s*{\n)';
		$replacement = '\1'.$text;
		push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
	} elsif ($indicator == 3) {
		print "Please enter the minOccurs indicators (0, 1, 2...) for each of these elements with commas in between: ";
		$minOccurs = <>;
		chomp($minOccurs);
		print "Please enter the maxOccurs indicators (1, 2, 3... or 'unlimited') for each of these elements with commas in between: ";
		$maxOccurs = <>;
		chomp($maxOccurs);
		$maxOccurs =~ s/unlimited/XSD_UNLIMITED/g;
		$text = '\1\tstatic const unsigned int s_'.$parentElem."MinOccurs[];\n".
			'\1\tstatic const unsigned int s_'.$parentElem."MaxOccurs[];\n";
		$pattern = "(\t*)private:\n";
		$replacement = '\1private:\n'.$text;
		push(@{$operations{$hFile}}, [$pattern, $replacement, 0]);

		$text = 'const unsigned int \2::s_'.$parentElem."MinOccurs[] = {$minOccurs};\n".
		        'const unsigned int \2::s_'.$parentElem."MaxOccurs[] = {$maxOccurs};\n";
		$pattern = '(bool (\w+)::load)';
		$replacement = $text.'\n\1';
		push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
		
		$text = "\tTiXmlHandle $parElemHandle(0);\n".
		        "\t$parElemHandle=TiXmlHandle($parElemVar);\n".
		        "\tconst char* ".$parentElem."ElemNames[] = {@nameStrs};\n".
		        "\tXsdElementUtil ".$parentElem."ElemUtil($parElemVar->Value(), XSD_SEQUENCE, ".$parentElem."ElemNames, s_".$parentElem."MinOccurs, s_".$parentElem."MaxOccurs);\n".
		        "\tif (!".$parentElem."ElemUtil.process($parElemHandle)) {\n".
		        "\t\treturn false;\n".
		        "\t}\n".
		        "\tstd::vector<TiXmlElement*>* ".$parentElem."Elements = ".$parentElem."ElemUtil.getSequenceElements();\n\n";
		$pattern = '(bool \w+::load\w*\s*\([^\)]+\)\s*{\n)';
		$replacement = '\1'.$text;
		push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
	}
	
	my $stringsRef = getStrings();
	addStrings("x", \@names, \@names, \%operations, $stringsRef);
	
	doOperations(\%operations);
	print "Done!\n";
}

sub addAttributeReader {
	print "What's the parent element name: ";
	my $parentElem = <>;
	chomp($parentElem);
	print "What .cc file will this go in? ";
	my $ccFile = <>;
	chomp($ccFile);
	my $hFile = $ccFile;
	$hFile =~ s/.cc/.h/;
	print "Please enter the attribute names with commas in between: ";
	my $names = <>;
	chomp($names);
	my @names = split(/,/, $names,-1);
	trim(@names);
	
	print "Please enter the required status of each attribute.  Enter true if required and false otherwise.  Seperate values with commas: ";
	my $required = <>;
	chomp($required);
	
	print "Please enter the default value of each attribute.  Seperate values with commas, using blank values when there is no default: ";
	my $defaults = <>;
	chomp($defaults);
	my @defaults = split(/,/, $defaults,-1);
	trim(@defaults);
	
	my $parElemVar = 'p'.ucfirst($parentElem).'Elem';
	my @ufNames = map(ucfirst, @names);
	my @nameVars;
	foreach (@ufNames) {
		push(@nameVars, "m_sx$_");
	}
	my @nameStrs;
	foreach (@nameVars) {
		push(@nameStrs, "messages->$_.c_str()");
	}
	my @phraseStrings;
	my @defaultStrs;
	foreach (@defaults) {
		if (/[a-zA-Z]/) {
			push(@defaultStrs, "messages->m_sp".ucfirst($_).".c_str()");
			push(@phraseStrings, $_);
		} else {
			push(@defaultStrs, "\"$_\"");
		}
	}
	
	my %operations;
	
	my $pattern;
	my $replacement;
	my $text;
	my $defalutArrayNm;
	$" = ', ';
	
	$text = '\1\tstatic const bool s_'.$parentElem."AttReq[];\n";
	if (@phraseStrings == 0) {
		$defalutArrayNm = "s_$parentElem"."AttDef";
		$text .= '\1\tstatic const char* '.$defalutArrayNm."[];\n";
	}
	$pattern = "(\t*)private:\n";
	$replacement = '\1private:\n'.$text;
	push(@{$operations{$hFile}}, [$pattern, $replacement, 0]);
	
	$text = 'const bool \2::s_'.$parentElem."AttReq[] = {$required};\n";
	if (@phraseStrings == 0) {
		$text .= 'const char* \2::'.$defalutArrayNm."[] = {@defaultStrs};\n";
	}
	$pattern = '(bool (\w+)::load)';
	$replacement = $text.'\n\1';
	push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
	
	$text = "\tconst char* ".$parentElem."AttNames[] = {@nameStrs};\n";
	if (@phraseStrings > 0) {
		$defalutArrayNm = "$parentElem"."AttDef";
		$text .= "\tconst char* ".$defalutArrayNm."[] = {@defaultStrs};\n";
	}
	$text .= "\tXsdAttributeUtil ".$parentElem."AttUtil($parElemVar->Value(), ".$parentElem."AttNames, s_".$parentElem."AttReq, $defalutArrayNm);\n".
	         "\tif (!".$parentElem."AttUtil.process($parElemVar)) {\n".
	         "\t\treturn false;\n".
	         "\t}\n".
	         "\tconst char** ".$parentElem."AttValues = ".$parentElem."AttUtil.getAllAttributes();\n\n";
	$pattern = '(bool \w+::load\w*\s*\([^\)]+\)\s*{\n)';
	$replacement = '\1'.$text;
	push(@{$operations{$ccFile}}, [$pattern, $replacement, 0]);
	
	my $stringsRef = getStrings();
	addStrings("x", \@names, \@names, \%operations, $stringsRef);
	
	if (@phraseStrings > 0) {
		addStrings("p", \@phraseStrings, \@phraseStrings, \%operations, $stringsRef);
	}
	
	doOperations(\%operations);
	print "Done!\n";
}

sub addStringsFromCCFile {
	print "What .cc file are the print statements in? ";
	my $ccFile = <>;
	chomp($ccFile);
	my $lines;
	my %operations;
	my %operationsHash;
	my $pattern;
	my $replacement;
	
	readFile(\$ccFile, \$lines);
	my $stringsRef = getStrings();
	my %stringsOriginal = %{ dclone ($stringsRef) };
	my %valueNameHash;
	my $type;
	my $name;
	my $value;
	my @names;
	my @values;
	while (($name, $value) = each(%{$$stringsRef{"e"}})){
		$valueNameHash{$value} = $name;
	}
	my $count = 0;
	while ($lines =~ /printf\s*\("(([^\\"]|\\.)*?[^\\])"/g) {
		$value = $1;
		trim($value);
		print("Here is a string value: \"$value\".\n");
		if (defined $valueNameHash{$value}) {
			$name = $valueNameHash{$value};
			print "Using the already associated string name: $name\n";
		} else {
			while (1) {
				print("Please enter a name (or leave blank to skip): ");
				$name = <>;
				$name = lcfirst($name);
				trim($name);
				if (defined $$stringsRef{"e"}{$name}) {
					print ("There is already a string with the same name, but a different value: \"". $$stringsRef{"e"}{$name} ."\n");
					next;
				} elsif (length($name) > 0) {
					$value =~ s/([\^\$\.\*\+\?\|\(\)\[\]\{\}\\])/\\\1/g; # Escape special characters
					push(@names, $name);
					push(@values, $value);
				} else {
					print "Skipping...\n";
				}
				last;
			}
		}
		if (length($name) == 0) {
			next;
		}
		if (!defined $operationsHash{$value}) {
			$pattern = 'printf\s*\("'.$value.'"';
			$replacement = 'printf(messages->m_s'.ucfirst($name).'.c_str()';
			push(@{$operations{$ccFile}}, [$pattern, $replacement, 1]);
			$$stringsRef{"e"}{$name} = $value;
			$valueNameHash{$value} = $name;
			$operationsHash{$value} = 1;
			++$count;
		}
	}
	if ($count == 0) {
		print "No replacements made in $ccFile.\n";
	} else {
		addStrings("e", \@names, \@values, \%operations, \%stringsOriginal);
		doOperations(\%operations);
		print "Done!\n";
	}
}

sub getStrings {
	print "Reading: $stringsPropertiesFile\n";
	open INPUT_FILE, "<", $stringsPropertiesFile or die $!;
	my @lines = <INPUT_FILE>;
	chomp(@lines);
	close INPUT_FILE;
	
	my $stringsRef = {};
	my %values;
	my $line;
	my $type;
	my $name;
	my $value;
	foreach $line (@lines)
	{
		if ($line =~ /^input.(p|x)?([A-Z]\w*)\s*=\s*(\S+(\s+\S+)*)\s*$/) {
			$type = $1;
			if (length($type) == 0) {
				$type = "e";
			} elsif ($type ne "x" && $type ne "p") {
				die "Unknown string type found in $stringsPropertiesFile: $type\n";
			}
			$name = lcfirst($2);
			$value = $3;
			if (defined $values{$type} && defined $values{$type}{$value}) {
				print "Found two strings with a different name but the same value.  Please correct this.\n";
				print "Value: $value, names: $name, $values{$type}{$value}\n.";
				exit(0);
			}
			if (defined $$stringsRef{$type} && defined $$stringsRef{$type}{$name}) {
				print "Found two strings with a different value but the same name.  Please correct this.\n";
				print "Name: $name, values: $value, $$stringsRef{$type}{$name}\n.";
				exit(0);
			}
			$$stringsRef{$type}{$name} = $value;
			$values{$type}{$value} = $name;
		} elsif ($line =~ /^input/) {
			die "Unrecognized input on line in $stringsPropertiesFile: $line\n";
		}
	}
	return $stringsRef;
}

sub addStrings {
	my $type = shift;
	my $namesRef = shift;
	my $valuesRef = shift;
	my $operationsHashRef = shift;
	my $stringsRef = shift;
	
	my $ufName;
	my $usedType;
	if ($type eq "e") {
		$usedType = "";
	} else {
		$usedType = $type;
	}
	
	my $name;
	my $value;
	my $text;
	my $pattern;
	my $replacement;
	
	$text = "";
	foreach $name (@{$namesRef}) {
		$ufName = ucfirst($name);
		
		if ($$stringsRef{$type}{$name}) {
			next;
		} else {
			$text .= "\tstd::string m_s$usedType$ufName;\n";
		}
	}
	if ($text ne "") {
		if ($type eq "x") {
			$pattern = '(\tstd::string m_sp)';
		} elsif ($type eq "p") {
			$pattern = '(\tstd::string m_s[^px])';
		} elsif ($type eq "e") {
			$pattern = '(\t?private:)';
		}
		$replacement = $text.'\n\1';
		push(@{$$operationsHashRef{$stringsHeaderFile}}, [$pattern, $replacement, 0]);
	}
	
	$text = "";
	foreach $name (@{$namesRef}) {
		$ufName = ucfirst($name);
		
		if ($$stringsRef{$type}{$name}) {
			next;
		} else {
			$text .= "\tif (!copy(stringMap, \"$usedType$ufName\", m_s$usedType$ufName))\n\t\treturn false;\n";
		}
	}
	if ($text ne "") {
		if ($type eq "x") {
			$pattern = '(\tif \(!copy\(stringMap, \"p)';
		} elsif ($type eq "p") {
			$pattern = '(\tif \(!copy\(stringMap, \"[^px])';
		} elsif ($type eq "e") {
			$pattern = '(\t?m_bLoaded\s+=\s+true;)';
		}
		$replacement = $text.'\n\1';
		push(@{$$operationsHashRef{$stringsCCFile}}, [$pattern, $replacement, 0]);
	}
	
	$text = "";
	for (my $i = 0; $i < @{$namesRef}; ++$i) {
		$name = $$namesRef[$i];
		$value = $$valuesRef[$i];
		$ufName = ucfirst($name);
		
		if ($$stringsRef{$type}{$name}) {
			next;
		} else {
			$text .= "input.$usedType$ufName = $value\n";
		}
	}
	if ($text ne "") {
		if ($type eq "x") {
			$pattern = '(input\.p)';
			$replacement = $text.'\n\1';
		} elsif ($type eq "p") {
			$pattern = '(input\.[^px])';
			$replacement = $text.'\n\1';
		} elsif ($type eq "e") {
			$pattern = '(.|\s)$';
			$replacement = '\1'.$text;
		}
		push(@{$$operationsHashRef{$stringsPropertiesFile}}, [$pattern, $replacement, 0]);
	}
}

sub doOperations {
	my $operationsHashRef = shift;
	my $fileOperations;
	my $i;
	my $lines;
	my $file;
	my %fileLines;
	foreach $file (sort keys %$operationsHashRef) {
		$fileOperations = $$operationsHashRef{$file};
		readFile(\$file,\$lines);
		for ($i = 0; $i < @$fileOperations; ++$i) {
			if (!patternInLines(\$lines,\$fileOperations->[$i][0])) {
				print "Error: expected to find this pattern: ".$fileOperations->[$i][0]." in this file: $file\n";
				return;
			}
		}
		$fileLines{$file} = $lines;
	}
	foreach $file (sort keys %$operationsHashRef) {
		$fileOperations = $$operationsHashRef{$file};
		$lines = \$fileLines{$file};
		for ($i = 0; $i < @$fileOperations; ++$i) {
			if ($fileOperations->[$i][2]) {
				replaceAll($lines , \$fileOperations->[$i][0], \$fileOperations->[$i][1]);
			} else {
				replaceFirst($lines , \$fileOperations->[$i][0], \$fileOperations->[$i][1]);
			}
		}
		writeFile(\$file,$lines);
	}
}

sub readFile {
	my $file_ref = shift;
	my $lines_ref = shift;

	print "Reading file: '$$file_ref'\n";
	open INPUT_FILE, "<", $$file_ref or die $!;
	$$lines_ref = join("", <INPUT_FILE>);
	close INPUT_FILE;
}

sub writeFile {
	my $file_ref = shift;
	my $lines_ref = shift;

	print "Writing file: '$$file_ref'\n";
	open OUT_FILE, ">", $$file_ref or die $!;
	print OUT_FILE $$lines_ref;
	close OUT_FILE;
}

sub patternInLines {
	my $lines_ref = shift;
	my $pattern_ref = shift;

	if (!eval('$$lines_ref =~ /'.$$pattern_ref.'/')) {
		return 0;
	}
	return 1;
}

sub replaceFirst {
	my $lines_ref = shift;
	my $pattern_ref = shift;
	my $replacement_ref = shift;

	eval('$$lines_ref =~ s/'.$$pattern_ref.'/'.$$replacement_ref.'/');
}

sub replaceAll {
	my $lines_ref = shift;
	my $pattern_ref = shift;
	my $replacement_ref = shift;

	eval('$$lines_ref =~ s/'.$$pattern_ref.'/'.$$replacement_ref.'/g');
}

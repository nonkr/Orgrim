#!/usr/bin/env perl
use strict;
use warnings;
use FindBin qw($Bin);
use File::Basename;
use Data::Dumper;

my $PROJECT = basename($Bin);

# add directories where header files exist
my @INCLUDES = ();

my @EXCLUDES = (
    "cmake-build-debug",
);
my $EXCLUDES_STR = '';
if (scalar(@EXCLUDES) != 0)
{
    $EXCLUDES_STR = join(" -o ", map {qq(-path "./$_")} @EXCLUDES);
    $EXCLUDES_STR = qq(\\( $EXCLUDES_STR \\) -prune -o)
}

# add reg which you want to exclude
my @src_files = `find . $EXCLUDES_STR -name "*.c" -print -o -name "*.cpp" -print -o -name "*.cc" -print -o -name "*.h" -print -o -name "*.hpp" -print -o -name "Makefile*" -print`;

open(my $fh, ">", "CMakeLists.txt");

print $fh qq(cmake_minimum_required(VERSION 3.3.2)\n);
print $fh qq(project($PROJECT)\n);
print $fh qq(set\(CMAKE_CXX_FLAGS "\${CMAKE_CXX_FLAGS} -std=c++11"\)\n\n);

print $fh qq(set\(SOURCE_FILES\n);
foreach my $src_file (@src_files)
{
    chomp($src_file);
    next if ($src_file =~ /\.svn/ or $src_file =~ /CMakeFiles/);
    $src_file =~ s!./!!;
    next if($src_file =~ /^dist/);
    print $fh qq(    $src_file\n);
}
print $fh qq(\)\n\n);

foreach my $include_path (@INCLUDES)
{
    print $fh qq(include_directories($include_path)\n);
}

print $fh qq(\nadd_executable($PROJECT \${SOURCE_FILES})\n);

close($fh);

#open(my $fh2, ">", "main.c");
#printf $fh2 "int main() { return 0; }\n";
#close($fh2);

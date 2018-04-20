#!/usr/bin/perl


# This Perl script generates a CMakeLists.txt project file for an Oaknut project.
#
# There's absolutely no need to build Oaknut projects with CMake, this exists to
# support the CLion IDE.



use warnings;
use strict;
use File::Basename;
use Getopt::Long;

my $projectname="app";
my @frameworks=();
GetOptions ("projectname=s" => \$projectname,
			"framework=s" => \@frameworks);

my $proj_ios=$projectname."_ios";
my $proj_macos=$projectname."_macos";
my $proj_android=$projectname."_android";
my $proj_web=$projectname."_web";

mkdir '.idea' unless -d '.idea';
my $filename = '.idea/workspace.xml';
open(my $fh, '>', $filename) or die "Could not open file '$filename' $!";
print $fh qq(<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
<component name="CMakeRunConfigurationManager" shouldGenerate="true" shouldDeleteObsolete="true" buildAllGenerated="true">
<generated>);
print $fh "<config projectName=\"$projectname\" targetName=\"".$proj_ios."\" />";
print $fh "<config projectName=\"$projectname\" targetName=\"".$proj_macos."\" />";
print $fh "<config projectName=\"$projectname\" targetName=\"".$proj_android."\" />";
print $fh "<config projectName=\"$projectname\" targetName=\"".$proj_web."\" />";
print $fh "<config projectName=\"$projectname\" targetName=\"".$projectname."_do_not_build\" />";
print $fh qq(
</generated>
</component>
<component name="CMakeSettings">
<configurations>
<configuration PROFILE_NAME="Debug" CONFIG_NAME="Debug" GENERATION_DIR=".build/cmake-debug">
<ADDITIONAL_GENERATION_ENVIRONMENT>
<envs>
<env name="CONFIG" value="debug" />
</envs>
</ADDITIONAL_GENERATION_ENVIRONMENT>
</configuration>
<configuration PROFILE_NAME="Release" CONFIG_NAME="Release" GENERATION_DIR=".build/cmake-release">
<ADDITIONAL_GENERATION_ENVIRONMENT>
<envs>
<env name="CONFIG" value="release" />
</envs>
</ADDITIONAL_GENERATION_ENVIRONMENT>
</configuration>
</configurations>
</component>
<component name="RunManager">
<configuration name="$proj_macos" type="CMakeRunConfiguration" factoryName="Application" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_macos" CONFIG_NAME="Debug" RUN_PATH="\$PROJECT_DIR\$/.build/macos/debug/$projectname.app/Contents/MacOS/$projectname">
<envs />
</configuration>
<configuration name="$proj_macos" type="CMakeRunConfiguration" factoryName="Application" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_macos" CONFIG_NAME="Release" RUN_PATH="\$PROJECT_DIR\$/.build/macos/debug/$projectname.app/Contents/MacOS/$projectname">
<envs />
</configuration>
<configuration name="$proj_web" type="CMakeRunConfiguration" factoryName="Application" PROGRAM_PARAMS="$ENV{'EMSCRIPTEN_ROOT'}/emrun.py xx.html" WORKING_DIR="file://\$PROJECT_DIR\$/.build/web/debug/webroot" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_web" CONFIG_NAME="Debug" RUN_PATH="/usr/bin/python">
<envs />
</configuration>
<configuration name="$proj_web" type="CMakeRunConfiguration" factoryName="Application" PROGRAM_PARAMS="$ENV{'EMSCRIPTEN_ROOT'}/emrun.py xx.html" WORKING_DIR="file://\$PROJECT_DIR\$/.build/web/release/webroot" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_web" CONFIG_NAME="Release" RUN_PATH="/usr/bin/python">
<envs />
</configuration>

</component>
</project>);
close $fh;


print qq(cmake_minimum_required(VERSION 3.9)
project($projectname)

if (NOT IS_DIRECTORY \$ENV{OAKNUT_DIR})
message(FATAL_ERROR "Environment variable OAKNUT_DIR must point to the Oaknut repo")
endif()

include(\$ENV{OAKNUT_DIR}/CMakeLists.txt)

);

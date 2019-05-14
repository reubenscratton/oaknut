#!/usr/bin/perl


# This Perl script generates a CMakeLists.txt project file for an Oaknut project.
#
# There's no need to build Oaknut projects with CMake, this exists to
# support the CLion IDE.


use warnings;
use strict;
use File::Basename;
use File::Path;
use Getopt::Long;

my $projectname="app";
my @frameworks=();
GetOptions ("projectname=s" => \$projectname,
			"framework=s" => \@frameworks);

my $proj_ios=$projectname."_ios";
my $proj_macos=$projectname."_macos";
my $proj_android=$projectname."_android";
my $proj_web_asmjs=$projectname."_web_asmjs";
my $proj_web_wasm=$projectname."_web_wasm";

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
print $fh "<config projectName=\"$projectname\" targetName=\"".$proj_web_asmjs."\" />";
print $fh "<config projectName=\"$projectname\" targetName=\"".$proj_web_wasm."\" />";
#print $fh "<config projectName=\"$projectname\" targetName=\"".$projectname."_do_not_build\" />";
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
<configuration name="$proj_web_asmjs" type="CMakeRunConfiguration" factoryName="Application" PROGRAM_PARAMS="$ENV{'EMSCRIPTEN_ROOT'}/emrun.py xx.html" WORKING_DIR="file://\$PROJECT_DIR\$/.build/web_asmjs/debug/webroot" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_web_asmjs" CONFIG_NAME="Debug" RUN_PATH="/usr/bin/python">
<envs />
</configuration>
<configuration name="$proj_web_asmjs" type="CMakeRunConfiguration" factoryName="Application" PROGRAM_PARAMS="$ENV{'EMSCRIPTEN_ROOT'}/emrun.py xx.html" WORKING_DIR="file://\$PROJECT_DIR\$/.build/web_asmjs/release/webroot" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_web_asmjs" CONFIG_NAME="Release" RUN_PATH="/usr/bin/python">
<envs />
</configuration>
<configuration name="$proj_web_wasm" type="CMakeRunConfiguration" factoryName="Application" PROGRAM_PARAMS="$ENV{'EMSCRIPTEN_ROOT'}/emrun.py xx.html" WORKING_DIR="file://\$PROJECT_DIR\$/.build/web_wasm/debug/webroot" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_web_wasm" CONFIG_NAME="Debug" RUN_PATH="/usr/bin/python">
<envs />
</configuration>
<configuration name="$proj_web_wasm" type="CMakeRunConfiguration" factoryName="Application" PROGRAM_PARAMS="$ENV{'EMSCRIPTEN_ROOT'}/emrun.py xx.html" WORKING_DIR="file://\$PROJECT_DIR\$/.build/web_wasm/release/webroot" PASS_PARENT_ENVS_2="true" PROJECT_NAME="$projectname" TARGET_NAME="$proj_web_wasm" CONFIG_NAME="Release" RUN_PATH="/usr/bin/python">
<envs />
</configuration>

</component>
</project>);
close $fh;

# Clean
sub gen_clean_file {
	my ($platform, $config) = @_;
	my $cmdir = '.build/cmake-'.$config.'/CMakeFiles/'.$projectname.'_'.$platform.'.dir';
	mkpath($cmdir,0,0755);
	open(my $fh, '>', $cmdir.'/cmake_clean_CXX.cmake') or die $!;
	print $fh 'file(REMOVE_RECURSE  "../../.build/'.$platform.'/'.$config.'")';
	close $fh;
}
sub gen_clean_files {
		gen_clean_file(@_, 'debug');
		gen_clean_file(@_, 'release');
}
gen_clean_files('macos');
gen_clean_files('ios');
gen_clean_files('android');
gen_clean_files('web_asmjs');
gen_clean_files('web_wasm');

# CMakeLists.txt
open($fh, '>', 'CMakeLists.txt');
print $fh qq(cmake_minimum_required(VERSION 3.9)
project($projectname)

if (NOT IS_DIRECTORY \$ENV{OAKNUT_DIR})
message(FATAL_ERROR "Environment variable OAKNUT_DIR must point to the Oaknut repo")
endif()

include(\$ENV{OAKNUT_DIR}/build/CMakeLists.txt)
);
close $fh;

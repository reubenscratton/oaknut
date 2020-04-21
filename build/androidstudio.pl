#!/usr/bin/perl

# This Perl script generates an Android Studio project folder for an Oaknut
# project. The generated project file is of course Android-only.
#
# Developed and tested with Android Studio 3.2 on OS X.


use warnings;
use strict;
use File::Basename;
use Getopt::Long;

my $projectname="app";
my $android_ver="19";
my $package_name="org.example.app";

GetOptions ("projectname=s" => \$projectname,
						"package_name=s" => \$package_name,
					  "android_ver=s" => \$android_ver);

# Need Android Studio location
my $sdk_dir=$ENV{'ANDROID_SDK_DIR'};
if (!$sdk_dir) {
	$sdk_dir="~/Library/Android/sdk";
}


# .gitignore (Project)
my $str = <<END;
*.iml
.gradle
/local.properties
/.idea/caches/build_file_checksums.ser
/.idea/libraries
/.idea/modules.xml
/.idea/workspace.xml
.DS_Store
/build
/captures
.externalNativeBuild
END
open(FH, '>', "$projectname.androidstudio/.gitignore") or die $!;
print FH $str;
close FH;

# build.gradle (Project)
$str = <<END;
buildscript {
    repositories {
        google()
        jcenter()
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:3.5.3'
    }
}

allprojects {
    repositories {
        google()
        jcenter()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
END
open(FH, '>', "$projectname.androidstudio/build.gradle") or die $!;
print FH $str;
close FH;

# local.properties
#open(FH, '>', "$projectname.androidstudio/local.properties") or die $!;
#print FH "ndk.dir=$sdk_dir/ndk-bundle\n";
#print FH "sdk.dir=$sdk_dir\n";
#close FH;

# settings.gradle
open(FH, '>', "$projectname.androidstudio/settings.gradle") or die $!;
print FH "include ':app'\n";
close FH;

# .gitignore (Module)
open(FH, '>', "$projectname.androidstudio/app/.gitignore") or die $!;
print FH "/build\n";
close FH;

# build.gradle (Module)
$str = <<END;
apply plugin: 'com.android.application'

android {
    compileSdkVersion $android_ver
    defaultConfig {
        applicationId "$package_name"
        minSdkVersion 19
        targetSdkVersion $android_ver
        versionCode 1
        versionName "1.0"
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++11"
            }
        }
    }
    buildTypes {
        release {
            minifyEnabled false
        }
    }
    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
    sourceSets {
        main {
            java.srcDirs = [System.getenv('OAKNUT_DIR') + '/src/platform/android/java']
            res.srcDirs = ['../../platform/android/res']
            manifest.srcFile '../../platform/android/AndroidManifest.xml'
            assets.srcDirs = ['../../assets']
        }
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])
}
END
open(FH, '>', "$projectname.androidstudio/app/build.gradle") or die $!;
print FH $str;
close FH;


# CMakeLists.txt (Module)
$str = <<END;
cmake_minimum_required(VERSION 3.4.1)
file(GLOB_RECURSE SOURCES  "../../app/*.*" "assets/*.*" "platform/*.*" "\$ENV{OAKNUT_DIR}/src/*.*")
include_directories(SYSTEM \$ENV{OAKNUT_DIR}/src)
add_definitions(-DPLATFORM_ANDROID=1)
add_library(oaknutapp SHARED \${SOURCES} )
target_link_libraries(oaknutapp -landroid -lEGL -lGLESv2 -ljnigraphics -llog -lOpenSLES -latomic)
END
open(FH, '>', "$projectname.androidstudio/app/CMakeLists.txt") or die $!;
print FH $str;
close FH;

#!/usr/bin/perl

# This Perl script generates project files for Visual Studio Code (VSCode)


use warnings;
use strict;
use File::Basename;
use Getopt::Long;

my $projectname="app";
GetOptions ("projectname=s" => \$projectname);

mkdir '.vscode' unless -d '.vscode';


# <projectname>.code-workspace
my $str = <<END;
{
	"folders": [
		{
			"path": "."
		},
		{
			"name": "Oaknut",
			"path": "$ENV{OAKNUT_DIR}/src"
		}
	],
	"settings": {
		"files.associations": {
			"functional": "cpp"
		}
	}
}
END
open(FH, '>', "$projectname.code-workspace") or die $!;
print FH $str;
close FH;


# .vscode/tasks.json
$str = <<END;
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build native (debug)",
            "type": "shell",
            "command": "make CONFIG=debug",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Build native (release)",
            "type": "shell",
            "command": "make CONFIG=release",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Build android (debug)",
            "type": "shell",
            "command": "make PLATFORM=android CONFIG=debug",
            "group": "build",
            "problemMatcher": []
        },
				{
            "label": "Build android (release)",
            "type": "shell",
            "command": "make PLATFORM=android CONFIG=release",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Build web_wasm (debug)",
            "type": "shell",
            "command": "make PLATFORM=web_wasm CONFIG=debug",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Build web_wasm (release)",
            "type": "shell",
            "command": "make PLATFORM=web_wasm CONFIG=release",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Build web_asmjs (debug)",
            "type": "shell",
            "command": "make PLATFORM=web_asmjs CONFIG=debug",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Build web_asmjs (release)",
            "type": "shell",
            "command": "make PLATFORM=web_asmjs CONFIG=release",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Clean native (debug)",
            "type": "shell",
            "command": "make clean",
            "group": "none",
            "problemMatcher": []
        }
    ]
}
END
open(FH, '>', ".vscode/tasks.json") or die $!;
print FH $str;
close FH;

# .vscode/launch.json
$str = <<END;
{
    "version": "0.2.0",
    "configurations": [

        {
            "name": "Run native (debug)",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/.build/macos/debug/$projectname.app/Contents/MacOS/$projectname",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "lldb",
            "preLaunchTask": "Build native (debug)"
        },
        {
            "name": "Run native (release)",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/.build/macos/release/$projectname.app/Contents/MacOS/$projectname",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "lldb",
            "preLaunchTask": "Build native (release)"
        },
        {
            "name": "Launch web_wasm (debug)",
            "type": "python",
            "request": "launch",
            "program": "\${env:EMSCRIPTEN_ROOT}/emrun.py",
            "args" : ["\${workspaceFolder}/.build/web_wasm/debug/webroot/xx.html"],
            "preLaunchTask": "Build web_wasm (debug)"
        },
        {
            "name": "Launch web_wasm (release)",
            "type": "python",
            "request": "launch",
            "program": "\${env:EMSCRIPTEN_ROOT}/emrun.py",
            "args" : ["\${workspaceFolder}/.build/web_wasm/release/webroot/xx.html"],
            "preLaunchTask": "Build web_wasm (release)"
        },
        {
            "name": "Launch web_asmjs (debug)",
            "type": "python",
            "request": "launch",
            "program": "\${env:EMSCRIPTEN_ROOT}/emrun.py",
            "args" : ["\${workspaceFolder}/.build/web_asmjs/debug/webroot/xx.html"],
            "preLaunchTask": "Build web_asmjs (debug)"
        },
        {
            "name": "Launch web_asmjs (release)",
            "type": "python",
            "request": "launch",
            "program": "\${env:EMSCRIPTEN_ROOT}/emrun.py",
            "args" : ["\${workspaceFolder}/.build/web_asmjs/release/webroot/xx.html"],
            "preLaunchTask": "Build web_asmjs (release)"
        }
    ]
}
END
open(FH, '>', ".vscode/launch.json") or die $!;
print FH $str;
close FH;


# .vscode/c_cpp_properties.json
$str = <<END;
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "\${workspaceFolder}/**",
                "\${env:OAKNUT_DIR}/src"
            ],
            "defines": [
                "PLATFORM_MAC=1",
                "PLATFORM_APPLE=1"
            ],
            "browse": {
                "path": [
                    "\${workspaceFolder}",
                    "\${env:OAKNUT_DIR}/src"
                ],
                "limitSymbolsToIncludedHeaders": true,
                "databaseFilename": ""
            },
            "macFrameworkPath": [
                "/System/Library/Frameworks",
                "/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
END
open(FH, '>', ".vscode/c_cpp_properties.json") or die $!;
print FH $str;
close FH;

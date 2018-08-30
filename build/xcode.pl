#!/usr/bin/perl


# This Perl script generates an XCode project file for an Oaknut project. The project
# defines native targets for MacOS and iOS, and a make-driven target for Web.
#
# XCode will autogenerate schemes for all three targets, the Web scheme needs a little
# editing to be usable:
#
#   1. Set the executable to be /usr/bin/perl and untick the 'Debug executable' box.
#   2. Add this single launch argument:
#
#      $(EMSCRIPTEN_ROOT)/emrun.py $(SRCROOT)/.build/$(TARGET_NAME)/$(CONFIGURATION)/webroot/xx.html
#
# Now you should be able to run your app in your web browser directly from Xcode.
#
#
# NB: This script is not intended for direct use, run "make xcode" from your project
# directory to ensure this script gets its expected parameters.
#
# NB: This script is the first thing I've ever written in Perl and it probably shows.



use warnings;
use strict;
use File::Basename;
use Getopt::Long;

my $projectname="app";
my @frameworks=();
my @frameworks_macos=();
my @frameworks_ios=();
GetOptions ("projectname=s" => \$projectname,
			"framework=s" => \@frameworks,
            "framework_macos=s" => \@frameworks_macos,
            "framework_ios=s" => \@frameworks_ios);

my $ref_prefix=sprintf("%08X%08X", int(rand(0xFFFFFFFF)), int(rand(0xFFFFFFFF)));
my $current_ref=1;

sub genref {
	return sprintf("$ref_prefix%08d", $current_ref++);
}



my $proj_ref=genref();
my $bcl_ref=genref();
my $bc_debug_ref=genref();
my $bc_release_ref=genref();
my $main_group_ref=genref();
my $app_group_ref=genref();
my $oaknut_group_ref=genref();
my $group_decls="";
my $fileref_decls="";
my $buildfile_decls="";
my $sources_decls="";
my $assets_fileref=genref();
my $frameworksRefGroup=genref();
my $productRefGroup=genref();
#my $frameworks_refs="";
#my $frameworks_buildrefs="";
my $product_refs="";


$fileref_decls.="$assets_fileref /* assets */ = {isa = PBXFileReference; lastKnownFileType = folder; path = assets; sourceTree = \"<group>\"; };\n";

sub gen_framework_decls {
	my $refs="";
	my $buildrefs="";
	foreach my $framework (@_) {
		my $ref=genref();
		$fileref_decls.="$ref /* $framework.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = $framework.framework; path = System/Library/Frameworks/$framework.framework; sourceTree = SDKROOT; };\n";
		$refs.="$ref,\n";
		my $buildref=genref();
		$buildfile_decls .= "$buildref /* $framework.framework */ = {isa = PBXBuildFile; fileRef = $ref /* $framework.framework */; };\n";
		$buildrefs.="$buildref,\n";
	}
	return ($refs, $buildrefs)
}

my ($frameworks_refs, $frameworks_buildrefs)=gen_framework_decls(@frameworks);
my ($frameworks_refs_macos, $frameworks_buildrefs_macos)=gen_framework_decls(@frameworks_macos);
my ($frameworks_refs_ios, $frameworks_buildrefs_ios)=gen_framework_decls(@frameworks_ios);


sub filetype {
	my($filename, $dirs, $suffix) = fileparse(@_, qr/\.[^.]*/);
	if ($suffix eq ".hpp") {return "sourcecode.cpp.h";}
	if ($suffix eq ".cpp") {return "sourcecode.cpp.cpp";}
	if ($suffix eq ".h") {return "sourcecode.c.h";}
	if ($suffix eq ".c") {return "sourcecode.c.c";}
	if ($suffix eq ".m") {return "sourcecode.c.objc";}
	if ($suffix eq ".mm") {return "sourcecode.cpp.objcpp";}
	if ($suffix eq ".swift") {return "sourcecode.swift";}
	return "text";
}
sub is_translation_unit {
	my($filename, $dirs, $suffix) = fileparse(@_, qr/\.[^.]*/);
	if ($suffix eq ".cpp") {return 1;}
	if ($suffix eq ".c") {return 1;}
	if ($suffix eq ".m") {return 1;}
	if ($suffix eq ".mm") {return 1;}
	if ($suffix eq ".swift") {return 1;}
	return 0;
}

sub add_source_group {

	my ($label, $pwd, $parent_group_ref, $is_absolute) = @_;
	opendir(DIR,"$pwd") or die "Cannot open $pwd\n";
	my @files = sort readdir(DIR);
	closedir(DIR);
	my @children=();
	foreach my $file (@files) {
		if ($file =~ /^\.\.?$/) {
			next;
		}
		my $ref=genref();
		push @children, $ref;
		if (-d "$pwd/$file") {
			add_source_group($file, "$pwd/$file", $ref, 0);
		} else {
			if ($file eq ".DS_Store") {
				next;
			}
			my $filetype = filetype($file);
			$fileref_decls .= qq($ref /* $file */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = $filetype; path = $file; sourceTree = "<group>"; };
			);
			if (is_translation_unit($file)) {
				my $buildfile_ref=genref();
				$buildfile_decls .= "$buildfile_ref /* $file */ = {isa = PBXBuildFile; fileRef = $ref /* $file */; };\n";
				$sources_decls .= "$buildfile_ref /* $file */,\n";
			}
		}
	}
	my $foo = join(",\n", @children);
	my $bnd = $is_absolute ? $pwd : basename($pwd);
	my $sourceTree = $is_absolute ? "<absolute>" : "<group>";
	$group_decls .= qq(
		$parent_group_ref = {
			isa = PBXGroup;
			children = (
			$foo
			);
			name = $label;
			path = $bnd;
			sourceTree = "$sourceTree";
		};
	);
}


sub gen_target {
	my ($target_name, $info_plist_path, $frameworks_buildrefs_extra, $build_settings)=@_;
	my $target_ref = genref();
	my $bcl_native_ref=genref();
	my $bc_native_debug_ref=genref();
	my $bc_native_release_ref=genref();
	my $phase_sources_ref=genref();
	my $phase_frameworks_ref=genref();
	my $phase_resources_ref=genref();
	my $product_ref=genref();
	my $assets_buildref=genref();
	$buildfile_decls .= "$assets_buildref /* assets */ = {isa = PBXBuildFile; fileRef = $assets_fileref /* assets */; };\n";

	# Extract the bundle identifier from the Info.plist. This is contrary to normal XCode usage where the .plist
	# references the PRODUCT_BUNDLE_IDENTIFIER setting in the project file.
	open my $input, '<', $info_plist_path or die "can't open info.plist: $!";
	my @lines = <$input>;
	my $info = join('\n',@lines);
	close $input;
	my $i1 = index($info, "CFBundleIdentifier");
	if ($i1<0) { die "CFBundleIdentifier not in Info.plist"; }
	my $i2 = index($info, "<string>", $i1) + 8;
	my $i3 = index($info, "</string>", $i2);
	my $bundle_id = substr($info, $i2, $i3-$i2);
	if ($bundle_id eq "\${PRODUCT_BUNDLE_IDENTIFIER}") {
		die "CFBundleIdentifier is set to PRODUCT_BUNDLE_IDENTIFIER, indicating that it was set in XCode. In Oaknut bundle IDs must be specified in Info.plist files rather than referencing the transient .pbxproj file";
	}

	$build_settings="{\n".$build_settings."\n"
				   ."INFOPLIST_FILE = $info_plist_path;\n"
    			   ."PRODUCT_BUNDLE_IDENTIFIER = $bundle_id;\n"
				   ."}";

	my $target_decls = qq(
	$target_ref = {
		isa = PBXNativeTarget;
		buildConfigurationList = $bcl_native_ref;
		buildPhases = (
			$phase_sources_ref,
			$phase_frameworks_ref,
			$phase_resources_ref,
		);
		buildRules = (
		);
		dependencies = (
		);
		name = $target_name;
		productName = $projectname;
		productReference = $product_ref;
		productType = "com.apple.product-type.application";
	};
	$bcl_native_ref = {
		isa = XCConfigurationList;
		buildConfigurations = (
			$bc_native_debug_ref,
			$bc_native_release_ref,
		);
		defaultConfigurationIsVisible = 0;
		defaultConfigurationName = Release;
	};
	$bc_native_debug_ref = {
		isa = XCBuildConfiguration;
		buildSettings = $build_settings;
		name = debug;
	};
	$bc_native_release_ref = {
		isa = XCBuildConfiguration;
		buildSettings = $build_settings;
		name = release;
	};
	$phase_sources_ref = {
		isa = PBXSourcesBuildPhase;
		buildActionMask = 2147483647;
		files = (
			$sources_decls
		);
		runOnlyForDeploymentPostprocessing = 0;
	};
	$phase_frameworks_ref = {
		isa = PBXFrameworksBuildPhase;
		buildActionMask = 2147483647;
		files = (
			$frameworks_buildrefs
			$frameworks_buildrefs_extra
		);
		runOnlyForDeploymentPostprocessing = 0;
	};
	$phase_resources_ref = {
		isa = PBXResourcesBuildPhase;
		buildActionMask = 2147483647;
		files = (
			$assets_buildref
		);
		runOnlyForDeploymentPostprocessing = 0;
	};
	);

	$fileref_decls.="$product_ref = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; name = ".$projectname."_".$target_name.".app; path = ".$projectname.".app; sourceTree = BUILT_PRODUCTS_DIR; };\n";
	$product_refs.="$product_ref,\n";

	return ($target_ref, $target_decls);
}

my $oaknut_src_dir=$ENV{'OAKNUT_DIR'}."/src";

add_source_group("app", "app", $app_group_ref, 0);
add_source_group("oaknut", $oaknut_src_dir, $oaknut_group_ref, 1);


my $common_project_build_settings=qq(
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++11";
				CLANG_CXX_LIBRARY = "libc++";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				GCC_C_LANGUAGE_STANDARD = gnu11;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				IPHONEOS_DEPLOYMENT_TARGET = 8.3;
				MACOSX_DEPLOYMENT_TARGET = 10.13;
				SYSTEM_HEADER_SEARCH_PATHS = "$oaknut_src_dir";
);

my ($macos_target_ref, $macos_target_decls)=gen_target("macOS",
 		"platform/macos/Info.plist", $frameworks_buildrefs_macos, qq(
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				GCC_PREPROCESSOR_DEFINITIONS = (
					"\$(inherited)",
					"PLATFORM_MACOS=1",
				);
				LD_RUNPATH_SEARCH_PATHS = "\$(inherited) \@executable_path/../Frameworks";
				PRODUCT_NAME = $projectname;
				));

my ($ios_target_ref, $ios_target_decls)=gen_target("iOS",
 		"platform/ios/Info.plist", $frameworks_buildrefs_ios, qq(
				CODE_SIGN_STYLE = Automatic;
				GCC_PREPROCESSOR_DEFINITIONS = (
					"\$(inherited)",
					"PLATFORM_IOS=1",
				);
				IPHONEOS_DEPLOYMENT_TARGET = 8.3;
				LD_RUNPATH_SEARCH_PATHS = "\$(inherited) \@executable_path/Frameworks";
				PRODUCT_NAME = $projectname;
				SDKROOT = iphoneos;
				TARGETED_DEVICE_FAMILY = "1,2";
				));

sub gen_web_target {
	my ($target_name)=@_;
	my $web_target_ref=genref();
	my $web_bcl_ref=genref();
	my $web_bc_debug_ref=genref();
	my $web_bc_release_ref=genref();

	my $web_target_decls=qq(
		$web_target_ref /* $target_name */ = {
			isa = PBXLegacyTarget;
			buildArgumentsString = "\$(ACTION) PLATFORM=\$(TARGET_NAME) CONFIG=\$(CONFIGURATION)";
			buildConfigurationList = $web_bcl_ref /* Build configuration list for PBXLegacyTarget "$target_name" */;
			buildPhases = (
			);
			buildToolPath = /usr/bin/make;
			buildWorkingDirectory = "";
			dependencies = (
			);
			name = $target_name;
			passBuildSettingsInEnvironment = 1;
			productName = $projectname;
		};
		$web_bcl_ref /* Build configuration list for PBXLegacyTarget "$target_name" */ = {
			isa = XCConfigurationList;
			buildConfigurations = (
				$web_bc_debug_ref /* debug */,
				$web_bc_release_ref /* release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = release;
		};
		$web_bc_debug_ref /* debug */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				EMSCRIPTEN_ROOT = "$ENV{'EMSCRIPTEN_ROOT'}";
				OAKNUT_DIR = $ENV{'OAKNUT_DIR'};
			};
			name = debug;
		};
		$web_bc_release_ref /* release */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				EMSCRIPTEN_ROOT = "$ENV{'EMSCRIPTEN_ROOT'}";
				OAKNUT_DIR = $ENV{'OAKNUT_DIR'};
			};
			name = release;
		};
	);

	return ($web_target_ref, $web_target_decls);
}

my ($web_asmjs_target_ref, $web_asmjs_target_decls)=gen_web_target("web_asmjs");
my ($web_wasm_target_ref, $web_wasm_target_decls)=gen_web_target("web_wasm");



print qq(// !\$*UTF8*\$!
{
archiveVersion = 1;
classes = {
};
objectVersion = 48;
objects = {

	$fileref_decls

	$buildfile_decls

	$proj_ref = {
		isa = PBXProject;
		attributes = {
			TargetAttributes = {
				$macos_target_ref = {
					ProvisioningStyle = Automatic;
				};
				$ios_target_ref = {
					ProvisioningStyle = Automatic;
				};
			};
		};
		buildConfigurationList = $bcl_ref;
		compatibilityVersion = "Xcode 8.0";
		developmentRegion = en;
		hasScannedForEncodings = 0;
		knownRegions = (
			en,
			Base,
		);
		mainGroup = $main_group_ref;
		productRefGroup = $productRefGroup;
		projectDirPath = "";
		projectRoot = "";
		targets = (
			$macos_target_ref,
			$ios_target_ref,
			$web_asmjs_target_ref,
			$web_wasm_target_ref,
		);
	};
	$bcl_ref = {
		isa = XCConfigurationList;
		buildConfigurations = (
			$bc_debug_ref,
			$bc_release_ref,
		);
		defaultConfigurationIsVisible = 0;
		defaultConfigurationName = Release;
	};
	$bc_debug_ref = {
		isa = XCBuildConfiguration;
		buildSettings = {
			$common_project_build_settings
			ENABLE_TESTABILITY = YES;
			DEBUG_INFORMATION_FORMAT = dwarf;
			GCC_DYNAMIC_NO_PIC = NO;
			GCC_OPTIMIZATION_LEVEL = 0;
			GCC_PREPROCESSOR_DEFINITIONS = (
				"PLATFORM_APPLE=1",
				"DEBUG=1",
			);
		};
		name = debug;
	};
	$bc_release_ref = {
		isa = XCBuildConfiguration;
		buildSettings = {
			$common_project_build_settings
			DEBUG_INFORMATION_FORMAT = "dwarf-with-dsym";
			ENABLE_NS_ASSERTIONS = NO;
			GCC_PREPROCESSOR_DEFINITIONS = "PLATFORM_APPLE=1";
		};
		name = release;
	};
	$main_group_ref = {
		isa = PBXGroup;
		children = (
			$app_group_ref,
			$assets_fileref,
			$oaknut_group_ref,
			$frameworksRefGroup,
			$productRefGroup,
		);
		sourceTree = "<group>";
	};
	$frameworksRefGroup={
		isa = PBXGroup;
		children = (
			$frameworks_refs
			$frameworks_refs_macos
			$frameworks_refs_ios
		);
		name = Frameworks;
		sourceTree = "<group>";
	};
	$productRefGroup = {
		isa = PBXGroup;
		children = (
			$product_refs);
		name = Products;
		sourceTree = "<group>";
	};
	$group_decls

	$macos_target_decls

	$ios_target_decls

	$web_asmjs_target_decls
	$web_wasm_target_decls


	};
    rootObject = $proj_ref;
}
);

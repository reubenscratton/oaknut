# Declarations common to iOS and macOS builds

CFLAGS+= -DPLATFORM_APPLE=1

XCODE_APP:=/Applications/Xcode.app
XCODE_TOOLCHAIN:=$(XCODE_APP)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain
CLANG := $(XCODE_TOOLCHAIN)/usr/bin/clang

FLAGS := \
	-fmessage-length=169 \
	-fdiagnostics-show-note-include-stack \
	-fmacro-backtrace-limit=0 \
	-fcolor-diagnostics \
	-fmodules \
	-gmodules \
	-fmodules-prune-interval=86400 \
	-fmodules-prune-after=345600 \
	-fpascal-strings \
	-fno-common \
	-fstrict-aliasing \
	-fvisibility=hidden

FLAGS_CPP := \
	-fvisibility-inlines-hidden

WARNINGS := \
	-Wnon-modular-include-in-framework-module \
	-Werror=non-modular-include-in-framework-module \
	-Wno-trigraphs \
	-Wno-missing-field-initializers \
	-Wno-missing-prototypes \
	-Werror=return-type \
	-Wunreachable-code \
	-Werror=objc-root-class \
	-Wno-overloaded-virtual \
	-Wno-exit-time-destructors \
	-Wno-missing-braces \
	-Wparentheses \
	-Wswitch \
	-Wunused-function \
	-Wno-unused-label \
	-Wno-unused-parameter \
	-Wunused-variable \
	-Wunused-value \
	-Wempty-body \
	-Wuninitialized \
	-Wconditional-uninitialized \
	-Wno-unknown-pragmas \
	-Wno-shadow \
	-Wno-four-char-constants \
	-Wno-conversion \
	-Wconstant-conversion \
	-Wint-conversion \
	-Wbool-conversion \
	-Wenum-conversion \
	-Wno-float-conversion \
	-Wnon-literal-null-conversion \
	-Wobjc-literal-conversion \
	-Wshorten-64-to-32 \
	-Wno-newline-eof \
	-Wdeprecated-declarations \
	-Wno-sign-conversion \
	-Winfinite-recursion \
	-Wcomma \
	-Wblock-capture-autoreleasing \
	-Wstrict-prototypes

WARNINGS_CPP := \
	-Wno-c++11-extensions \
	-Winvalid-offsetof \
	-Wmove \
	-Wrange-loop-analysis

WARNINGS_OBJC := \
	-Wno-implicit-atomic-properties \
	-Werror=deprecated-objc-isa-usage \
	-Werror=objc-root-class \
	-Wno-arc-repeated-use-of-weak \
	-Wduplicate-method-match \
	-Wno-missing-braces \
	-Wparentheses \
	-Wpointer-sign \
	-Wno-newline-eof \
	-Wno-selector \
	-Wno-strict-selector-match \
	-Wundeclared-selector \
	-Wno-deprecated-implementations \
	-Wprotocol

FRAMEWORKS:=-framework CoreText \
			-framework GLKit \
			-framework SceneKit \
			-framework AudioToolbox \
			-framework OpenAL

ifdef OAKNUT_WANT_CAMERA
	FRAMEWORKS+= -framework AVFoundation -framework CoreMedia
endif
ifdef OAKNUT_WANT_AUDIOINPUT
	FRAMEWORKS+= -framework AudioToolbox -framework CoreMedia
endif

.PHONY: build run

all: build run

build:
	xcodebuild -configuration Debug -project xcode/interp1.xcodeproj/

run:
	./xcode/build/Debug/interp1.app/Contents/MacOS/interp1

.PHONY: build
build:
	mkdir -p build
	cmake -E chdir build cmake -DCMAKE_BUILD_TYPE=Debug ..
	cmake --build build

.PHONY: release
release:
	mkdir -p build_release
	cmake -E chdir build_release cmake -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build_release

.PHONY: run
run:
	build/game

.PHONY: run_release
run_release:
	build_release/game

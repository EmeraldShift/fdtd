#!/bin/bash
set -euxo pipefail

OUT=${OUT:-out}
HWPAR=${HWPAR:-}

[[ "$#" -ge 1 ]] && [[ "$1" == "r" ]] && {
	# Rebuild CMake files
	rm -rf "./$OUT"
	mkdir "$OUT"
	(cd "$OUT"; cmake -DCMAKE_CXX_FLAGS=-DQT=${QT:-1} ..)
}

# Build it
(cd "$OUT"; make -j ${HWPAR})

# Copy binary out into root directory for convenience
find "${OUT}" -maxdepth 1 -executable -type f | xargs cp -t.

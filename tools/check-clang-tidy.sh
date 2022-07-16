#!/bin/bash
set -o errexit
# Don't fail if there are no files passed into xargs
# set -o pipefail
set -o nounset
set -o noclobber

REPO_ROOT="$(git rev-parse --show-toplevel)"

git ls-files |
    grep --file="$REPO_ROOT/.clang-tidy.include" --extended-regexp |
    grep --file="$REPO_ROOT/.clang-tidy.exclude" --extended-regexp --invert-match |
    xargs --no-run-if-empty run-clang-tidy -header-filter=.* -p "$REPO_ROOT/build/"

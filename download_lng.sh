#!/bin/bash

## Download languages
REPO_URL="https://github.com/ps2homebrew/Open-PS2-Loader-lang"
REPO_FOLDER="lng_src"
BRANCH_NAME="main"
# If languages are already vendored locally, do not require network access.
if test -f "$REPO_FOLDER/English.yml"; then
  echo "Languages already present locally; skipping download."
  exit 0
fi
if test ! -d "$REPO_FOLDER"; then
  git clone --depth 1 -b $BRANCH_NAME $REPO_URL "$REPO_FOLDER" || exit 1
else
  (cd "$REPO_FOLDER" && git fetch origin && git reset --hard "origin/${BRANCH_NAME}" && git checkout "$BRANCH_NAME" && cd - )|| exit 1
fi

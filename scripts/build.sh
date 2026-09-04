#!/usr/bin/env bash

set -e

bear -- make -j"$(($(nproc) - 1))"


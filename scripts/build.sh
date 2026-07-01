#!/usr/bin/env bash
set -euo pipefail

ROOT="$(pwd)"
LIBS_DIR="$ROOT/libs"
VENDOR_DIR="$ROOT/vendor"
INCLUDES_DIR="$ROOT/include/vendor"

mkdir -p "$LIBS_DIR"
mkdir -p "$INCLUDES_DIR"

N_PROC=$(( $(nproc) > 1 ? $(nproc) - 1 : 1 ))

run_if_missing() {
  local out_file="$1"
  shift

  if [[ -f "$out_file" ]]; then
    echo "exists: $(basename "$out_file")"
    return 0
  fi

  "$@"
}

# uSockets
# ========================================
run_if_missing "$LIBS_DIR/uSockets.a" bash -c "
set -euo pipefail
cd '$VENDOR_DIR/uWebSockets'

make -j$N_PROC WITH_OPENSSL=1

if [[ -f uSockets/uSockets.a ]]; then
  cp uSockets/uSockets.a '$LIBS_DIR/uSockets.a'
  mkdir -p '$INCLUDES_DIR/uWebSockets'
  cp src/*.h '$INCLUDES_DIR/uWebSockets'
  
  mkdir -p '$INCLUDES_DIR/uWebSockets/uSockets'
  cp uSockets/src/*.h '$INCLUDES_DIR/uWebSockets/uSockets'
else
  echo 'uSockets.a not found'
  exit 1
fi
"

# libpq
run_if_missing "$LIBS_DIR/libpq.a" bash -c "
set -euo pipefail

cd '$VENDOR_DIR/libpq'

./configure \
  --without-readline \
  --without-zlib \
  --disable-shared

make -j$N_PROC -C src/interfaces/libpq

cp -f src/interfaces/libpq/libpq.a '$LIBS_DIR/libpq.a'
cp -f src/common/libpgcommon_shlib.a libs '$LIBS_DIR'
cp -f src/port/libpgport_shlib.a '$LIBS_DIR"

"

# libpqxx
# ========================================
run_if_missing "$LIBS_DIR/libpqxx.a" bash -c "
set -euo pipefail
cd '$VENDOR_DIR/libpqxx'

CXXFLAGS='-std=c++20' bash configure
make -j$N_PROC

PQXX_A=\$(find src -name 'libpqxx.a' | head -n 1)

if [[ -z \"\$PQXX_A\" ]]; then
  echo 'libpqxx.a not found'
  exit 1
fi

cp \"\$PQXX_A\" '$LIBS_DIR/libpqxx.a'
"

# OpenSSL
# ========================================
run_if_missing "$LIBS_DIR/libcrypto.a" bash -c "
set -euo pipefail
cd '$VENDOR_DIR/openssl'

./Configure
make -j$N_PROC

if [[ -f libcrypto.a && -f libssl.a ]]; then
  cp libcrypto.a libssl.a '$LIBS_DIR/'
else
  echo 'OpenSSL static libs not found'
  exit 1
fi
"

# libsodium
# ========================================
run_if_missing "$LIBS_DIR/libsodium.a" bash -c "
set -euo pipefail
cd '$VENDOR_DIR/libsodium'

bash configure
make -j$N_PROC

SODIUM_A=\$(find src -name 'libsodium.a' | head -n 1)

if [[ -z \"\$SODIUM_A\" ]]; then
  echo 'libsodium.a not found'
  exit 1
fi

cp \"\$SODIUM_A\" '$LIBS_DIR/libsodium.a'
"

echo "All static libraries built"


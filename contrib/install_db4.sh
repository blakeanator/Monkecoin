#!/bin/sh
# Copyright (c) 2017-2019 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# Install libdb5.3 (Berkeley DB).

export LC_ALL=C
set -e

if [ -z "${1}" ]; then
  echo "Usage: $0 <base-dir> [<extra-bdb-configure-flag> ...]"
  echo
  echo "Must specify a single argument: the directory in which db4 will be built."
  echo "This is probably \`pwd\` if you're at the root of the monkecoin repository."
  exit 1
fi

expand_path() {
  cd "${1}" && pwd -P
}

BDB_PREFIX="$(expand_path ${1})/db4"; shift;
BDB_VERSION='db-5.3.28.NC'
BDB_HASH='76a25560d9e52a198d37a31440fd07632b5f1f8f9f2b6d5438f4bc3e7c9013ef'
BDB_URL="https://www.monkecoin.net/downloads/${BDB_VERSION}.tar.gz"

check_exists() {
  command -v "$1" >/dev/null
}

sha256_check() {
  # Args: <sha256_hash> <filename>
  #
  if check_exists sha256sum; then
    echo "${1}  ${2}" | sha256sum -c
  elif check_exists sha256; then
    if [ "$(uname)" = "FreeBSD" ]; then
      sha256 -c "${1}" "${2}"
    else
      echo "${1}  ${2}" | sha256 -c
    fi
  else
    echo "${1}  ${2}" | shasum -a 256 -c
  fi
}

http_get() {
  # Args: <url> <filename> <sha256_hash>
  #
  # It's acceptable that we don't require SSL here because we manually verify
  # content hashes below.
  #
  if [ -f "${2}" ]; then
    echo "File ${2} already exists; not downloading again"
  elif check_exists curl; then
    curl --insecure --retry 5 "${1}" -o "${2}"
  else
    wget --no-check-certificate "${1}" -O "${2}"
  fi

  sha256_check "${3}" "${2}"
}

mkdir -p "${BDB_PREFIX}"
http_get "${BDB_URL}" "${BDB_VERSION}.tar.gz" "${BDB_HASH}"
tar -xzvf ${BDB_VERSION}.tar.gz -C "$BDB_PREFIX"
cd "${BDB_PREFIX}/${BDB_VERSION}/"

# Apply a patch necessary when building with clang and c++11 (see https://community.oracle.com/thread/3952592)
CLANG_CXX11_PATCH_URL='https://gist.githubusercontent.com/LnL7/5153b251fd525fe15de69b67e63a6075/raw/7778e9364679093a32dec2908656738e16b6bdcb/clang.patch'
CLANG_CXX11_PATCH_HASH='7a9a47b03fd5fb93a16ef42235fa9512db9b0829cfc3bdf90edd3ec1f44d637c'
http_get "${CLANG_CXX11_PATCH_URL}" clang.patch "${CLANG_CXX11_PATCH_HASH}"
patch -p2 < clang.patch

# The packaged config.guess and config.sub are ancient (2009) and can cause build issues.
# Replace them with modern versions.
# See https://github.com/bitcoin/bitcoin/issues/16064
CONFIG_GUESS_URL='https://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=1dc64510987d67f06a851b66203f2a2286c32b1f'
CONFIG_GUESS_HASH='7d1e3c79b86de601c3a0457855ab854dffd15163f53c91edac54a7be2e9c931b'
CONFIG_SUB_URL=${CONFIG_GUESS_URL/guess/sub}
CONFIG_SUB_HASH='0c6489c65150773a2a94eebaa794b079e74a403b50b48d5adb69fc6cd14f4810'

rm -f "dist/config.guess"
rm -f "dist/config.sub"

http_get "${CONFIG_GUESS_URL}" dist/config.guess "${CONFIG_GUESS_HASH}"
http_get "${CONFIG_SUB_URL}" dist/config.sub "${CONFIG_SUB_HASH}"

cd build_unix/

"${BDB_PREFIX}/${BDB_VERSION}/dist/configure" \
  --enable-cxx --disable-shared --disable-replication --with-pic --prefix="${BDB_PREFIX}" \
  "${@}"

make install

echo
echo "db4 build complete."
echo
# shellcheck disable=SC2016
echo 'When compiling monkecoind, run `./configure` in the following way:'
echo
echo "  export BDB_PREFIX='${BDB_PREFIX}'"
# shellcheck disable=SC2016
echo '  ./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-5.3" BDB_CFLAGS="-I${BDB_PREFIX}/include" ...'

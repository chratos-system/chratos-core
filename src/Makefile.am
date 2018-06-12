DIST_SUBDIRS = secp256k1 univalue

AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS)
AM_CXXFLAGS = $(HARDENED_CXXFLAGS)
AM_CPPFLAGS = $(HARDENED_CPPFLAGS)
EXTRA_LIBRARIES =

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

CHRATOS_CONFIG_INCLUDES=-I$(builddir)/config
CHRATOS_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

CHRATOS_INCLUDES += -I$(srcdir)/secp256k1/include
CHRATOS_INCLUDES += $(UNIVALUE_CFLAGS)

LIBCHRATOS_SERVER=libchratos_server.a
LIBCHRATOS_COMMON=libchratos_common.a
LIBCHRATOS_CONSENSUS=libchratos_consensus.a
LIBCHRATOS_CLI=libchratos_cli.a
LIBCHRATOS_UTIL=libchratos_util.a
LIBCHRATOS_CRYPTO=crypto/libchratos_crypto.a
LIBCHRATOSQT=qt/libchratosqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

if ENABLE_ZMQ
LIBCHRATOS_ZMQ=libchratos_zmq.a
endif
if BUILD_CHRATOS_LIBS
LIBCHRATOSCONSENSUS=libchratosconsensus.la
endif
if ENABLE_WALLET
LIBCHRATOS_WALLET=libchratos_wallet.a
endif

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES += \
  $(LIBCHRATOS_CRYPTO) \
  $(LIBCHRATOS_UTIL) \
  $(LIBCHRATOS_COMMON) \
  $(LIBCHRATOS_CONSENSUS) \
  $(LIBCHRATOS_SERVER) \
  $(LIBCHRATOS_CLI) \
  $(LIBCHRATOS_WALLET) \
  $(LIBCHRATOS_ZMQ)

lib_LTLIBRARIES = $(LIBCHRATOSCONSENSUS)

bin_PROGRAMS =
TESTS =
BENCHMARKS =

if BUILD_CHRATOSD
  bin_PROGRAMS += chratosd
endif

if BUILD_CHRATOS_UTILS
  bin_PROGRAMS += chratos-cli chratos-tx
endif

.PHONY: FORCE check-symbols check-security
# chratos core #
CHRATOS_CORE_H = \
  addressindex.h \
  spentindex.h \
  timestampindex.h \
  addrman.h \
  base58.h \
  bignum.h \
  bloom.h \
  blockencodings.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  consensus/consensus.h \
  core_io.h \
  core_memusage.h \
	dividend/dividend.h \
	dividend/dividenddb.h \
	dividend/rpcdividend.h \
  httprpc.h \
  httpserver.h \
  indirectmap.h \
  kernel.h \
  init.h \
  key.h \
  keystore.h \
  dbwrapper.h \
  limitedmap.h \
  main.h \
  memusage.h \
  merkleblock.h \
  miner.h \
  net.h \
  netbase.h \
  noui.h \
  policy/fees.h \
  policy/policy.h \
  policy/rbf.h \
	pow.h \
  pos.h \
  consensus/cfund.h \
  protocol.h \
  random.h \
  reverselock.h \
  rpc/client.h \
  rpc/protocol.h \
  rpc/server.h \
  rpc/register.h \
  scheduler.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/ismine.h \
  streams.h \
  support/allocators/secure.h \
  support/allocators/zeroafterfree.h \
  support/cleanse.h \
  support/pagelocker.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  torcontrol.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utils/dns_utils.h \
  utiltime.h \
  validationinterface.h \
  versionbits.h \
  wallet/crypter.h \
  wallet/db.h \
  wallet/rpcwallet.h \
  wallet/wallet.h \
  wallet/walletdb.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h\
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h


obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libchratos_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between chratosd and chratos-qt
libchratos_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libchratos_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_server_a_SOURCES = \
  addrman.cpp \
  bloom.cpp \
  blockencodings.cpp \
  chain.cpp \
  checkpoints.cpp \
  consensus/cfund.cpp \
  httprpc.cpp \
  httpserver.cpp \
  kernel.cpp \
  init.cpp \
  dbwrapper.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  policy/fees.cpp \
  policy/policy.cpp \
  pow.cpp \
  pos.cpp \
  rest.cpp \
  rpc/blockchain.cpp \
  rpc/mining.cpp \
  rpc/misc.cpp \
  rpc/net.cpp \
  rpc/rawtransaction.cpp \
  rpc/server.cpp \
  script/sigcache.cpp \
  script/ismine.cpp \
  timedata.cpp \
  torcontrol.cpp \
  txdb.cpp \
  txmempool.cpp \
  ui_interface.cpp \
  utils/dns_utils.cpp \
  validationinterface.cpp \
  versionbits.cpp \
  $(CHRATOS_CORE_H)

if ENABLE_ZMQ
libchratos_zmq_a_CPPFLAGS = $(CHRATOS_INCLUDES) $(ZMQ_CFLAGS)
libchratos_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif


# wallet: shared between chratosd and chratos-qt, but only linked
# when wallet enabled
libchratos_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
libchratos_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_wallet_a_SOURCES = \
  consensus/cfund.cpp \
  wallet/crypter.cpp \
  wallet/db.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  wallet/wallet.cpp \
  wallet/walletdb.cpp \
  policy/rbf.cpp \
  $(CHRATOS_CORE_H)

# crypto primitives library
crypto_libchratos_crypto_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_CONFIG_INCLUDES)
crypto_libchratos_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
crypto_libchratos_crypto_a_SOURCES = \
  crypto/aes.cpp \
  crypto/aes.h \
  crypto/common.h \
  crypto/hmac_sha256.cpp \
  crypto/hmac_sha256.h \
  crypto/hmac_sha512.cpp \
  crypto/hmac_sha512.h \
  crypto/ripemd160.cpp \
  crypto/ripemd160.h \
  crypto/sha1.cpp \
  crypto/sha1.h \
  crypto/sha256.cpp \
  crypto/sha256.h \
  crypto/sha512.cpp \
  crypto/sha512.h

# consensus: shared between all executables that validate any consensus rules.
libchratos_consensus_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
libchratos_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_consensus_a_SOURCES = \
  amount.h \
  arith_uint256.cpp \
  arith_uint256.h \
  consensus/merkle.cpp \
  consensus/merkle.h \
  consensus/params.h \
  consensus/validation.h \
	dividend/dividend.cpp \
	dividend/dividenddb.cpp \
	dividend/rpcdividend.cpp \
	aes_helper.c \
	blake.c \
	bmw.c \
	cubehash.c \
	echo.c \
	fugue.c \
	groestl.c \
	keccak.c \
	hamsi_helper.c \
	hamsi.c \
	jh.c \
	luffa.c \
	shavite.c \
	simd.c \
	skein.c \
	sph_blake.h \
	sph_bmw.h \
	sph_cubehash.h \
	sph_echo.h \
	sph_fugue.h \
	sph_groestl.h \
	sph_hamsi.h \
	sph_jh.h \
	sph_keccak.h \
	sph_luffa.h \
	sph_shavite.h \
	sph_simd.h \
	sph_skein.h \
	sph_types.h \
	hashblock.h \
  hash.cpp \
  hash.h \
  prevector.h \
  primitives/block.cpp \
  primitives/block.h \
  primitives/transaction.cpp \
  primitives/transaction.h \
  pubkey.cpp \
  pubkey.h \
  script/chratosconsensus.cpp \
  script/interpreter.cpp \
  script/interpreter.h \
  script/script.cpp \
  script/script.h \
  script/script_error.cpp \
  script/script_error.h \
  serialize.h \
	tinyformat.h \
  uint256.cpp \
  uint256.h \
  utilstrencodings.cpp \
  utilstrencodings.h \
  version.h


# common: shared between chratosd, and chratos-qt and non-server tools
libchratos_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
libchratos_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_common_a_SOURCES = \
  amount.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  core_read.cpp \
  core_write.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  protocol.cpp \
  scheduler.cpp \
  script/sign.cpp \
  script/standard.cpp \
  $(CHRATOS_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libchratos_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
libchratos_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_util_a_SOURCES = \
  support/pagelocker.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpc/protocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(CHRATOS_CORE_H)

if GLIBC_BACK_COMPAT
libchratos_util_a_SOURCES += compat/glibc_compat.cpp
endif

# cli: shared between chratos-cli and chratos-qt
libchratos_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
libchratos_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libchratos_cli_a_SOURCES = \
  rpc/client.cpp \
  $(CHRATOS_CORE_H)

nodist_libchratos_util_a_SOURCES = $(srcdir)/obj/build.h
#

# chratosd binary #
chratosd_SOURCES = chratosd.cpp
chratosd_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
chratosd_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
chratosd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
chratosd_SOURCES += chratosd-res.rc
endif

chratosd_LDADD = \
  $(LIBCHRATOS_SERVER) \
  $(LIBCHRATOS_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBCHRATOS_UTIL) \
  $(LIBCHRATOS_WALLET) \
  $(LIBCHRATOS_ZMQ) \
  $(LIBCHRATOS_CONSENSUS) \
  $(LIBCHRATOS_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

chratosd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(UNBOUND_LIBS) $(CURL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(ZMQ_LIBS)

# chratos-cli binary #
chratos_cli_SOURCES = chratos-cli.cpp
chratos_cli_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES) $(EVENT_CFLAGS)
chratos_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
chratos_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
chratos_cli_SOURCES += chratos-cli-res.rc
endif

chratos_cli_LDADD = \
  $(LIBCHRATOS_CLI) \
  $(LIBUNIVALUE) \
  $(LIBCHRATOS_UTIL) \
  $(LIBCHRATOS_CRYPTO)

chratos_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(UNBOUND_LIBS) $(CURL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS)
#

# chratos-tx binary #
chratos_tx_SOURCES = chratos-tx.cpp
chratos_tx_CPPFLAGS = $(AM_CPPFLAGS) $(CHRATOS_INCLUDES)
chratos_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
chratos_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
chratos_tx_SOURCES += chratos-tx-res.rc
endif

chratos_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBCHRATOS_COMMON) \
  $(LIBCHRATOS_UTIL) \
  $(LIBCHRATOS_CONSENSUS) \
  $(LIBCHRATOS_CRYPTO) \
  $(LIBSECP256K1)

chratos_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)
#

# chratosconsensus library #
if BUILD_CHRATOS_LIBS
include_HEADERS = script/chratosconsensus.h
libchratosconsensus_la_SOURCES = $(crypto_libchratos_crypto_a_SOURCES) $(libchratos_consensus_a_SOURCES)

if GLIBC_BACK_COMPAT
  libchratosconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

libchratosconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libchratosconsensus_la_LIBADD = $(LIBSECP256K1)
libchratosconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_CHRATOS_INTERNAL
libchratosconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)

endif
#

CTAES_DIST =  crypto/ctaes/bench.c
CTAES_DIST += crypto/ctaes/ctaes.c
CTAES_DIST += crypto/ctaes/ctaes.h
CTAES_DIST += crypto/ctaes/README.md
CTAES_DIST += crypto/ctaes/test.c

CLEANFILES = $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += consensus/*.gcda consensus/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += policy/*.gcda policy/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += support/*.gcda support/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += wallet/*.gcda wallet/*.gcno
CLEANFILES += wallet/test/*.gcda wallet/test/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = $(CTAES_DIST)

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h
	-rm -rf test/__pycache__

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(AM_CXXFLAGS) $(PIE_FLAGS) $(CXXFLAGS) -c -o $@ $<

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.leveldb.include
endif

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_BENCH
include Makefile.bench.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif

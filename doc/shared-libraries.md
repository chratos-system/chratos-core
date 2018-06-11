Shared Libraries
================

## chratosconsensus

The purpose of this library is to make the verification functionality that is critical to Chratos's consensus available to other applications, e.g. to language bindings.

### API

The interface is defined in the C header `chratosconsensus.h` located in  `src/script/chratosconsensus.h`.

#### Version

`chratosconsensus_version` returns an `unsigned int` with the API version *(currently at an experimental `0`)*.

#### Script Validation

`chratosconsensus_verify_script` returns an `int` with the status of the verification. It will be `1` if the input script correctly spends the previous output `scriptPubKey`.

##### Parameters
- `const unsigned char *scriptPubKey` - The previous output script that encumbers spending.
- `unsigned int scriptPubKeyLen` - The number of bytes for the `scriptPubKey`.
- `const unsigned char *txTo` - The transaction with the input that is spending the previous output.
- `unsigned int txToLen` - The number of bytes for the `txTo`.
- `unsigned int nIn` - The index of the input in `txTo` that spends the `scriptPubKey`.
- `unsigned int flags` - The script validation flags *(see below)*.
- `chratosconsensus_error* err` - Will have the error/success code for the operation *(see below)*.

##### Script Flags
- `chratosconsensus_SCRIPT_FLAGS_VERIFY_NONE`
- `chratosconsensus_SCRIPT_FLAGS_VERIFY_P2SH` - Evaluate P2SH ([BIP16](https://github.com/chratos/bips/blob/master/bip-0016.mediawiki)) subscripts
- `chratosconsensus_SCRIPT_FLAGS_VERIFY_DERSIG` - Enforce strict DER ([BIP66](https://github.com/chratos/bips/blob/master/bip-0066.mediawiki)) compliance

##### Errors
- `chratosconsensus_ERR_OK` - No errors with input parameters *(see the return value of `chratosconsensus_verify_script` for the verification status)*
- `chratosconsensus_ERR_TX_INDEX` - An invalid index for `txTo`
- `chratosconsensus_ERR_TX_SIZE_MISMATCH` - `txToLen` did not match with the size of `txTo`
- `chratosconsensus_ERR_DESERIALIZE` - An error deserializing `txTo`

### Example Implementations
- [NChratos](https://github.com/NicolasDorier/NChratos/blob/master/NChratos/Script.cs#L814) (.NET Bindings)
- [node-libchratosconsensus](https://github.com/bitpay/node-libchratosconsensus) (Node.js Bindings)
- [java-libchratosconsensus](https://github.com/dexX7/java-libchratosconsensus) (Java Bindings)
- [chratosconsensus-php](https://github.com/Bit-Wasp/chratosconsensus-php) (PHP Bindings)

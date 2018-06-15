#pragma once

#include "secp256k1/secp256k1.h"
#include "secp256k1/secp256k1_ecdh.h"
#include "secp256k1/secp256k1_recovery.h"

#ifdef _DEBUG
#pragma comment(lib, "secp256k1_d.lib" )
#else // _DEBUG
#pragma comment(lib, "secp256k1.lib" )
#endif // _DEBUG

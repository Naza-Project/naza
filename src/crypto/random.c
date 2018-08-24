// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "hash-impl.h"
#include "initializer.h"
#include "random.h"

#if defined(_WIN32)
	#include <windows.h>
	#include <wincrypt.h>

static void generate_system_random_bytes(size_t n, void *result) {
	HCRYPTPROV prov = 0;
	if(!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT) ||
	   !CryptGenRandom(prov, (DWORD)n, result) ||
	   !CryptReleaseContext(prov, 0)){
		assert(0);
	}
}

#else

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static void generate_system_random_bytes(size_t n, void *result) {
	int fd;
	if ((fd = open("/dev/urandom", O_RDONLY | O_NOCTTY | O_CLOEXEC)) < 0) {
		err(EXIT_FAILURE, "open /dev/urandom");
	}
	for (;;) {
		ssize_t res = read(fd, result, n);
		if ((size_t) res == n) {
			break;
		}
		if (res < 0) {
			if (errno != EINTR) {
				err(EXIT_FAILURE, "read /dev/urandom");
			}
		} else if (res == 0) {
			err(EXIT_FAILURE, "read /dev/urandom: end of file");
		} else {
			result = padd(result, (size_t) res);
			n -= (size_t) res;
		}
	}
	if (close(fd) < 0) {
		err(EXIT_FAILURE, "close /dev/urandom");
	}
}

#endif

static union hash_state state;
static bool initialized = false;

void initialize_random(void){
	generate_system_random_bytes(32, &state);
	initialized = true;
}

void unsafe_generate_random_bytes(size_t n, void *result) {
	if( !initialized)
		initialize_random();
	for (;;) {
		hash_permutation(&state);
		if (n <= HASH_DATA_AREA) {
			memcpy(result, &state, n);
			return;
		}
		memcpy(result, &state, HASH_DATA_AREA);
		result = padd(result, HASH_DATA_AREA);
		n -= HASH_DATA_AREA;
	}
}

void initialize_random_for_tests(void) {
	memset(&state, 42, sizeof(union hash_state));
	initialized = true;
}

// We keep initialize@start, because generate_system_random_bytes will exit on errror
// If INITIALIZER fails to compile on your platform, just comment out 3 lines below
INITIALIZER(init_random) {
	initialize_random();
}

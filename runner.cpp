#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bnfuzz/config.h>
#include <bnfuzz/operation.h>
#include <bnfuzz/module.h>

#include "runner.h"
#include "multi.h"

bool Runner::run(Input& input) {
    uint8_t numbers[NUM_BIGNUMS][num_len];
    bool ret = false;
    bool l_compare = true;
    size_t loops = 0;

    if ( input.extract((uint8_t*)numbers, sizeof(numbers)) == false ) {
        return false;
    }

    if ( multi->initialize() == false ) {
        printf("Fatal error: cannot initialize. Exiting.\n");
        exit(0);
    }

    for (size_t i = 0; i < NUM_BIGNUMS; i++) {
        if ( multi->bignum_from_bin(numbers[i], num_len, i) == false ) {
            goto end;
        }
    }

    operation_t l_operation;
    while ( input.extract(&l_operation, sizeof(l_operation)) == true ) {
        if ( swapswapop == true && loops < 2 ) {
            l_operation = BN_FUZZ_OP_SWAP;
        } else {
            if ( operation != 0 ) {
                l_operation = (operation_t)operation;
            }
        }

        loops++;
        uint8_t opt;
        if ( input.extract(&opt, sizeof(opt)) != true ) {
            break;
        }

        if ( multi->exec_operation(l_operation, opt) == false ) {
            l_compare = false;
            break;
        }

        if ( compare == true && multi->compare() == false ) {
            abort();
        }


#if 0
        /* Swap two arbitrary bignums */
        uint8_t swap_a, swap_b;
        if ( input.extract(&swap_a, sizeof(swap_a)) != true ) {
            break;
        }
        if ( input.extract(&swap_b, sizeof(swap_b)) != true ) {
            break;
        }

        swap_a %= NUM_BIGNUMS;
        swap_b %= NUM_BIGNUMS;

        multi->swap_bignum(swap_a, swap_b);
#endif

        if ( num_loops != 0 && loops == num_loops ) {
            break;
        }
    }

    if ( l_compare == true ) {
        if ( compare == true && multi->compare() == false ) {
            abort();
        }
    }

end:
    multi->shutdown();
    return ret;
}

void Runner::SetLogging(const bool setlogging) {
    multi->SetLogging(setlogging);
}

void Runner::SetNegative(const bool setnegative) {
    multi->SetNegative(setnegative);
}

void Runner::SetCompare(const bool setcompare) {
    compare = setcompare;
}

void Runner::SetSwapSwapOp(const bool setswapswapop) {
    swapswapop = setswapswapop;
}
void Runner::SetNumberLength(const size_t _num_len) {
    num_len = _num_len;
}
void Runner::SetOperation(const size_t _operation) {
    operation = _operation;
}
void Runner::SetNumLoops(const size_t _num_loops) {
    num_loops = _num_loops;
}

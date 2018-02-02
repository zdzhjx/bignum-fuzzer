#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bnfuzz/config.h>
#include <bnfuzz/operation.h>
#include <bnfuzz/module.h>

#include "runner.h"
#include "multi.h"

bool Runner::run(void) {
    uint8_t numbers[NUM_BIGNUMS][BNSTR_LEN];
    bool ret = false;
    bool l_compare = true;
    size_t loops = 0;

    if ( input->extract((uint8_t*)numbers, sizeof(numbers)) == false ) {
        return false;
    }

    if ( multi->initialize() == false ) {
        printf("Fatal error: cannot initialize. Exiting.\n");
        exit(0);
    }

    for (size_t i = 0; i < NUM_BIGNUMS; i++) {
        if ( multi->bignum_from_bin(numbers[i], BNSTR_LEN, i) == false ) {
            goto end;
        }
    }

    operation_t operation;
    while ( input->extract(&operation, sizeof(operation)) == true ) {
        loops++;
        uint8_t opt;
        operation %= BN_FUZZ_OP_ETH_MAX;
        if ( input->extract(&opt, sizeof(opt)) != true ) {
            break;
        }

        if ( multi->exec_operation(operation, opt) == false ) {
            l_compare = false;
            break;
        }

        if ( compare == true && multi->compare() == false ) {
            abort();
        }


#if 0
        /* Swap two arbitrary bignums */
        uint8_t swap_a, swap_b;
        if ( input->extract(&swap_a, sizeof(swap_a)) != true ) {
            break;
        }
        if ( input->extract(&swap_b, sizeof(swap_b)) != true ) {
            break;
        }

        swap_a %= NUM_BIGNUMS;
        swap_b %= NUM_BIGNUMS;

        multi->swap_bignum(swap_a, swap_b);
#endif

        if ( loops == 2 ) {
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

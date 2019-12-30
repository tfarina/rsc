#ifndef BITOPS_H_
#define BITOPS_H_

/**
 * Helper macros for bit manipulation.
 */

#define BIT_MASK(bit) (1U << (bit))

/**
 * These macros work on positions defined by constants.
 *
 * Examples:
 *
 * 1- To set bit 2:
 *
 *    BIT_SET(flags, 0x04)
 */
#define BIT_SET(flags, mask)   ((flags) |= (mask))
#define BIT_CLR(flags, mask)   ((flags) &= ~(mask))

/**
 * Returns true if any of bits specified in 'mask' is set.
 */
#define BIT_ISSET(flags, mask) ((flags) & (mask))

#define BIT_TOGGLE(flags, mask) ((flags) ^= (mask))

/* https://www.pixelbeat.org/libs/bitops.h */
/* https://github.com/irrdnet/irrd/blob/03de032c2d1769a6ffb72513a03cec77d9759fc2/src/include/defs.h#L27 */
/* https://github.com/moxuse/ofxSgp/blob/74ccfa7c164226d17e287beb63152a3d84ea4e7d/libs/sgp4sdp4/sgp4sdp4.c */
/* https://github.com/acassen/keepalived/blob/7630e72ba38be27b060a9db4b01a6045ee794c66/lib/bitops.h#L37 */
/* https://people.cs.umu.se/isak/snippets/bitops.c */
/* https://github.com/royratcliffe/ta-lib/blob/272b60afa70625b84ab647fd1e0a13909735280b/branches/ta-lib/ta-lib/c/src/ta_common/imatix/sfl/prelude.h#L429 */
/* https://github.com/signalwire/libks/blob/master/src/include/libks/ks.h#L42 */

#endif  /* BITOPS_H_ */

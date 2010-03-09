/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pm.h"
#include <stdio.h>
#include <limits.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x70

/** Raise an exception if the given expression is false.
 *  This MUST be called from the C implementation of a 
 *  Python function, becuase it assumes the existance of
 *  - PmReturn_t retval
 *  \param expr Expression to evaluate.
 *  \param prReturn If expression is false, exception to raise. Must
 *                  be of type \ref PmReturn_t and typically is in the
 *                  \ref PmReturn_e enum.
 *  \param eStr Diagostic string to inclcude with the exception.
 *              Currently, not used.
 */
#define EXCEPTION_ASSERT(expr, prReturn, eStr) \
    do { \
        if (!(expr)) { \
            printf("Error: %s\n", eStr); \
            PM_RAISE(retval, prReturn); \
            return retval; \
        } \
    } while (C_FALSE)


/** Get an unsigned, 16-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pu16_val Pointer to resulting uint16 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getUint16(pPmFrame_t *ppframe, uint16_t u16_ndx, uint16_t* pu16_val) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    int32_t i32;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_ASSERT(OBJ_GET_TYPE(ppo) == OBJ_TYPE_INT, PM_RET_EX_TYPE, 
      "Expected an int");

    // Raise a ValueError if address is < 0 or > 65535
    i32 = ((pPmInt_t) ppo)->val;
    EXCEPTION_ASSERT((i32 >= 0) && (i32 <= UINT_MAX), PM_RET_EX_VAL, 
        "Value must be between 0 and 65535.");

    // Assignment is now safe
    *pu16_val = (uint16_t) i32;

    return retval;
}

/** Get a signed, 32-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pu16_val Pointer to resulting int32 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getInt32(pPmFrame_t *ppframe, uint16_t u16_ndx, int32_t* pi32_val) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_ASSERT(OBJ_GET_TYPE(ppo) == OBJ_TYPE_INT, PM_RET_EX_TYPE, 
      "Expected an int");

    // Assignment is now safe
    *pi32_val = ((pPmInt_t) ppo)->val;

    return retval;
}

/** Get a boolean value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pu16_val Pointer to resulting uint16 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getBool(pPmFrame_t *ppframe, uint16_t u16_ndx, uint_t* pu_bool) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_ASSERT(OBJ_GET_TYPE(ppo) == OBJ_TYPE_BOOL, PM_RET_EX_TYPE, 
      "Expected a bool");
    
    *pu_bool = ((pPmBoolean_t) ppo)->val ? C_TRUE : C_FALSE;

    return retval;
}

/** Macro to ease calling getXxxx functions. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_XXX(u16_ndx, val, getFunc) \
    do { \
        retval = getFunc(ppframe, u16_ndx, &val); \
        PM_RETURN_IF_ERROR(retval); \
    } while (C_FALSE)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param u16_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_UINT16(u16_ndx, u16_val) GET_XXX(u16_ndx, u16_val, getUint16)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param i32_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_INT32(u16_ndx, i32_val) GET_XXX(u16_ndx, i32_val, getInt32)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param u16_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_BOOL(u16_ndx, u_bool) GET_XXX(u16_ndx, u_bool, getBool)

/** Check the number of arguments passed to a Python function.
 *  Report an exception if the number is incorrect. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  @param numArgs Number of arguemnts expected.
 */
#define CHECK_NUM_ARGS(numArgs) \
    EXCEPTION_ASSERT(NATIVE_GET_NUM_ARGS() == numArgs, PM_RET_EX_TYPE, \
      "Incorrect number of arguments.")

/** Implements the Python \ref readBits function. See it for details. */
PmReturn_t readBitsC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    uint16_t u16;
    uint16_t* pu16_evenAddress;
    uint16_t u16_startBit;
    uint16_t u16_numBits;
    uint16_t u16_bitmask;
    uint16_t u16_value;

    // Get the arguments
    CHECK_NUM_ARGS(3);
    GET_UINT16(0, u16);
    pu16_evenAddress = (uint16_t*) u16;
    GET_UINT16(1, u16_startBit);
    GET_UINT16(2, u16_numBits);

    // Check their values.
    /// \todo Allow an odd addres for 8 bit values.
    EXCEPTION_ASSERT( !(((uint16_t) pu16_evenAddress) & 1), PM_RET_EX_VAL,
      "The address must be even.");
    EXCEPTION_ASSERT(u16_startBit <= 15, PM_RET_EX_VAL,
      "The start bit must be <= 15.");
    EXCEPTION_ASSERT(u16_numBits > 0, PM_RET_EX_VAL,
      "The number of bits must be > 0.");
    EXCEPTION_ASSERT(u16_startBit + u16_numBits <= 16, PM_RET_EX_VAL,
      "start bit + num bits <= 16.");

    // Form the bitmask
    u16_bitmask = (1 << u16_numBits) - 1;
    // Read the port and mask
    u16_value = (*pu16_evenAddress >> u16_numBits) & u16_bitmask;
    printf("Value at 0x%04X, bit(s) %d to %d = 0x%02X.\n", (uint16_t) pu16_evenAddress, 
      u16_startBit, u16_startBit + u16_numBits - 1, u16_value);

    retval = int_new(u16_value, &ppo);
    PM_RETURN_IF_ERROR(retval);
    NATIVE_SET_TOS(ppo);

    return retval;
}

/** Look up a specific bit in a bitfield.
 *  @param u16_bitfeld Bitfield to access.
 *  @param u_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @return C_TRUE if the bit is a 1, C_FALSE otherwise.
 */
uint_t getBit(uint16_t u16_bitfield, uint_t u_bit)
{
    ASSERT(u_bit < 16);
    return (u16_bitfield & (1 << u_bit)) ? C_TRUE : C_FALSE;
}

/** Set a specific bit in a bitfield.
 *  @param u16_bitfeld Bitfield to access.
 *  @param u_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @param u_value True to set the bit, false to clear it.
 */
void setBit(volatile uint16_t* pu16_bitfield, uint_t u_bit, uint_t u_val)
{
    ASSERT(u_bit < 16);
    if (u_val)
        *pu16_bitfield |= 1 << u_bit;
    else
        *pu16_bitfield &= ~(1 << u_bit);
}

/** Determine if the given digitial I/O port/pin exists.
 *  @param u_port Port, where 0 = A, 1 = B, etc.
 *  @param u_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_TRUE if the port/pin exists, C_FALSE otherwise.
 *          Nonexistant ports or pins simply return C_FALSE.
 */
static uint_t digitalPortPinExists(uint_t u_port, uint_t u_pin)
{
    // Check for an out-of-range port
    if (u_port > NUM_DIGITAL_PORTS)
        return C_FALSE;
    // Check for an out-of-range pin
    if (u_pin > 15)
        return C_FALSE;
    // Otherwise, check the map of pins.
    return getBit(u16_digitalPinPresent[u_port], u_pin);
}


/** Determine if the given digitial I/O open-drain port/pin exists.
 *  @param u_port Port, where 0 = A, 1 = B, etc.
 *  @param u_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_TRUE if the port/pin open-drain exists, C_FALSE otherwise.
 *          Nonexistant ports or pins simply return C_FALSE.
 */
static uint_t digitalPortPinOpenDrainExists(uint_t u_port, uint_t u_pin)
{
    // Check for an out-of-range port
    if (u_port > NUM_DIGITAL_PORTS)
        return C_FALSE;
    // Check for an out-of-range pin
    if (u_pin > 15)
        return C_FALSE;
    // Otherwise, check the map of pins.
    return getBit(u16_digitalPinOpenDrainPresent[u_port], u_pin);
}


/** Implements the Python \ref configDigitalPin function. 
 *  See it for details. Implementation:
 *  -# Check to see if the port/pin exists.
 *  -# If the pin has analog capability, turn it off.
 *  -# Select the pin to be either an input or an output.
 *  -# Check and configure open-drain for the pin.
 *  -# Check and configure pull-ups/pull-downs for the pin.
 *  \todo Need to also remove any peripheral outputs mapped to
 *  this pin if it's a remappable pin.
 */
PmReturn_t configDigitalPinC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    uint_t u_isInput;
    uint_t u_isOpenDrain;
    uint16 u16_offset;
    int32_t i32_pullDir;

    // Get the arguments
    u16_port = 0;
    CHECK_NUM_ARGS(5);
    GET_UINT16(0, u16_port);
    GET_UINT16(1, u16_pin);
    GET_BOOL(2, u_isInput);
    GET_BOOL(3, u_isOpenDrain);
    GET_INT32(4, i32_pullDir);

    // 1. Check to see if the port/pin exists.
    EXCEPTION_ASSERT(digitalPortPinExists(u16_port, u16_pin), PM_RET_EX_VAL,
      "Invalid port or pin.");

    // 3. Select the pin to be either an input or an output.
    u16_offset = (uint16) (&TRISB - &TRISA);
    setBit((&TRISA) + u16_port*u16_offset, u16_pin, u_isInput);

    // 4. Check and configure open-drain for the pin.
    // First, see if the pin exists.
    if (digitalPortPinExists(u16_port, u16_pin)) {
        // Set the pin per the OD boolean.
        // PIC24F names this differently, so define around it.
        #ifdef ODA
        #define ODCA ODA
        #endif
        setBit((&ODCA) + u16_port*u16_offset, u16_pin, u_isOpenDrain);
    } else {
        // If open-drain is enabled on a pin without OD ability,
        // report an error. Otherwise, do nothing -- open-drain
        // is already disabled for a pin without OD ability.
        EXCEPTION_ASSERT(!u_isOpenDrain, PM_RET_EX_VAL,
          "The port/pin selected has no open-drain ability.");
    }

    return retval;
}

/**
 * dwarf_eh.h - Defines some helper functions for parsing DWARF exception
 * handling tables.
 *
 * This file contains various helper functions that are independent of the
 * language-specific code.  It can be used in any personality function for the
 * Itanium ABI.
 */
#include <assert.h>

// TODO: Factor out Itanium / ARM differences.  We probably want an itanium.h
// and arm.h that can be included by this file depending on the target ABI.

// _GNU_SOURCE must be defined for unwind.h to expose some of the functions
// that we want.  If it isn't, then we define it and undefine it to make sure
// that it doesn't impact the rest of the program.
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE 1
#	include <unwind.h>
#	undef _GNU_SOURCE
#else
#	include <unwind.h>
#endif

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/// Type used for pointers into DWARF data
typedef unsigned char *dw_eh_ptr_t;

// Flag indicating a signed quantity
#define DW_EH_PE_signed 0x08
/// DWARF data encoding types.  
enum dwarf_data_encoding
{
	/// pointer
	DW_EH_PE_ptr   = 0x00,
	/// Unsigned, little-endian, base 128-encoded (variable length).
	DW_EH_PE_uleb128 = 0x01,
	/// Unsigned 16-bit integer.
	DW_EH_PE_udata2  = 0x02,
	/// Unsigned 32-bit integer.
	DW_EH_PE_udata4  = 0x03,
	/// Unsigned 64-bit integer.
	DW_EH_PE_udata8  = 0x04,
	/// Signed, little-endian, base 128-encoded (variable length)
	DW_EH_PE_sleb128 = DW_EH_PE_uleb128 | DW_EH_PE_signed,
	/// Signed 16-bit integer.
	DW_EH_PE_sdata2  = DW_EH_PE_udata2 | DW_EH_PE_signed,
	/// Signed 32-bit integer.
	DW_EH_PE_sdata4  = DW_EH_PE_udata4 | DW_EH_PE_signed,
	/// Signed 32-bit integer.
	DW_EH_PE_sdata8  = DW_EH_PE_udata8 | DW_EH_PE_signed
};

/**
 * Returns the encoding for a DWARF EH table entry.  The encoding is stored in
 * the low four of an octet.  The high four bits store the addressing mode.
 */
static inline enum dwarf_data_encoding get_encoding(unsigned char x)
{
	return (enum dwarf_data_encoding)(x & 0xf);
}

/**
 * DWARF addressing mode constants.  When reading a pointer value from a DWARF
 * exception table, you must know how it is stored and what the addressing mode
 * is.  The low four bits tell you the encoding, allowing you to decode a
 * number.  The high four bits tell you the addressing mode, allowing you to
 * turn that number into an address in memory.
 */
enum dwarf_data_relative
{
	/// Value is omitted
	DW_EH_PE_omit     = 0xff,
	/// Absolute pointer value
	DW_EH_PE_absptr   = 0x00,
	/// Value relative to program counter
	DW_EH_PE_pcrel    = 0x10,
	/// Value relative to the text segment
	DW_EH_PE_textrel  = 0x20,
	/// Value relative to the data segment
	DW_EH_PE_datarel  = 0x30,
	/// Value relative to the start of the function
	DW_EH_PE_funcrel  = 0x40,
	/// Aligned pointer (Not supported yet - are they actually used?)
	DW_EH_PE_aligned  = 0x50,
	/// Pointer points to address of real value
	DW_EH_PE_indirect = 0x80
};
/**
 * Returns the addressing mode component of this encoding.
 */
static inline enum dwarf_data_relative get_base(unsigned char x)
{
	return (enum dwarf_data_relative)(x & 0x70);
}
/**
 * Returns whether an encoding represents an indirect address.
 */
static int is_indirect(unsigned char x)
{
	return ((x & DW_EH_PE_indirect) == DW_EH_PE_indirect);
}

/**
 * Returns the size of a fixed-size encoding.  This function will abort if
 * called with a value that is not a fixed-size encoding.
 */
static inline int dwarf_size_of_fixed_size_field(unsigned char type)
{
	switch (get_encoding(type))
	{
		default: abort();
		case DW_EH_PE_sdata2: 
		case DW_EH_PE_udata2: return 2;
		case DW_EH_PE_sdata4:
		case DW_EH_PE_udata4: return 4;
		case DW_EH_PE_sdata8:
		case DW_EH_PE_udata8: return 8;
		case DW_EH_PE_ptr: return sizeof(void*);
	}
}

/** 
 * Read an unsigned, little-endian, base-128, DWARF value.  Updates *data to
 * point to the end of the value.  Stores the number of bits read in the value
 * pointed to by b, allowing you to determine the value of the highest bit, and
 * therefore the sign of a signed value.
 *
 * This function is not intended to be called directly.  Use read_sleb128() or
 * read_uleb128() for reading signed and unsigned versions, respectively.
 */
static uint64_t read_leb128(dw_eh_ptr_t *data, int *b)
{
	uint64_t uleb = 0;
	unsigned int bit = 0;
	unsigned char digit = 0;
	// We have to read at least one octet, and keep reading until we get to one
	// with the high bit unset
	do
	{
		// This check is a bit too strict - we should also check the highest
		// bit of the digit.
		assert(bit < sizeof(uint64_t) * 8);
		// Get the base 128 digit 
		digit = (**data) & 0x7f;
		// Add it to the current value
		uleb += digit << bit;
		// Increase the shift value
		bit += 7;
		// Proceed to the next octet
		(*data)++;
		// Terminate when we reach a value that does not have the high bit set
		// (i.e. which was not modified when we mask it with 0x7f)
	} while ((*(*data - 1)) != digit);
	*b = bit;

	return uleb;
}

/**
 * Reads an unsigned little-endian base-128 value starting at the address
 * pointed to by *data.  Updates *data to point to the next byte after the end
 * of the variable-length value.
 */
static int64_t read_uleb128(dw_eh_ptr_t *data)
{
	int b;
	return read_leb128(data, &b);
}

/**
 * Reads a signed little-endian base-128 value starting at the address pointed
 * to by *data.  Updates *data to point to the next byte after the end of the
 * variable-length value.
 */
static int64_t read_sleb128(dw_eh_ptr_t *data)
{
	int bits;
	// Read as if it's signed
	uint64_t uleb = read_leb128(data, &bits);
	// If the most significant bit read is 1, then we need to sign extend it
	if ((uleb >> (bits-1)) == 1)
	{
		// Sign extend by setting all bits in front of it to 1
		uleb |= ((int64_t)-1) << bits;
	}
	return (int64_t)uleb;
}
/**
 * Reads a value using the specified encoding from the address pointed to by
 * *data.  Updates the value of *data to point to the next byte after the end
 * of the data.
 */
static uint64_t read_value(char encoding, dw_eh_ptr_t *data)
{
	enum dwarf_data_encoding type = get_encoding(encoding);
	uint64_t v;
	switch (type)
	{
		// Read fixed-length types
#define READ(dwarf, type) \
		case dwarf:\
			v = (uint64_t)(*(type*)(*data));\
			*data += sizeof(type);\
			break;
		READ(DW_EH_PE_udata2, uint16_t)
		READ(DW_EH_PE_udata4, uint32_t)
		READ(DW_EH_PE_udata8, uint64_t)
		READ(DW_EH_PE_sdata2, int16_t)
		READ(DW_EH_PE_sdata4, int32_t)
		READ(DW_EH_PE_sdata8, int64_t)
		READ(DW_EH_PE_absptr, intptr_t)
#undef READ
		// Read variable-length types
		case DW_EH_PE_sleb128:
			v = read_sleb128(data);
			break;
		case DW_EH_PE_uleb128:
			v = read_uleb128(data);
			break;
		default: abort();
	}

	return v;
}

/**
 * Resolves an indirect value.  This expects an unwind context, an encoding, a
 * decoded value, and the start of the region as arguments.  The returned value
 * is a pointer to the address identified by the encoded value.
 *
 * If the encoding does not specify an indirect value, then this returns v.
 */
static uint64_t resolve_indirect_value(_Unwind_Context *c,
                                       unsigned char encoding,
                                       int64_t v,
                                       dw_eh_ptr_t start)
{
	switch (get_base(encoding))
	{
		case DW_EH_PE_pcrel:
			v += (uint64_t)start;
			break;
		case DW_EH_PE_textrel:
			v += (uint64_t)_Unwind_GetTextRelBase(c);
			break;
		case DW_EH_PE_datarel:
			v += (uint64_t)_Unwind_GetDataRelBase(c);
			break;
		case DW_EH_PE_funcrel:
			v += (uint64_t)_Unwind_GetRegionStart(c);
		default:
			break;
	}
	// If this is an indirect value, then it is really the address of the real
	// value
	// TODO: Check whether this should really always be a pointer - it seems to
	// be a GCC extensions, so not properly documented...
	if (is_indirect(encoding))
	{
		v = (uint64_t)(uintptr_t)*(void**)v;
	}
	return v;
}


/**
 * Reads an encoding and a value, updating *data to point to the next byte.  
 */
static inline void read_value_with_encoding(_Unwind_Context *context,
                                            dw_eh_ptr_t *data,
                                            uint64_t *out)
{
	dw_eh_ptr_t start = *data;
	unsigned char encoding = *((*data)++);
	// If this value is omitted, skip it and don't touch the output value
	if (encoding == DW_EH_PE_omit) { return; }

	*out = read_value(encoding, data);
	*out = resolve_indirect_value(context, encoding, *out, start);
}

/**
 * Structure storing a decoded language-specific data area.  Use parse_lsda()
 * to generate an instance of this structure from the address returned by the
 * generic unwind library.  
 *
 * You should not need to inspect the fields of this structure directly if you
 * are just using this header.  The structure stores the locations of the
 * various tables used for unwinding exceptions and is used by the functions
 * for reading values from these tables.
 */
struct dwarf_eh_lsda
{
	/// The start of the region.  This is a cache of the value returned by
	/// _Unwind_GetRegionStart().
	dw_eh_ptr_t region_start;
	/// The start of the landing pads table.
	dw_eh_ptr_t landing_pads;
	/// The start of the type table.
	dw_eh_ptr_t type_table;
	/// The encoding used for entries in the type tables.
	unsigned char type_table_encoding;
	/// The location of the call-site table.
	dw_eh_ptr_t call_site_table;
	/// The location of the action table.
	dw_eh_ptr_t action_table;
	/// The encoding used for entries in the call-site table.
	unsigned char callsite_encoding;
};

/**
 * Parse the header on the language-specific data area and return a structure
 * containing the addresses and encodings of the various tables.
 */
static inline struct dwarf_eh_lsda parse_lsda(_Unwind_Context *context,
                                              unsigned char *data)
{
	struct dwarf_eh_lsda lsda;

	lsda.region_start = (dw_eh_ptr_t)(uintptr_t)_Unwind_GetRegionStart(context);

	// If the landing pads are relative to anything other than the start of
	// this region, find out where.  This is @LPStart in the spec, although the
	// encoding that GCC uses does not quite match the spec.
	uint64_t v = (uint64_t)(uintptr_t)lsda.region_start;
	read_value_with_encoding(context, &data, &v);
	lsda.landing_pads = (dw_eh_ptr_t)(uintptr_t)v;

	// If there is a type table, find out where it is.  This is @TTBase in the
	// spec.  Note: we find whether there is a type table pointer by checking
	// whether the leading byte is DW_EH_PE_omit (0xff), which is not what the
	// spec says, but does seem to be how G++ indicates this.
	lsda.type_table = 0;
	lsda.type_table_encoding = *data++;
	if (lsda.type_table_encoding != DW_EH_PE_omit)
	{
		v = read_uleb128(&data);
		dw_eh_ptr_t type_table = data;
		type_table += v;
		lsda.type_table = type_table;
		//lsda.type_table = (uintptr_t*)(data + v);
	}

	lsda.callsite_encoding = (enum dwarf_data_encoding)(*(data++));

	// Action table is immediately after the call site table
	lsda.action_table = data;
	uintptr_t callsite_size = (uintptr_t)read_uleb128(&data);
	lsda.action_table = data + callsite_size;
	// Call site table is immediately after the header
	lsda.call_site_table = (dw_eh_ptr_t)data;


	return lsda;
}

/**
 * Structure representing an action to be performed while unwinding.  This
 * contains the address that should be unwound to and the action record that
 * provoked this action.
 */
struct dwarf_eh_action
{
	/** 
	 * The address that this action directs should be the new program counter
	 * value after unwinding.
	 */
	dw_eh_ptr_t landing_pad;
	/// The address of the action record.
	dw_eh_ptr_t action_record;
};

/**
 * Look up the landing pad that corresponds to the current invoke.
 * Returns true if record exists.  The context is provided by the generic
 * unwind library and the lsda should be the result of a call to parse_lsda().
 *
 * The action record is returned via the result parameter.  
 */
static bool dwarf_eh_find_callsite(struct _Unwind_Context *context,
                                   struct dwarf_eh_lsda *lsda,
                                   struct dwarf_eh_action *result)
{
	result->action_record = 0;
	result->landing_pad = 0;
	// The current instruction pointer offset within the region
	uint64_t ip = _Unwind_GetIP(context) - _Unwind_GetRegionStart(context);
	unsigned char *callsite_table = (unsigned char*)lsda->call_site_table;

	while (callsite_table <= lsda->action_table)
	{
		// Once again, the layout deviates from the spec.
		uint64_t call_site_start, call_site_size, landing_pad, action;
		call_site_start = read_value(lsda->callsite_encoding, &callsite_table);
		call_site_size = read_value(lsda->callsite_encoding, &callsite_table);

		// Call site entries are sorted, so if we find a call site that's after
		// the current instruction pointer then there is no action associated
		// with this call and we should unwind straight through this frame
		// without doing anything.
		if (call_site_start > ip) { break; }

		// Read the address of the landing pad and the action from the call
		// site table.
		landing_pad = read_value(lsda->callsite_encoding, &callsite_table);
		action = read_uleb128(&callsite_table);

		// We should not include the call_site_start (beginning of the region)
		// address in the ip range. For each call site:
		//
		// address1: call proc
		// address2: next instruction
		//
		// The call stack contains address2 and not address1, address1 can be
		// at the end of another EH region.
		if (call_site_start < ip && ip <= call_site_start + call_site_size)
		{
			if (action)
			{
				// Action records are 1-biased so both no-record and zeroth
				// record can be stored.
				result->action_record = lsda->action_table + action - 1;
			}
			// No landing pad means keep unwinding.
			if (landing_pad)
			{
				// Landing pad is the offset from the value in the header
				result->landing_pad = lsda->landing_pads + landing_pad;
			}
			return true;
		}
	}
	return false;
}

/// Defines an exception class from 8 bytes (endian independent)
#define EXCEPTION_CLASS(a,b,c,d,e,f,g,h) \
	(((uint64_t)a << 56) +\
	 ((uint64_t)b << 48) +\
	 ((uint64_t)c << 40) +\
	 ((uint64_t)d << 32) +\
	 ((uint64_t)e << 24) +\
	 ((uint64_t)f << 16) +\
	 ((uint64_t)g << 8) +\
	 ((uint64_t)h))

#define GENERIC_EXCEPTION_CLASS(e,f,g,h) \
	 ((uint32_t)e << 24) +\
	 ((uint32_t)f << 16) +\
	 ((uint32_t)g << 8) +\
	 ((uint32_t)h)

#ifdef	__cplusplus
}
#endif

/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "vterm_keycodes.h"

#define TRUE 1
#define FALSE 0

// from stdint.h
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;

#define VTERM_VERSION_MAJOR 0
#define VTERM_VERSION_MINOR 2

#define VTERM_CHECK_VERSION \
        vterm_check_version(VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR)

typedef struct VTerm VTerm;
typedef struct VTermState VTermState;
typedef struct VTermScreen VTermScreen;

// Specifies a screen point.
typedef struct {
  int row;
  int col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
int vterm_pos_cmp(VTermPos a, VTermPos b);

#if defined(DEFINE_INLINES) || USE_INLINE
inline int vterm_pos_cmp(VTermPos a, VTermPos b)
{
  return (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
typedef struct {
  int start_row;
  int end_row;
  int start_col;
  int end_col;
} VTermRect;

/* true if the rect contains the point */
int vterm_rect_contains(VTermRect r, VTermPos p);

#if defined(DEFINE_INLINES) || USE_INLINE
inline int vterm_rect_contains(VTermRect r, VTermPos p)
{
  return p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
void vterm_rect_move(VTermRect *rect, int row_delta, int col_delta);

#if defined(DEFINE_INLINES) || USE_INLINE
inline void vterm_rect_move(VTermRect *rect, int row_delta, int col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
typedef enum {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_INDEXED = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_MASK = 0x01,

  /**
   * If set, indicates that this colour should be the default foreground
   * color, i.e. there was no SGR request for another colour. When
   * rendering this colour it is possible to ignore "idx" and just use a
   * colour that is not in the palette.
   */
  VTERM_COLOR_DEFAULT_FG = 0x02,

  /**
   * If set, indicates that this colour should be the default background
   * color, i.e. there was no SGR request for another colour. A common
   * option when rendering this colour is to not render a background at
   * all, for example by rendering the window transparently at this spot.
   */
  VTERM_COLOR_DEFAULT_BG = 0x04,

  /**
   * Mask that can be used to extract the default foreground/background bit.
   */
  VTERM_COLOR_DEFAULT_MASK = 0x06,

  /**
   * If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#define VTERM_COLOR_IS_INDEXED(col) \
  (((col)->type & VTERM_COLOR_TYPE_MASK) == VTERM_COLOR_INDEXED)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#define VTERM_COLOR_IS_RGB(col) \
  (((col)->type & VTERM_COLOR_TYPE_MASK) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#define VTERM_COLOR_IS_DEFAULT_FG(col) \
  (!!((col)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#define VTERM_COLOR_IS_DEFAULT_BG(col) \
  (!!((col)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#define VTERM_COLOR_IS_INVALID(col) (!!((col)->type & VTERM_COLOR_INVALID))

typedef struct {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t index;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
void vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
void vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
int vterm_color_is_equal(const VTermColor *a, const VTermColor *b);

typedef enum {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTermValueType;

typedef struct {
  const char *str;
  size_t      len : 30;
  unsigned int  initial : 1;
  unsigned int  final : 1;
} VTermStringFragment;

typedef union {
  int boolean;
  int number;
  VTermStringFragment string;
  VTermColor color;
} VTermValue;

typedef enum {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_REVERSE,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107

  VTERM_N_ATTRS
} VTermAttr;

typedef enum {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       // number
  VTERM_PROP_MOUSE,             // number
  VTERM_PROP_CURSORCOLOR,       // string

  VTERM_N_PROPS
} VTermProp;

enum {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_LEFT,

  VTERM_N_PROP_CURSORSHAPES
};

enum {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

typedef enum {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

typedef struct {
  const uint32_t *chars;
  int             width;
  unsigned int    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned int    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned int    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

typedef struct {
  unsigned int    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned int    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned int    continuation:1;    /* Line is a flow continuation of the previous */
} VTermLineInfo;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
typedef struct {
  VTermPos pos;                /* current cursor position */
} VTermStateFields;

typedef struct {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  void *(*malloc)(size_t size, void *allocdata);
  void  (*free)(void *ptr, void *allocdata);
} VTermAllocatorFunctions;

void vterm_check_version(int major, int minor);

// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(int rows, int cols);

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(int rows, int cols, VTermAllocatorFunctions *funcs, void *allocdata);

// Free and cleanup a terminal and all its data.
void   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
void vterm_get_size(const VTerm *vt, int *rowsp, int *colsp);

void vterm_set_size(VTerm *vt, int rows, int cols);

int  vterm_get_utf8(const VTerm *vt);
void vterm_set_utf8(VTerm *vt, int is_utf8);

size_t vterm_input_write(VTerm *vt, const char *bytes, size_t len);

/* Setting output callback will override the buffer logic */
typedef void VTermOutputCallback(const char *s, size_t len, void *user);
void vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, void *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(const VTerm *vt);
size_t vterm_output_get_buffer_current(const VTerm *vt);
size_t vterm_output_get_buffer_remaining(const VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, char *buffer, size_t len);

int vterm_is_modify_other_keys(VTerm *vt);
void vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
void vterm_keyboard_key(VTerm *vt, VTermKey key, VTermModifier mod);

void vterm_keyboard_start_paste(VTerm *vt);
void vterm_keyboard_end_paste(VTerm *vt);

void vterm_mouse_move(VTerm *vt, int row, int col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
void vterm_mouse_button(VTerm *vt, int button, int pressed, VTermModifier mod);

// ------------
// Parser layer
// ------------

/* Flag to indicate non-final subparameters in a single CSI parameter.
 * Consider
 *   CSI 1;2:3:4;5a
 * 1 4 and 5 are final.
 * 2 and 3 are non-final and will have this bit set
 *
 * Don't confuse this with the final byte of the CSI escape; 'a' in this case.
 */
#define CSI_ARG_FLAG_MORE (1U<<31)
#define CSI_ARG_MASK      (~(1U<<31))

#define CSI_ARG_HAS_MORE(a) ((a) & CSI_ARG_FLAG_MORE)
#define CSI_ARG(a)          ((a) & CSI_ARG_MASK)

/* Can't use -1 to indicate a missing argument; use this instead */
#define CSI_ARG_MISSING ((1<<30)-1)

#define CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#define CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#define CSI_ARG_COUNT(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

typedef struct {
  int (*text)(const char *bytes, size_t len, void *user);
  int (*control)(unsigned char control, void *user);
  int (*escape)(const char *bytes, size_t len, void *user);
  int (*csi)(const char *leader, const long args[], int argcount, const char *intermed, char command, void *user);
  int (*osc)(int command, VTermStringFragment frag, void *user);
  int (*dcs)(const char *command, size_t commandlen, VTermStringFragment frag, void *user);
  int (*apc)(VTermStringFragment frag, void *user);
  int (*pm)(VTermStringFragment frag, void *user);
  int (*sos)(VTermStringFragment frag, void *user);
  int (*resize)(int rows, int cols, void *user);
} VTermParserCallbacks;

void  vterm_parser_set_callbacks(VTerm *vt, const VTermParserCallbacks *callbacks, void *user);
void *vterm_parser_get_cbdata(VTerm *vt);

// -----------
// State layer
// -----------

typedef struct {
  int (*putglyph)(VTermGlyphInfo *info, VTermPos pos, void *user);
  int (*movecursor)(VTermPos pos, VTermPos oldpos, int visible, void *user);
  int (*scrollrect)(VTermRect rect, int downward, int rightward, void *user);
  int (*moverect)(VTermRect dest, VTermRect src, void *user);
  int (*erase)(VTermRect rect, int selective, void *user);
  int (*initpen)(void *user);
  int (*setpenattr)(VTermAttr attr, VTermValue *val, void *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  int (*settermprop)(VTermProp prop, VTermValue *val, void *user);
  int (*bell)(void *user);
  int (*resize)(int rows, int cols, VTermStateFields *fields, void *user);
  int (*setlineinfo)(int row, const VTermLineInfo *newinfo, const VTermLineInfo *oldinfo, void *user);
} VTermStateCallbacks;

typedef struct {
  VTermPos pos;
  int	   buttons;
#define MOUSE_BUTTON_LEFT 0x01
#define MOUSE_BUTTON_MIDDLE 0x02
#define MOUSE_BUTTON_RIGHT 0x04
  int      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

typedef struct {
  int (*control)(unsigned char control, void *user);
  int (*csi)(const char *leader, const long args[], int argcount, const char *intermed, char command, void *user);
  int (*osc)(int command, VTermStringFragment frag, void *user);
  int (*dcs)(const char *command, size_t commandlen, VTermStringFragment frag, void *user);
  int (*apc)(VTermStringFragment frag, void *user);
  int (*pm)(VTermStringFragment frag, void *user);
  int (*sos)(VTermStringFragment frag, void *user);
} VTermStateFallbacks;

typedef struct {
  int (*set)(VTermSelectionMask mask, VTermStringFragment frag, void *user);
  int (*query)(VTermSelectionMask mask, void *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

void  vterm_state_set_callbacks(VTermState *state, const VTermStateCallbacks *callbacks, void *user);
void *vterm_state_get_cbdata(VTermState *state);

void  vterm_state_set_unrecognised_fallbacks(VTermState *state, const VTermStateFallbacks *fallbacks, void *user);
void *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
void vterm_state_reset(VTermState *state, int hard);

void vterm_state_get_cursorpos(const VTermState *state, VTermPos *cursorpos);
void vterm_state_get_mousestate(const VTermState *state, VTermMouseState *mousestate);
void vterm_state_get_default_colors(const VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
void vterm_state_get_palette_color(const VTermState *state, int index, VTermColor *col);
void vterm_state_set_default_colors(VTermState *state, const VTermColor *default_fg, const VTermColor *default_bg);
void vterm_state_set_palette_color(VTermState *state, int index, const VTermColor *col);
void vterm_state_set_bold_highbright(VTermState *state, int bold_is_highbright);
int  vterm_state_get_penattr(const VTermState *state, VTermAttr attr, VTermValue *val);
int  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTermValue *val);
void vterm_state_focus_in(VTermState *state);
void vterm_state_focus_out(VTermState *state);
const VTermLineInfo *vterm_state_get_lineinfo(const VTermState *state, int row);

/**
 * Makes sure that the given color `col` is indeed an RGB colour. After this
 * function returns, VTERM_COLOR_IS_RGB(col) will return true, while all other
 * flags stored in `col->type` will have been reset.
 *
 * @param state is the VTermState instance from which the colour palette should
 * be extracted.
 * @param col is a pointer at the VTermColor instance that should be converted
 * to an RGB colour.
 */
void vterm_state_convert_color_to_rgb(const VTermState *state, VTermColor *col);

void vterm_state_set_selection_callbacks(VTermState *state, const VTermSelectionCallbacks *callbacks, void *user,
    char *buffer, size_t buflen);

void vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

typedef struct {
    unsigned int bold      : 1;
    unsigned int underline : 2;
    unsigned int italic    : 1;
    unsigned int blink     : 1;
    unsigned int reverse   : 1;
    unsigned int conceal   : 1;
    unsigned int strike    : 1;
    unsigned int font      : 4; /* 0 to 9 */
    unsigned int dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned int dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
} VTermScreenCellAttrs;

enum {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

typedef struct {
#define VTERM_MAX_CHARS_PER_CELL 6
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  char     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
typedef struct {
  int (*damage)(VTermRect rect, void *user);
  int (*moverect)(VTermRect dest, VTermRect src, void *user);
  int (*movecursor)(VTermPos pos, VTermPos oldpos, int visible, void *user);
  int (*settermprop)(VTermProp prop, VTermValue *val, void *user);
  int (*bell)(void *user);
  int (*resize)(int rows, int cols, void *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  int (*sb_pushline)(int cols, const VTermScreenCell *cells, void *user);
  int (*sb_popline)(int cols, VTermScreenCell *cells, void *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
void  vterm_screen_set_callbacks(VTermScreen *screen, const VTermScreenCallbacks *callbacks, void *user);
void *vterm_screen_get_cbdata(VTermScreen *screen);

void  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, const VTermStateFallbacks *fallbacks, void *user);
void *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
void vterm_screen_enable_altscreen(VTermScreen *screen, int altscreen);

typedef enum {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
void vterm_screen_flush_damage(VTermScreen *screen);

void vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
void   vterm_screen_reset(VTermScreen *screen, int hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(const VTermScreen *screen, uint32_t *chars, size_t len, const VTermRect rect);
size_t vterm_screen_get_text(const VTermScreen *screen, char *str, size_t len, const VTermRect rect);

typedef enum {
  VTERM_ATTR_BOLD_MASK       = 1 << 0,
  VTERM_ATTR_UNDERLINE_MASK  = 1 << 1,
  VTERM_ATTR_ITALIC_MASK     = 1 << 2,
  VTERM_ATTR_BLINK_MASK      = 1 << 3,
  VTERM_ATTR_REVERSE_MASK    = 1 << 4,
  VTERM_ATTR_STRIKE_MASK     = 1 << 5,
  VTERM_ATTR_FONT_MASK       = 1 << 6,
  VTERM_ATTR_FOREGROUND_MASK = 1 << 7,
  VTERM_ATTR_BACKGROUND_MASK = 1 << 8,
  VTERM_ATTR_CONCEAL_MASK    = 1 << 9,

  VTERM_ALL_ATTRS_MASK = (1 << 10) - 1
} VTermAttrMask;

int vterm_screen_get_attrs_extent(const VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

int vterm_screen_get_cell(const VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

int vterm_screen_is_eol(const VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
void vterm_screen_convert_color_to_rgb(const VTermScreen *screen, VTermColor *col);

// ---------
// Utilities
// ---------

VTermValueType vterm_get_attr_type(VTermAttr attr);
VTermValueType vterm_get_prop_type(VTermProp prop);

void vterm_scroll_rect(VTermRect rect,
                       int downward,
                       int rightward,
                       int (*moverect)(VTermRect src, VTermRect dest, void *user),
                       int (*eraserect)(VTermRect rect, int selective, void *user),
                       void *user);

void vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      void (*copycell)(VTermPos dest, VTermPos src, void *user),
                      void *user);

#ifdef __cplusplus
}
#endif

#endif

/*
AppleWin : An Apple //e emulator for Windows

Copyright (C) 2010-2011, William S Simms

AppleWin is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

AppleWin is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AppleWin; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "StdAfx.h"
#include "NTSC_CharSet.h"

unsigned char csbits_enhanced2e[2][256][8];	// Enhanced //e
unsigned char csbits_2e[2][256][8];			// Original //e (no mousetext)
unsigned char csbits_a2[1][256][8];			// ][ and ][+
unsigned char csbits_pravets82[1][256][8];	// Pravets 82
unsigned char csbits_pravets8M[1][256][8];	// Pravets 8M
unsigned char csbits_pravets8C[2][256][8];	// Pravets 8A & 8C

#if 1

static const UINT bitmapWidth = 256;
static const UINT bitmapWidthBytes = bitmapWidth/8;
static const UINT bitmapHeight = 768;

static const UINT charWidth = 16;
static const UINT charWidthBytes = 16/8;
static const UINT charHeight = 16;

void get_csbits_xy (csbits_t csbits, UINT ch, UINT cx, UINT cy, const BYTE* pBitmap)
{
	_ASSERT(ch < 256);
	_ASSERT((cx < bitmapWidth/charWidth) && (cy < bitmapHeight/charHeight));

	pBitmap += cy*charHeight*bitmapWidthBytes + cx*charWidthBytes;

	for (UINT y=0; y<8; y++)
	{
		BYTE n = 0;
		for (int x=0; x<14; x+=2)
		{
			UINT xp = x/8;
			BYTE d = pBitmap[xp];
			UINT b = 7 - x%8;
			if (d & (1<<b)) n |= 0x80;
			n >>= 1;
		}

		csbits[0][ch][y] = n;
		pBitmap += bitmapWidthBytes*2;
	}
}

void get_csbits (csbits_t csbits, const char* resourceName, const UINT cy0)
{
	const UINT bufferSize = bitmapWidthBytes*bitmapHeight;
	BYTE* pBuffer = new BYTE [bufferSize];

	HBITMAP hCharBitmap = LoadBitmap(g_hInstance, resourceName);
	GetBitmapBits(hCharBitmap, bufferSize, pBuffer);

	for (UINT cy=cy0, ch=0; cy<cy0+16; cy++)
	{
		for (UINT cx=0; cx<16; cx++)
		{
			get_csbits_xy(csbits, ch++, cx, cy, pBuffer);
		}
	}

	DeleteObject(hCharBitmap);

	delete [] pBuffer;
}

void make_csbits (void)
{
	get_csbits(&csbits_enhanced2e[0], TEXT("CHARSET40"), 0);	// Enhanced //e: Alt char set off
	get_csbits(&csbits_enhanced2e[1], TEXT("CHARSET40"), 16);	// Enhanced //e: Alt char set on (mousetext)
	get_csbits(&csbits_a2[0],         TEXT("CHARSET40"), 32);	// Apple ][, ][+
	get_csbits(&csbits_pravets82[0],  TEXT("CHARSET82"), 0);	// Pravets 82
	get_csbits(&csbits_pravets8M[0],  TEXT("CHARSET8M"), 0);	// Pravets 8M
	get_csbits(&csbits_pravets8C[0],  TEXT("CHARSET8C"), 0);	// Pravets 8A / 8C: Alt char set off
	get_csbits(&csbits_pravets8C[1],  TEXT("CHARSET8C"), 16);	// Pravets 8A / 8C: Alt char set on

	// Original //e is just Enhanced //e with the 32 mousetext chars [0x40..0x5F] replaced by the non-alt charset chars [0x40..0x5F]
	memcpy(csbits_2e, csbits_enhanced2e, sizeof(csbits_enhanced2e));
	memcpy(&csbits_2e[1][64], &csbits_2e[0][64], 32*8);
}

#else

// NB. '(', ')' are 1 bit out of alignment
static const char *csstrs[] = {
	"   ###  ","    #   ","  ####  ","   ###  ","  ####  ","  ##### ","  ##### ","   #### ",
	"  #   # ","   # #  ","  #   # ","  #   # ","  #   # ","  #     ","  #     ","  #     ",
	"  # # # ","  #   # ","  #   # ","  #     ","  #   # ","  #     ","  #     ","  #     ",
	"  # ### ","  #   # ","  ####  ","  #     ","  #   # ","  ####  ","  ####  ","  #     ",
	"  # ##  ","  ##### ","  #   # ","  #     ","  #   # ","  #     ","  #     ","  #  ## ",
	"  #     ","  #   # ","  #   # ","  #   # ","  #   # ","  #     ","  #     ","  #   # ",
	"   #### ","  #   # ","  ####  ","   ###  ","  ####  ","  ##### ","  #     ","   #### ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"  #   # ","   ###  ","      # ","  #   # ","  #     ","  #   # ","  #   # ","   ###  ",
	"  #   # ","    #   ","      # ","  #  #  ","  #     ","  ## ## ","  #   # ","  #   # ",
	"  #   # ","    #   ","      # ","  # #   ","  #     ","  # # # ","  ##  # ","  #   # ",
	"  ##### ","    #   ","      # ","  ##    ","  #     ","  # # # ","  # # # ","  #   # ",
	"  #   # ","    #   ","      # ","  # #   ","  #     ","  #   # ","  #  ## ","  #   # ",
	"  #   # ","    #   ","  #   # ","  #  #  ","  #     ","  #   # ","  #   # ","  #   # ",
	"  #   # ","   ###  ","   ###  ","  #   # ","  ##### ","  #   # ","  #   # ","   ###  ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"  ####  ","   ###  ","  ####  ","   ###  ","  ##### ","  #   # ","  #   # ","  #   # ",
	"  #   # ","  #   # ","  #   # ","  #   # ","    #   ","  #   # ","  #   # ","  #   # ",
	"  #   # ","  #   # ","  #   # ","  #     ","    #   ","  #   # ","  #   # ","  #   # ",
	"  ####  ","  #   # ","  ####  ","   ###  ","    #   ","  #   # ","  #   # ","  # # # ",
	"  #     ","  # # # ","  # #   ","      # ","    #   ","  #   # ","  #   # ","  # # # ",
	"  #     ","  #  #  ","  #  #  ","  #   # ","    #   ","  #   # ","   # #  ","  ## ## ",
	"  #     ","   ## # ","  #   # ","   ###  ","    #   ","   ###  ","    #   ","  #   # ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"  #   # ","  #   # ","  ##### ","  ##### ","        ","  ##### ","        ","        ",
	"  #   # ","  #   # ","      # ","  ##    ","  #     ","     ## ","        ","        ",
	"   # #  ","   # #  ","     #  ","  ##    ","   #    ","     ## ","    #   ","        ",
	"    #   ","    #   ","    #   ","  ##    ","    #   ","     ## ","   # #  ","        ",
	"   # #  ","    #   ","   #    ","  ##    ","     #  ","     ## ","  #   # ","        ",
	"  #   # ","    #   ","  #     ","  ##    ","      # ","     ## ","        ","        ",
	"  #   # ","    #   ","  ##### ","  ##### ","        ","  ##### ","        ","        ",
	"        ","        ","        ","        ","        ","        ","        "," #######",
	"        ","    #   ","   # #  ","   # #  ","    #   ","  ##    ","   #    ","    #   ",
	"        ","    #   ","   # #  ","   # #  ","   #### ","  ##  # ","  # #   ","    #   ",
	"        ","    #   ","   # #  ","  ##### ","  # #   ","     #  ","  # #   ","    #   ",
	"        ","    #   ","        ","   # #  ","   ###  ","    #   ","   #    ","        ",
	"        ","    #   ","        ","  ##### ","    # # ","   #    ","  # # # ","        ",
	"        ","        ","        ","   # #  ","  ####  ","  #  ## ","  #  #  ","        ",
	"        ","    #   ","        ","   # #  ","    #   ","     ## ","   ## # ","        ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"     #  ","   #    ","    #   ","        ","        ","        ","        ","        ",
	"    #   ","    #   ","  # # # ","    #   ","        ","        ","        ","      # ",
	"   #    ","     #  ","   ###  ","    #   ","        ","        ","        ","     #  ",
	"   #    ","     #  ","    #   ","  ##### ","        ","  ##### ","        ","    #   ",
	"   #    ","     #  ","   ###  ","    #   ","    #   ","        ","        ","   #    ",
	"    #   ","    #   ","  # # # ","    #   ","    #   ","        ","        ","  #     ",
	"     #  ","   #    ","    #   ","        ","   #    ","        ","    #   ","        ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"   ###  ","    #   ","   ###  ","  ##### ","     #  ","  ##### ","    ### ","  ##### ",
	"  #   # ","   ##   ","  #   # ","      # ","    ##  ","  #     ","   #    ","      # ",
	"  #  ## ","    #   ","      # ","     #  ","   # #  ","  ####  ","  #     ","     #  ",
	"  # # # ","    #   ","    ##  ","    ##  ","  #  #  ","      # ","  ####  ","    #   ",
	"  ##  # ","    #   ","   #    ","      # ","  ##### ","      # ","  #   # ","   #    ",
	"  #   # ","    #   ","  #     ","  #   # ","     #  ","  #   # ","  #   # ","   #    ",
	"   ###  ","   ###  ","  ##### ","   ###  ","     #  ","   ###  ","   ###  ","   #    ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"   ###  ","   ###  ","        ","        ","      # ","        ","  #     ","   ###  ",
	"  #   # ","  #   # ","        ","        ","     #  ","        ","   #    ","  #   # ",
	"  #   # ","  #   # ","    #   ","    #   ","    #   ","  ##### ","    #   ","     #  ",
	"   ###  ","   #### ","        ","        ","   #    ","        ","     #  ","    #   ",
	"  #   # ","      # ","    #   ","    #   ","    #   ","  ##### ","    #   ","    #   ",
	"  #   # ","     #  ","        ","    #   ","     #  ","        ","   #    ","        ",
	"   ###  ","  ###   ","        ","   #    ","      # ","        ","  #     ","    #   ",
	"        ","        ","        ","        ","        ","        ","        ","        ",
	"   #    ","        ","  #     ","        ","      # ","        ","    ##  ","        ",
	"    #   ","        ","  #     ","        ","      # ","        ","   #  # ","        ",
	"     #  ","   ###  ","  ####  ","   #### ","   #### ","   ###  ","   #    ","   #### ",
	"        ","      # ","  #   # ","  #     ","  #   # ","  #   # ","  ####  ","  #   # ",
	"        ","   #### ","  #   # ","  #     ","  #   # ","  ##### ","   #    ","  #   # ",
	"        ","  #   # ","  #   # ","  #     ","  #   # ","  #     ","   #    ","   #### ",
	"        ","   #### ","  ####  ","   #### ","   #### ","   #### ","   #    ","      # ",
	"        ","        ","        ","        ","        ","        ","        ","   ###  ",
	"  #     ","    #   ","     #  ","  #     ","   ##   ","        ","        ","        ",
	"  #     ","        ","        ","  #     ","    #   ","        ","        ","        ",
	"  ####  ","   ##   ","    ##  ","  #   # ","    #   ","  ## ## ","  ####  ","   ###  ",
	"  #   # ","    #   ","     #  ","  #  #  ","    #   ","  # # # ","  #   # ","  #   # ",
	"  #   # ","    #   ","     #  ","  ###   ","    #   ","  # # # ","  #   # ","  #   # ",
	"  #   # ","    #   ","     #  ","  #  #  ","    #   ","  # # # ","  #   # ","  #   # ",
	"  #   # ","   ###  ","  #  #  ","  #   # ","   ###  ","  #   # ","  #   # ","   ###  ",
	"        ","        ","   ##   ","        ","        ","        ","        ","        ",
	"        ","        ","        ","        ","   #    ","        ","        ","        ",
	"        ","        ","        ","        ","   #    ","        ","        ","        ",
	"  ####  ","   #### ","  # ### ","   #### ","  ####  ","  #   # ","  #   # ","  #   # ",
	"  #   # ","  #   # ","  ##    ","  #     ","   #    ","  #   # ","  #   # ","  #   # ",
	"  #   # ","  #   # ","  #     ","   ###  ","   #    ","  #   # ","  #   # ","  # # # ",
	"  ####  ","   #### ","  #     ","      # ","   #  # ","  #  ## ","   # #  ","  # # # ",
	"  #     ","      # ","  #     ","  ####  ","    ##  ","   ## # ","    #   ","  ## ## ",
	"  #     ","      # ","        ","        ","        ","        ","        ","        ",
	"        ","        ","        ","    ### ","    #   ","  ###   ","   ## # ","        ",
	"        ","        ","        ","   ##   ","    #   ","    ##  ","  # ##  ","  # # # ",
	"  #   # ","  #   # ","  ##### ","   ##   ","    #   ","    ##  ","        ","   # #  ",
	"   # #  ","  #   # ","     #  ","  ##    ","    #   ","     ## ","        ","  # # # ",
	"    #   ","  #   # ","    #   ","   ##   ","    #   ","    ##  ","        ","   # #  ",
	"   # #  ","   #### ","   #    ","   ##   ","    #   ","    ##  ","        ","  # # # ",
	"  #   # ","      # ","  ##### ","    ### ","    #   ","  ###   ","        ","        ",
	"        ","   ###  ","        ","        ","    #   ","        ","        ","        ",
	"     #  ","     #  ","        "," #######","        "," #######","     ###","        ",
	"    #   ","    #   ","        ","  #   # ","       #"," ###### ","      ##","    ##  ",
	"  ## ## ","  ## ## ","  #     ","   # #  ","      # "," ##### #","  ######"," ###    ",
	" #######"," #     #","  ##    ","    #   "," #   #  ","   ## ##"," #   ## ","        ",
	" ###### "," #    # ","  ###   ","    #   ","  # #   "," # # ###"," #  ####"," ###    ",
	" ###### "," #    # ","  ####  ","   # #  ","   #    "," ## ####","     ## ","   ##   ",
	"  ######","  # #  #","  ## ## ","  # # # ","   #    "," ## ####"," ###### ","    #   ",
	"  ## ## ","  ## ## ","  #    #"," #######","        "," #######","  #     ","     ###",
	"    #   ","        ","    #   ","    #   "," #######","       #"," ###### "," ##  #  ",
	"   #    ","        ","    #   ","   ###  ","        ","       #"," ###### ","    ##  ",
	"  #     ","        ","    #   ","  # # # ","        ","   #   #"," ###### ","   ###  ",
	" #######","        ","    #   "," #  #  #","        ","  ##   #"," ###### ","  ######",
	"  #     ","        "," #  #  #","    #   ","        "," #######"," ###### ","   ###  ",
	"   #    ","        ","  # # # ","    #   ","        ","  ##    "," ###### ","    ##  ",
	"    #   ","        ","   ###  ","    #   ","        ","   #    "," ###### ","     #  ",
	"        ","  # # # ","    #   ","    #   ","        ","        "," ###### "," #### ##",
	"   #  ##","       #","       #","        "," #      ","    #   ","  # # # "," # # # #",
	"   ##   ","    #  #","    #  #","        "," #      ","     #  "," # # # #","  # # # ",
	"   ###  ","    #   ","   ###  ","        "," #      ","      # ","  # # # "," # # # #",
	" ###### "," #######","  ##### "," #######"," #      "," #######"," # # # #","  # # # ",
	"   ###  ","  ##### "," #######","        "," #      ","      # ","  # # # "," # # # #",
	"   ##   ","   ###  ","    #   ","        "," #      ","     #  "," # # # #","  # # # ",
	"   #    ","    #  #","    #  #","        "," #      ","    #   ","  # # # "," # # # #",
	" ## ####","       #","       #","        "," #######","        "," # # # #","  # # # ",
	"        ","        ","       #","    #   "," #######","   # #  "," #######"," #      ",
	"  ##### ","        ","       #","   ###  ","        ","   # #  ","       #"," #      ",
	" #     #","####### ","       #","  ##### ","        "," ### ###","       #"," #      ",
	" #      ","       #","       #"," #######","        ","        ","   ##  #"," #      ",
	" #      ","       #","       #","  ##### ","        "," ### ###","   ##  #"," #      ",
	" #      ","       #","       #","   ###  ","        ","   # #  ","       #"," #      ",
	" #######","########","       #","    #   ","        ","   # #  ","       #"," #      ",
	"        ","        ","       #","        "," #######","        "," #######"," #      ",
};

#define STRINGS_PER_CHAR 8
#define CHARS_PER_ROW 8
#define STRINGS_PER_CHAR_ROW (STRINGS_PER_CHAR * CHARS_PER_ROW)

void make_csbits (void) {
	int i,j;
	int szstrs = sizeof csstrs / sizeof csstrs[0];
	int numchars = szstrs / STRINGS_PER_CHAR;
	
	for (i = 0; i < numchars; ++i) {
		int si = ((i / STRINGS_PER_CHAR) * STRINGS_PER_CHAR_ROW) + (i % CHARS_PER_ROW);
		for (j = 0; j < STRINGS_PER_CHAR; ++j)
		{
			const char *sp = csstrs[si];
			unsigned char cb = 0;
		
			si += CHARS_PER_ROW;

			while (*sp) {
				if (*sp++ != ' ') cb |= 0x80;
				cb >>= 1;
			}
		
			csbits[0][i][j] = cb;
		}
	}

	/* move mousetext */
	for (i = 96; i < 128; ++i)
		for (j = 0; j < 8; ++j)
			csbits[1][i-32][j] = csbits[0][i][j];

	/* move lowercase */
	for (i = 64; i < 96; ++i)
		for (j = 0; j < 8; ++j)
			csbits[1][i+32][j] = csbits[1][i+160][j] =
				csbits[0][i+160][j] = csbits[0][i][j];

	/* move numbers */
	for (i = 32; i < 64; ++i)
		for (j = 0; j < 8; ++j)
			csbits[1][i][j] = csbits[1][i+128][j] =
				csbits[0][i+64][j] = csbits[0][i+128][j] = csbits[0][i][j];

	/* move uppercase */
	for (i = 0; i < 32; ++i)
		for (j = 0; j < 8; ++j)
			csbits[1][i][j] = csbits[1][i+128][j] = csbits[1][i+192][j] =
				csbits[0][i+64][j] = csbits[0][i+128][j] = csbits[0][i+192][j] = csbits[0][i][j];

	/* invert (skip mousetext) */
	for (i = 0; i < 128; ++i)
		for (j = 0; j < 8; ++j)
		{
			csbits[0][i][j] ^= 0xFF;
			if (i < 64 || i >= 96)
				csbits[1][i][j] ^= 0xFF;
		}
}

#endif

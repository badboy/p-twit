/*
 * triFont.c: Implementation for bitmap font library
 * This file is part of the "tri Engine".
 *
 * Copyright (C) 2007 tri
 * Copyright (C) 2007 David Perry 'InsertWittyName' <tias_dp@hotmail.com>
 * Copyright (C) 2007 Alexander Berl 'Raphael' <raphael@fx-world.org>
 *
 * $Id: $
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "triGraphics.h"	// Needed for triEnable/Disable
#include "triFont.h"
#include "triVAlloc.h"
#include "triLog.h"
#include "triMemory.h"

static const triSInt TRI_FONT_TEXTURE_MIN_SIZE = 64;
static const triSInt TRI_FONT_TEXTURE_MAX_SIZE = 512;

static const triChar *TRI_FONT_CHARSET =
	" .,!?:;"
    	"0123456789"
    	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    	"abcdefghijklmnopqrstuvwxyz"
    	"@#$%^&*+-_()[]{}<>/\\|~`\""
;

// Store the clut on the framebuffer's first (or second in 16bit mode) line's padding area
static triUInt* clut = (triUInt*)(0x44000000 + 480*4);
//static triUInt __attribute__((aligned(16))) clut[16];

typedef struct GlyphInfo
{
	struct GlyphInfo *next, *prev;
	triUInt c;
	Glyph glyph;
} GlyphInfo;

GlyphInfo glyphList =
{
	.next = &glyphList,
	.prev = &glyphList,
};

// Return next power of 2
static triS32 next_pow2(triU32 w)
{
	w -= 1;		// To not change already power of 2 values
	w |= (w >> 1);
	w |= (w >> 2);
	w |= (w >> 4);
	w |= (w >> 8);
	w |= (w >> 16);
	return(w+1);
}

static triSInt triFontSwizzle(triFont *font)
{
	triSInt byteWidth = font->texSize/2;
	triSInt textureSize = font->texSize*font->texHeight/2;
	triSInt height = textureSize / byteWidth;

	// Swizzle the texture
	triSInt rowBlocks = (byteWidth / 16);
	triSInt rowBlocksAdd = (rowBlocks-1)*128;
	triUInt blockAddress = 0;
	triUInt *src = (triUInt*) font->texture;
	static triUChar *tData = 0;

	if(font->location)
		tData = (triUChar*) triVAlloc(textureSize);
	if(!tData)
		tData = (triUChar*) triMalloc(textureSize);
	
	if(!tData)
		return 0;

	triSInt j;

	for(j = 0; j < height; j++, blockAddress += 16)
	{
		triUInt *block;
		if (font->location)
			block = (triUInt*)((triUInt)&tData[blockAddress]|0x40000000);
		else
			block = (triUInt*)&tData[blockAddress];
		triSInt i;

		for(i=0; i < rowBlocks; i++)
		{
			*block++ = *src++;
			*block++ = *src++;
			*block++ = *src++;
			*block++ = *src++;
			block += 28;
		}

		if((j&0x7) == 0x7)
			blockAddress += rowBlocksAdd;
	}
	sceKernelDcacheWritebackAll();
	triFree(font->texture);

	font->texture = tData;
	
	return 1;
}

triFont* triFontLoad(const triChar *filename, triUInt fontSize, enum triFontSizeType fontSizeType, triUInt textureSize, enum triFontLocation location)
{
	FT_Library library;
	FT_Face face;
	FT_GlyphSlot slot;
	GlyphInfo *gp;
	GlyphInfo gi[256];
	triSInt n, count, charCount;
	triSInt xx, yy;

	charCount = strlen(TRI_FONT_CHARSET);
	count = charCount;

	textureSize = next_pow2( textureSize );	// Make sure it is really a pow2
	if((textureSize < TRI_FONT_TEXTURE_MIN_SIZE) || (textureSize > TRI_FONT_TEXTURE_MAX_SIZE))
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error, textureSize out of bounds: %s\r\n", filename);
		#endif
		return NULL;
	}

	triFont* font = (triFont*) triMalloc(sizeof(triFont));
	
	if(!font)
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error allocating font: %s\r\n", filename);
		#endif
		return NULL;
	}
	
	font->texSize = textureSize;
	font->location = location;

	if(FT_Init_FreeType(&library))
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Freetype init failure %s\r\n", filename);
		#endif
		triFree(font);
		return NULL;
	}

	if(FT_New_Face(library, filename, 0, &face))
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Freetype cannot load font %s\r\n", filename);
		#endif
		triFree(font);
		return NULL;
	}
	
	if(fontSizeType == TRI_FONT_SIZE_PIXELS)
	{
		if(FT_Set_Pixel_Sizes(face, fontSize, 0))
		{
			#ifdef _DEBUG_LOG
			triLogError("triFont: Freetype cannot set to fontSize: %s, %d\r\n", filename, fontSize);
			#endif
			triFree(font);
			return NULL;
		}
	}
	else
	{
		if(FT_Set_Char_Size(face, fontSize * 64, 0, 100, 0))
		{
			#ifdef _DEBUG_LOG
			triLogError("triFont: Freetype cannot set to fontSize: %s, %d\r\n", filename, fontSize);
			#endif
			triFree(font);
			return NULL;
		}
	}

	slot = face->glyph;

	triChar minLeft = 127, maxTop = -127;
	// Assemble metrics and sort by fontSize
	for(n = 0; n < count; n++)
	{
		if(FT_Load_Char(face, TRI_FONT_CHARSET[n], FT_LOAD_RENDER))
		{
			#ifdef _DEBUG_LOG
			triLogError("triFont: Cannot load character: %s, '%c'\r\n", filename, TRI_FONT_CHARSET[n]);
			#endif
			triFree(font);
			return NULL;
		}

		gi[n].c = TRI_FONT_CHARSET[n];
		gi[n].glyph.x = 0;
		gi[n].glyph.y = 0;
		gi[n].glyph.width = slot->bitmap.width;
		gi[n].glyph.height = slot->bitmap.rows;
		gi[n].glyph.top = slot->bitmap_top;
		gi[n].glyph.left = slot->bitmap_left;
		gi[n].glyph.advance = slot->advance.x / 64;
		gi[n].glyph.unused = 0;

		if (minLeft>slot->bitmap_left) minLeft=slot->bitmap_left;
		if (maxTop<slot->bitmap_top) maxTop=slot->bitmap_top;
		// Find a good fit
		gp = glyphList.next;

		while((gp != &glyphList) && (gp->glyph.height > gi[n].glyph.height))
		{
			gp = gp->next;
		}

		gi[n].next = gp;
		gi[n].prev = gp->prev;
		gi[n].next->prev = gi;
		gi[n].prev->next = gi;
	}

	triSInt x = 0;
	triSInt y = 0;
	triSInt ynext = 0;
	triSInt used = 0;

	count = 0;
	memset(font->map, 255, 256);

	font->texture = (triUChar*) triMalloc(textureSize * textureSize / 2);
	
	if(!font->texture)
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error allocating font texture: %s\r\n", filename);
		#endif
		triFree(font);
		return NULL;
	}
    	
	memset(font->texture, 0, textureSize * textureSize / 2);

	font->fontHeight = 0;
	for(n = 0; n < charCount; n++)
	{
		if(FT_Load_Char(face, gi[n].c, FT_LOAD_RENDER))
		{
			#ifdef _DEBUG_LOG
			triLogError("triFont: Cannot load character: %s, '%c'\r\n", filename, TRI_FONT_CHARSET[n]);
			#endif
			triFontUnload(font);
			return NULL;
		}
		
		if((x + gi[n].glyph.width) > textureSize)
		{
			y += ynext;
			x = 0;
		}

		if(gi[n].glyph.height > ynext)
			ynext = gi[n].glyph.height;

		if((y + ynext) > textureSize)
		{
			#ifdef _DEBUG_LOG
			triLogError("triFont: Error out of space in texture: %s\r\n", filename);
			#endif
			triFontUnload(font);
			return NULL;
		}

		font->map[gi[n].c] = count++;
		gi[n].glyph.x = x;
		gi[n].glyph.y = y;
		// Normalize font positions, so that printing at 0,0 will print the font completely
		gi[n].glyph.left -= minLeft;
		gi[n].glyph.top -= maxTop;

		if((triS32)(gi[n].glyph.height+gi[n].glyph.top) > font->fontHeight)
			font->fontHeight = (gi[n].glyph.height+gi[n].glyph.top);
		
		triU32 slotoffs = 0;
		triU32 texoffs = (y*textureSize)/2;
		triU8* slotbuf = slot->bitmap.buffer;
        for(yy = 0; yy < gi[n].glyph.height; yy++)
		{
			xx = 0;
			if (x & 1)
			{
				font->texture[(x>>1) + texoffs] |= (slotbuf[slotoffs] & 0xF0);
				xx++;
			}
			for(; xx < gi[n].glyph.width; xx+=2)
			{
				if (xx+1 < gi[n].glyph.width)
					font->texture[((x+xx)>>1) + texoffs] = (slotbuf[slotoffs + xx] >> 4)
													| (slotbuf[slotoffs + xx + 1] & 0xF0);
				else
					font->texture[((x+xx)>>1) + texoffs] = (slotbuf[slotoffs + xx] >> 4);
			}
			texoffs += textureSize>>1;
			slotoffs += slot->bitmap.width;
		}

		x += gi[n].glyph.width;

		used += (gi[n].glyph.width * gi[n].glyph.height);
	}

	font->texHeight = (y + ynext + 7)&~7;
	if (font->texHeight > font->texSize) font->texHeight = font->texSize;
	
	/* redirect any undefined characters to ' ' */
	for(n = 0; n < 256; n++)
	{
		if(font->map[n] == 255)
			font->map[n] = font->map[' '];
	}

    for(n = 0; n < charCount; n++)
	{
		memcpy(&font->glyph[n], &gi[n].glyph, sizeof(gi[n].glyph));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
	if(!triFontSwizzle(font))
	{
		triFontUnload(font);
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error swizzling font: %s\r\n", filename);
		#endif
		return NULL;
	}

	return font;
}

void triFontUnload(triFont *font)
{
	if(!font)
		return;
	
	if(font->texture)
	{
		if(font->location)
			triVFree(font->texture);
		else
			triFree(font->texture);
	}		

	triFree(font);
}

triBool triFontInit(void)
{
	triSInt n;

	for(n = 0; n < 16; n++)
	{
		clut[n] = ((n*17) << 24) | 0xffffff;
	}
	
	return 1;
}

void triFontShutdown(void)
{
	//Nothing yet
}

void triFontActivate(triFont *font)
{
	if(!font)
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error activating font, not a valid triFont struct\r\n");
		#endif
		return;
	}

	sceGuClutMode(GU_PSM_8888, 0, 255, 0);
	sceGuClutLoad(16 / 8, clut);

	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(GU_PSM_T4, 0, 0, 1);
	sceGuTexImage(0, font->texSize, font->texSize, font->texSize, font->texture);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexEnvColor(0x0);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuTexScale(1.0f / 256.0f, 1.0f / 256.0f);
	sceGuTexWrap(GU_REPEAT, GU_REPEAT);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
}

triSInt triFontPrintf(triFont *font, triFloat x, triFloat y, triUInt color, const triChar *text, ...)
{
	if(!font)
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error printing font, not a valid triFont struct\r\n");
		#endif
		return 0;
	}

	triChar buffer[256];
	va_list ap;
	
	va_start(ap, text);
	vsnprintf(buffer, 256, text, ap);
	va_end(ap);
	buffer[255] = 0;
	
	return triFontPrint(font, x, y, color, buffer);
}

triSInt triFontPrint(triFont *font, triFloat x, triFloat y, triUInt color, const triChar *text)
{
	if(!font)
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error printing font, not a valid triFont struct\r\n");
		#endif
		return 0;
	}

	typedef struct
	{
		triU16 u, v;
		//triUInt c;
		triS16 x, y, z;
	} FontVertex;

	triSInt i, length;
	FontVertex *v, *v0, *v1;
	
	if((length = strlen(text)) == 0)
		return 0;
		
	v = sceGuGetMemory(sizeof(FontVertex) * 2 * length);
	v0 = v;
	
	triFloat xstart = x;
	sceGuColor( color );
	for(i = 0; i < length; i++)
	{
		Glyph *glyph = font->glyph + font->map[text[i] & 0xff];
		
		v1 = v0+1;
		
		v0->u = glyph->x;
		v0->v = glyph->y;
		v0->x = (triU16)x + glyph->left;
		v0->y = (triU16)y - glyph->top;
		v0->z = 0;

		v1->u = glyph->x + glyph->width;
		v1->v = glyph->y + glyph->height;
		v1->x = v0->x + glyph->width;
		v1->y = v0->y + glyph->height;
		v1->z = 0;

		v0 += 2;
		x += glyph->advance;
		
		if (text[i]=='\n')
		{
			x = xstart;
			y += font->fontHeight+3;
		}
	}

	triDisable(TRI_DEPTH_TEST);
	triDisable(TRI_DEPTH_MASK);
	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D, length * 2, 0, v);
	triEnable(TRI_DEPTH_MASK);	// Need triEnable calls to make sure depthbuffer is available
	triEnable(TRI_DEPTH_TEST);
	
	return x;
}


triSInt triFontMeasureText(triFont *font, const triChar *text)
{
	if(!font)
	{
		#ifdef _DEBUG_LOG
		triLogError("triFont: Error measuring font text, not a valid triFont struct\r\n");
		#endif
		return 0;
	}

	triSInt x = 0;
	
	while(*text)
	{
		Glyph *glyph = font->glyph + font->map[*text++ & 0xff];
		x += glyph->advance;
	}
	
	return x;
}


/*
 * triGraphics.h: Header for 2D Graphics Engine
 * This file is part of the "tri Engine".
 *
 * Copyright (C) 2007 tri
 * Copyright (C) 2007 Alexander Berl 'Raphael' <raphael@fx-world.org>
 *
 * $Id: triGraphics.h 10 2007-12-19 20:32:11Z Raphael $
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

#ifndef __TRIGRAPHICS_H__
#define __TRIGRAPHICS_H__

#include <pspgu.h>
#include "triImage.h"


/** @defgroup triGraphics 2D graphics core
 *  @{
 */
 
// Constants
//--------------------------------------------------
#define SCREEN_WIDTH	480
#define SCREEN_HEIGHT	272

#define FRAME_BUFFER_WIDTH 512
#define FRAME_BUFFER_SIZE		(FRAME_BUFFER_WIDTH*SCREEN_HEIGHT)



/* triEnable/Disable */
#define TRI_ALPHA_TEST					0
#define TRI_DEPTH_TEST					1
#define TRI_SCISSOR_TEST				2
#define TRI_STENCIL_TEST				3
#define TRI_BLEND						4
#define TRI_CULL_FACE					5
#define TRI_DITHER						6
#define TRI_FOG							7
#define TRI_CLIP_PLANES					8
#define TRI_TEXTURE_2D					9
#define TRI_LIGHTING					10
#define TRI_LIGHT0						11
#define TRI_LIGHT1						12
#define TRI_LIGHT2						13
#define TRI_LIGHT3						14
#define TRI_AALINE                      15
#define TRI_COLORKEY	                17
#define TRI_LOGICAL_OP		            18
#define TRI_NORMAL_REVERSE				19
#define TRI_VBLANK						32
#define TRI_PSEUDO_FSAA					33
#define TRI_SMOOTH_DITHER				34
#define TRI_DEPTH_MASK					35
#define TRI_DEPTH_WRITE					35



extern void* triFramebuffer;
extern void* triDepthbuffer;
extern triS32 triPsm;
extern triS32 triBpp;

void triInit( triS32 psm );
void triClose();


/* these two are not needed anymore and only left for backwards compatibility */
void triBegin();
void triEnd();

/** Sync CPU to GU
  */
void triSync();
void triSwapbuffers();

/** Wait for Vblank.
  * Will do nothing if TRI_VBLANK is already enabled.
  */
void triVblank();

/** Return the current CPU load in percent.
  * @returns The current CPU load in percent
  */
triFloat triCPULoad();

/** Return the current GPU load in percent.
  * @returns The current GPU load in percent
  */
triFloat triGPULoad();

/** Return the current FPS.
  * @returns The current FPS
  */
triFloat triFps();

/** Return the maximal FPS.
  * @returns The maximal FPS
  */
triFloat triFpsMax();

/** Return the minimal FPS.
  * @returns The minimal FPS
  */
triFloat triFpsMin();

void triPerspective( triFloat fov );
void triOrtho();


void triEnable( triU32 state );
void triDisable( triU32 state );


void triClear( triU32 color );

// Set clipping area
void triSetClip( triS32 x, triS32 y, triS32 width, triS32 height );
// Reset clipping area
void triResetClip();


void triRendertoscreen();
void triRendertotexture( triS32 psm, void* tbp, triS32 tw, triS32 th, triS32 tbw );
void triRendertoimage( triImage* img );

void triCopyToScreen( triS32 x, triS32 y, triS32 w, triS32 h, triS32 sx, triS32 sy, triS32 sbw, void* src );
void triCopyFromScreen( triS32 x, triS32 y, triS32 w, triS32 h, triS32 dx, triS32 dy, triS32 dbw, void* dst );

void triDrawLine( triFloat x0, triFloat y0, triFloat x1, triFloat y1, triU32 color );
void triDrawRect( triFloat x, triFloat y, triFloat width, triFloat height, triU32 color );
void triDrawRectOutline( triFloat x, triFloat y, triFloat width, triFloat height, triU32 color );
void triDrawRectRotate( triFloat x, triFloat y, triFloat width, triFloat height, triU32 color, triFloat angle );
void triDrawRectGrad( triFloat x, triFloat y, triFloat width, triFloat height, triU32 color1, triU32 color2, triU32 color3, triU32 color4 );
void triDrawCircle( triFloat x, triFloat y, triFloat radius, triU32 color, triU32 numSteps );
void triDrawCircleOutline( triFloat x, triFloat y, triFloat radius, triU32 color, triU32 numSteps );
void triDrawCircleGrad( triFloat x, triFloat y, triFloat radius, triU32 color1, triU32 color2, triU32 numSteps );
void triDrawTri( triFloat x0, triFloat y0,  triFloat x1, triFloat y1,  triFloat x2, triFloat y2, triU32 color );
void triDrawTriOutline( triFloat x0, triFloat y0,  triFloat x1, triFloat y1,  triFloat x2, triFloat y2, triU32 color );
void triDrawTriGrad( triFloat x0, triFloat y0,  triFloat x1, triFloat y1,  triFloat x2, triFloat y2, triU32 color1, triU32 color2, triU32 color3 );


void triColorOp( triS32 op );
void trinoColorOp();

void triImageTint( triS32 mode, triS32 comp, triU32 vcolor, triU32 ccolor );
void triImageNoTint();
// const alpha overrides Tint mode!
void triImageConstAlpha( triU32 alpha );
void triImageBlend( triS32 op, triS32 srcOp, triS32 dstOp, triU32 src_fix, triU32 dst_fix );
void triImageNoBlend();
void triImageColorkey( triU32 color );
void triImageNoColorkey();


/** Set sprite mode.
  * @param width Width of all subsequent sprites to render
  * @param height Height of all subsequent sprites to render
  * @param angle Angle of all subsequent sprites to rotate
  */
void triSpriteMode( triFloat width, triFloat height, triFloat angle );

/** Render a sprite.
  * @param x Screen position to draw the sprite to
  * @param y Screen position to draw the sprite to
  * @param u Vertical sprite position to start drawing from (sprite sheet/tile map)
  * @param v Horizontal sprite position to start drawing from (sprite sheet/tile map)
  * @param img Image to draw
  */
void triDrawSprite( triFloat x, triFloat y, triFloat u, triFloat v, triImage* img );

void triBltSprite( triFloat x, triFloat y, triFloat u, triFloat v, triImage* img );

/** Render an image.
  * @param x Screen position to draw the image to
  * @param y Screen position to draw the image to
  * @param img Image to draw
  */
void triDrawImage2( triFloat x, triFloat y, triImage* img );

/** Render an image.
  * @param x Screen position to draw the image to
  * @param y Screen position to draw the image to
  * @param width Width of image on screen for scaling
  * @param height Height of image on screen for scaling
  * @param u0 Vertical image position to start drawing from (sprite sheet/tile map)
  * @param v0 Horizontal image position to start drawing from (sprite sheet/tile map)
  * @param u1 Vertical image position to end drawing at (sprite sheet/tile map)
  * @param v1 Horizontal image position to end drawing at (sprite sheet/tile map)
  * @param img Image to draw
  */
void triDrawImage( triFloat x, triFloat y, triFloat width, triFloat height, 
			   triFloat u0, triFloat v0, triFloat u1, triFloat v1, 
			   triImage* img );

/** Render an image with rotation.
  * @param x Screen position to draw the image to
  * @param y Screen position to draw the image to
  * @param angle Angle to rotate the image by in degree
  * @param img Image to draw
  */
void triDrawImageRotate2( triFloat x, triFloat y, triFloat angle, triImage* img );

/** Render an image with rotation.
  * @param x Screen position to draw the image to
  * @param y Screen position to draw the image to
  * @param width Width of image on screen for scaling
  * @param height Height of image on screen for scaling
  * @param u0 Vertical image position to start drawing from (sprite sheet/tile map)
  * @param v0 Horizontal image position to start drawing from (sprite sheet/tile map)
  * @param u1 Vertical image position to end drawing at (sprite sheet/tile map)
  * @param v1 Horizontal image position to end drawing at (sprite sheet/tile map)
  * @param angle Angle to rotate the image by in degree
  * @param img Image to draw
  */
void triDrawImageRotate( triFloat x, triFloat y, triFloat width, triFloat height, 
			   triFloat u0, triFloat v0, triFloat u1, triFloat v1, 
			   triFloat angle, triImage* img );


/** Render an image animation.
  * @param x Screen position to draw the image to
  * @param y Screen position to draw the image to
  * @param ani Image animation to draw
  */
void triDrawImageAnimation( triFloat x, triFloat y, triImageAnimation* ani );

/** Render an image animation with rotation.
  * @param x Screen position to draw the image to
  * @param y Screen position to draw the image to
  * @param angle Angle to rotate the image by in degree
  * @param ani Image animation to draw
  */
void triDrawImageAnimationRotate( triFloat x, triFloat y, triFloat angle, triImageAnimation* ani );

/** @} */

#endif // __TRIGRAPHICS_H__

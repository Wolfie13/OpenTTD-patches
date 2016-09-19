/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file 32bpp_simple.cpp Implementation of the simple 32 bpp blitter. */

#include "../stdafx.h"
#include "../debug.h"
#include "../zoom_func.h"
#include "32bpp_simple.hpp"

#include "../table/sprites.h"

const char Blitter_32bppSimple::name[] = "32bpp-simple";
const char Blitter_32bppSimple::desc[] = "32bpp Simple Blitter (no palette animation)";

void Blitter_32bppSimple::Surface::draw (const BlitterParams *bp, BlitterMode mode, ZoomLevel zoom)
{
	const Blitter_32bppSimple::Pixel *src, *src_line;
	Colour *dst, *dst_line;

	/* Find where to start reading in the source sprite */
	const Sprite *sprite = static_cast<const Sprite*> (bp->sprite);
	src_line = sprite->data + (bp->skip_top * sprite->width + bp->skip_left) * ScaleByZoom(1, zoom);
	dst_line = (Colour *)bp->dst + bp->top * bp->pitch + bp->left;

	for (int y = 0; y < bp->height; y++) {
		dst = dst_line;
		dst_line += bp->pitch;

		src = src_line;
		src_line += sprite->width * ScaleByZoom(1, zoom);

		for (int x = 0; x < bp->width; x++) {
			switch (mode) {
				case BM_COLOUR_REMAP:
					/* In case the m-channel is zero, do not remap this pixel in any way */
					if (src->m == 0) {
						if (src->a != 0) *dst = ComposeColourRGBA(src->r, src->g, src->b, src->a, *dst);
					} else {
						if (bp->remap[src->m] != 0) *dst = ComposeColourPA (AdjustBrightness (LookupColourInPalette (bp->remap[src->m]), src->v), src->a, *dst);
					}
					break;

				case BM_CRASH_REMAP:
					if (src->m == 0) {
						if (src->a != 0) {
							uint8 g = MakeDark(src->r, src->g, src->b);
							*dst = ComposeColourRGBA(g, g, g, src->a, *dst);
						}
					} else {
						if (bp->remap[src->m] != 0) *dst = ComposeColourPA (AdjustBrightness (LookupColourInPalette (bp->remap[src->m]), src->v), src->a, *dst);
					}
					break;

				case BM_BLACK_REMAP:
					if (src->a != 0) {
						*dst = Colour(0, 0, 0);
					}
					break;

				case BM_TRANSPARENT:
					/* TODO -- We make an assumption here that the remap in fact is transparency, not some colour.
					 *  This is never a problem with the code we produce, but newgrfs can make it fail... or at least:
					 *  we produce a result the newgrf maker didn't expect ;) */

					/* Make the current colour a bit more black, so it looks like this image is transparent */
					if (src->a != 0) *dst = MakeTransparent(*dst, 192);
					break;

				default:
					if (src->a != 0) *dst = ComposeColourRGBA(src->r, src->g, src->b, src->a, *dst);
					break;
			}
			dst++;
			src += ScaleByZoom(1, zoom);
		}
	}
}

::Sprite *Blitter_32bppSimple::Encode (const SpriteLoader::Sprite *sprite, bool is_font, AllocatorProc *allocator)
{
	Blitter_32bppSimple::Pixel *dst;
	Sprite *dest_sprite = AllocateSprite<Sprite> (sprite, allocator, (size_t)sprite->height * (size_t)sprite->width * sizeof(*dst));

	dst = dest_sprite->data;
	SpriteLoader::CommonPixel *src = (SpriteLoader::CommonPixel *)sprite->data;

	for (int i = 0; i < sprite->height * sprite->width; i++) {
		if (src->m == 0) {
			dst[i].r = src->r;
			dst[i].g = src->g;
			dst[i].b = src->b;
			dst[i].a = src->a;
			dst[i].m = 0;
			dst[i].v = 0;
		} else {
			/* Get brightest value */
			uint8 rgb_max = max(src->r, max(src->g, src->b));

			/* Black pixel (8bpp or old 32bpp image), so use default value */
			if (rgb_max == 0) rgb_max = DEFAULT_BRIGHTNESS;
			dst[i].v = rgb_max;

			/* Pre-convert the mapping channel to a RGB value */
			Colour colour = this->AdjustBrightness(this->LookupColourInPalette(src->m), dst[i].v);
			dst[i].r = colour.r;
			dst[i].g = colour.g;
			dst[i].b = colour.b;
			dst[i].a = src->a;
			dst[i].m = src->m;
		}
		src++;
	}

	return dest_sprite;
}

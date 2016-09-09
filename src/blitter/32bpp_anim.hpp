/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file 32bpp_anim.hpp A 32 bpp blitter with animation support. */

#ifndef BLITTER_32BPP_ANIM_HPP
#define BLITTER_32BPP_ANIM_HPP

#include "32bpp_optimized.hpp"

/** The optimised 32 bpp blitter with palette animation. */
class Blitter_32bppAnim : public Blitter_32bppBase {
public:
	typedef Blitter_32bppOptimized::Sprite Sprite;

	static const char name[]; ///< Name of the blitter.
	static const char desc[]; ///< Description of the blitter.

	/* virtual */ int BufferSize(int width, int height);
	/* virtual */ Blitter::PaletteAnimation UsePaletteAnimation();

	/* virtual */ int GetBytesPerPixel() { return 6; }

	::Sprite *Encode (const SpriteLoader::Sprite *sprite, bool is_font, AllocatorProc *allocator) OVERRIDE
	{
		return Sprite::encode (sprite, is_font, allocator);
	}

	/** Blitting surface. */
	struct Surface : Blitter_32bppBase::Surface {
		ttd_unique_free_ptr <uint16> anim_buf; ///< In this buffer we keep track of the 8bpp indexes so we can do palette animation
		Palette palette;     ///< The current palette.

		Surface (void *ptr, uint width, uint height, uint pitch)
			: Blitter_32bppBase::Surface (ptr, width, height, pitch),
				anim_buf (xcalloct<uint16> (width * height))
		{
		}

		/** Look up the colour in the current palette. */
		Colour lookup_colour (uint index) const
		{
			return this->palette.palette[index];
		}

		void set_pixel (void *video, int x, int y, uint8 colour) OVERRIDE;

		void draw_rect (void *video, int width, int height, uint8 colour) OVERRIDE;

		void recolour_rect (void *video, int width, int height, PaletteID pal) OVERRIDE;

		void scroll (void *video, int &left, int &top, int &width, int &height, int scroll_x, int scroll_y) OVERRIDE;

		bool palette_animate (const Palette &palette) OVERRIDE;

		template <BlitterMode mode> void draw (const BlitterParams *bp, ZoomLevel zoom);

		void draw (const BlitterParams *bp, BlitterMode mode, ZoomLevel zoom) OVERRIDE;

		void copy (void *dst, int x, int y, int width, int height) OVERRIDE;

		void paste (const void *src, int x, int y, int width, int height) OVERRIDE;
	};

	/** Create a surface for this blitter. */
	Blitter_32bppBase::Surface *create (void *ptr, uint width, uint height, uint pitch, bool anim) OVERRIDE
	{
		if (anim) {
			return new Surface (ptr, width, height, pitch);
		} else {
			return new Blitter_32bppOptimized::Surface (ptr, width, height, pitch);
		}
	}
};

#endif /* BLITTER_32BPP_ANIM_HPP */

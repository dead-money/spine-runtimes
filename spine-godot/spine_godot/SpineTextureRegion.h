/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated April 5, 2025. Replaces all prior versions.
 *
 * Copyright (c) 2013-2025, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#pragma once

#include "SpineCommon.h"
#include "SpineRendererObject.h"
#include <spine/Atlas.h>

#ifdef SPINE_GODOT_EXTENSION
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/rect2.hpp>
#else
#include "scene/resources/texture.h"
#endif

class SpineAtlasRegion;

// DEAD MONEY (Hommlet patch): build a spine region backed by an arbitrary
// Godot texture, so art that lives outside any spine atlas — item icons, in
// particular — can be remapped onto a skeleton slot via
// SpineAttachment.set_region, the same path equipment uses for prebuilt-atlas
// regions.
//
// Owns a synthetic spine::AtlasPage + spine::AtlasRegion + SpineRendererObject:
// the page's `texture` points at the SpineRendererObject (where SpineSprite's
// draw path reads the Godot texture/RID), and the region carries full-texture
// (or sub-rect) UVs. SpineSprite sizes the quad from the *attachment's*
// authored geometry — the region only supplies UVs — so the item art is
// expected to be authored to the slot's geometry.
//
// The SpineAtlasRegion from get_region() is a NON-owning wrapper around our
// spine::AtlasRegion, and SpineAttachment.set_region stores the raw region
// pointer on the attachment's Sequence. This object must therefore outlive any
// attachment it was bound to; the caller holds the reference for the duration.
class SpineTextureRegion : public REFCOUNTED {
	GDCLASS(SpineTextureRegion, REFCOUNTED)

protected:
	static void _bind_methods();

	spine::AtlasPage *page = nullptr;
	spine::AtlasRegion *region = nullptr;
	SpineRendererObject *renderer_object = nullptr;
	Ref<SpineAtlasRegion> wrapper;

public:
	~SpineTextureRegion() override;

	// Build the synthetic region from `texture`, sampling the normalized
	// sub-rect `uv_rect` (default: full texture). Returns false on a null
	// texture or if already set up (one region per object).
	bool setup(Ref<Texture2D> texture, Rect2 uv_rect);

	// The remappable wrapper to hand to SpineAttachment.set_region.
	Ref<SpineAtlasRegion> get_region() const { return wrapper; }
};

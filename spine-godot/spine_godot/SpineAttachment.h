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
#include "spine/Attachment.h"
#include <spine/spine.h>

class SpineAtlasRegion;

class SpineSkeletonDataResource;

class SpineAttachment : public SpineSkeletonDataResourceOwnedObject<spine::Attachment> {
	GDCLASS(SpineAttachment, SpineObjectWrapper)

protected:
	static void _bind_methods();

public:
	~SpineAttachment() override;

	String get_attachment_name();

	Ref<SpineAttachment> copy();

	// Hommlet patch: rebind a region/mesh attachment's UVs onto a different
	// atlas region. Mirrors Spine.Unity's Attachment.GetRemappedClone behavior
	// at the C++ runtime level (RegionAttachment::setRegion + updateRegion()
	// or MeshAttachment::setRegion + updateRegion(), dispatched via RTTI).
	// Returns false if the attachment kind has no region (e.g. BoundingBox).
	bool set_region(Ref<SpineAtlasRegion> region);

	// DEAD MONEY: per-attachment mask LUT index used by Hommlet's
	// character.gdshader. Emitted into ARRAY_CUSTOM0.y at mesh build time.
	float get_mask_index();
	void set_mask_index(float v);

	// DEAD MONEY (test support): first resolved texture-U of a region/mesh
	// attachment's setup-index sequence entry — the U the renderer samples.
	// Lets tests assert that set_region actually rebound the rendered UVs.
	// Returns -1 for kinds with no region (BoundingBox, Path, …).
	float get_first_uv();

	void set_spine_object(const SpineSkeletonDataResource *_owner, spine::Attachment *_object) override {
		if (get_spine_object()) get_spine_object()->dereference();
		_set_spine_object_internal(_owner, _object);
		if (_object) _object->reference();
	}
};

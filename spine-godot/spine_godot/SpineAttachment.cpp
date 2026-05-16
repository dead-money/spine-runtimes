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

#include "SpineAttachment.h"
#include "SpineAtlasRegion.h"
#include "SpineCommon.h"
#include <spine/RegionAttachment.h>
#include <spine/MeshAttachment.h>

void SpineAttachment::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_attachment_name"), &SpineAttachment::get_attachment_name);
	ClassDB::bind_method(D_METHOD("copy"), &SpineAttachment::copy);
	ClassDB::bind_method(D_METHOD("set_region", "region"), &SpineAttachment::set_region);
	ClassDB::bind_method(D_METHOD("get_mask_index"), &SpineAttachment::get_mask_index);
	ClassDB::bind_method(D_METHOD("set_mask_index", "v"), &SpineAttachment::set_mask_index);
}

SpineAttachment::~SpineAttachment() {
	if (get_spine_object()) get_spine_object()->dereference();
}

String SpineAttachment::get_attachment_name() {
	SPINE_CHECK(get_spine_object(), "")
	String name;
#if (VERSION_MAJOR >= 4 && VERSION_MINOR >= 5)
	name = String::utf8(get_spine_object()->getName().buffer());
#else
	name.parse_utf8(get_spine_object()->getName().buffer());
#endif
	return name;
}

Ref<SpineAttachment> SpineAttachment::copy() {
	SPINE_CHECK(get_spine_object(), nullptr)
	auto copy = &get_spine_object()->copy();
	if (!copy) return nullptr;
	Ref<SpineAttachment> attachment_ref(memnew(SpineAttachment));
	attachment_ref->set_spine_object(get_spine_owner(), copy);
	return attachment_ref;
}

bool SpineAttachment::set_region(Ref<SpineAtlasRegion> region) {
	SPINE_CHECK(get_spine_object(), false)
	if (region.is_null() || !region->get_region()) return false;

	auto *att = get_spine_object();
	auto *tex_region = static_cast<spine::TextureRegion *>(region->get_region());

	// 4.3 moved per-attachment region storage onto Sequence. Rebind by
	// overwriting every entry in the Sequence's regions array with the new
	// region and refreshing the cached UVs/offsets via Sequence::update().
	auto &rtti = att->getRTTI();
	if (rtti.isExactly(spine::RegionAttachment::rtti)) {
		auto *r = static_cast<spine::RegionAttachment *>(att);
		auto &seq = r->getSequence();
		auto &regions = seq.getRegions();
		for (size_t i = 0; i < regions.size(); i++) regions[i] = tex_region;
		seq.update(*r);
		return true;
	}
	if (rtti.isExactly(spine::MeshAttachment::rtti)) {
		auto *m = static_cast<spine::MeshAttachment *>(att);
		auto &seq = m->getSequence();
		auto &regions = seq.getRegions();
		for (size_t i = 0; i < regions.size(); i++) regions[i] = tex_region;
		seq.update(*m);
		return true;
	}
	// BoundingBox / Path / Point / Clipping — no region.
	return false;
}

float SpineAttachment::get_mask_index() {
	SPINE_CHECK(get_spine_object(), 0.0f)
	return get_spine_object()->getMaskIndex();
}

void SpineAttachment::set_mask_index(float v) {
	SPINE_CHECK(get_spine_object(), )
	get_spine_object()->setMaskIndex(v);
}

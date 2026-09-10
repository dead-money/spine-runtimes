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
#include "SpineSlot.h"
#include "SpineSprite.h"
#include <spine/Skeleton.h>
#include <cmath>
#include <spine/RegionAttachment.h>
#include <spine/MeshAttachment.h>

void SpineAttachment::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_attachment_name"), &SpineAttachment::get_attachment_name);
	ClassDB::bind_method(D_METHOD("copy"), &SpineAttachment::copy);
	ClassDB::bind_method(D_METHOD("set_region", "region"), &SpineAttachment::set_region);
	ClassDB::bind_method(D_METHOD("get_mask_index"), &SpineAttachment::get_mask_index);
	ClassDB::bind_method(D_METHOD("set_mask_index", "v"), &SpineAttachment::set_mask_index);
	ClassDB::bind_method(D_METHOD("get_first_uv"), &SpineAttachment::get_first_uv);
	ClassDB::bind_method(D_METHOD("map_region_points", "slot", "region_uvs"), &SpineAttachment::map_region_points);
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

float SpineAttachment::get_first_uv() {
	SPINE_CHECK(get_spine_object(), -1.0f)
	auto *att = get_spine_object();
	auto &rtti = att->getRTTI();
	spine::Sequence *seq = nullptr;
	if (rtti.isExactly(spine::RegionAttachment::rtti)) {
		seq = &static_cast<spine::RegionAttachment *>(att)->getSequence();
	} else if (rtti.isExactly(spine::MeshAttachment::rtti)) {
		seq = &static_cast<spine::MeshAttachment *>(att)->getSequence();
	}
	if (!seq) return -1.0f;
	spine::Array<float> &uvs = seq->getUVs(seq->getSetupIndex());
	return uvs.size() > 0 ? uvs[0] : -1.0f;
}

static void map_points_through_triangles(const float *world, const float *uvs, const unsigned short *tris, size_t tri_count,
		const PackedVector2Array &region_uvs, PackedVector2Array &out) {
	for (int p = 0; p < region_uvs.size(); p++) {
		float px = region_uvs[p].x, py = region_uvs[p].y;
		float best_min = -1e30f;
		Vector2 best;
		for (size_t t = 0; t < tri_count; t++) {
			int i0 = tris[t * 3], i1 = tris[t * 3 + 1], i2 = tris[t * 3 + 2];
			float u0 = uvs[i0 * 2], v0 = uvs[i0 * 2 + 1];
			float u1 = uvs[i1 * 2], v1 = uvs[i1 * 2 + 1];
			float u2 = uvs[i2 * 2], v2 = uvs[i2 * 2 + 1];
			float det = (u1 - u0) * (v2 - v0) - (u2 - u0) * (v1 - v0);
			if (fabsf(det) < 1e-12f) continue;
			float l1 = ((px - u0) * (v2 - v0) - (u2 - u0) * (py - v0)) / det;
			float l2 = ((u1 - u0) * (py - v0) - (px - u0) * (v1 - v0)) / det;
			float l0 = 1.0f - l1 - l2;
			float m = fminf(l0, fminf(l1, l2));
			if (m > best_min) {
				best_min = m;
				best = Vector2(
						l0 * world[i0 * 2] + l1 * world[i1 * 2] + l2 * world[i2 * 2],
						l0 * world[i0 * 2 + 1] + l1 * world[i1 * 2 + 1] + l2 * world[i2 * 2 + 1]);
			}
			if (m >= 0.0f) break;
		}
		out.push_back(best);
	}
}

PackedVector2Array SpineAttachment::map_region_points(Ref<SpineSlot> slot, PackedVector2Array region_uvs) {
	PackedVector2Array out;
	SPINE_CHECK(get_spine_object(), out)
	if (slot.is_null() || !slot->get_spine_object() || region_uvs.size() == 0) return out;
	auto *att = get_spine_object();
	spine::Slot &spine_slot = *slot->get_spine_object();
	auto &rtti = att->getRTTI();
	if (rtti.isExactly(spine::RegionAttachment::rtti)) {
		auto *region = static_cast<spine::RegionAttachment *>(att);
		spine::Array<float> &offsets = region->getOffsets(spine_slot.getAppliedPose());
		float world[8];
		region->computeWorldVertices(spine_slot, offsets.buffer(), world, 0, 2);
		// computeWorldVertices order: BR, BL, UL, UR.
		static const float uvs[8] = { 1, 1, 0, 1, 0, 0, 1, 0 };
		static const unsigned short tris[6] = { 0, 1, 2, 2, 3, 0 };
		map_points_through_triangles(world, uvs, tris, 2, region_uvs, out);
		return out;
	}
	if (rtti.isExactly(spine::MeshAttachment::rtti)) {
		auto *mesh = static_cast<spine::MeshAttachment *>(att);
		SpineSprite *sprite = slot->get_spine_owner();
		if (!sprite || sprite->get_skeleton().is_null() || !sprite->get_skeleton()->get_spine_object()) return out;
		spine::Skeleton &skeleton = *sprite->get_skeleton()->get_spine_object();
		size_t count = mesh->getWorldVerticesLength();
		spine::Array<float> world;
		world.setSize(count, 0.0f);
		mesh->computeWorldVertices(skeleton, spine_slot, 0, count, world.buffer(), 0, 2);
		spine::Array<float> &uvs = mesh->getRegionUVs();
		spine::Array<unsigned short> &tris = mesh->getTriangles();
		if (uvs.size() < count || tris.size() < 3) return out;
		map_points_through_triangles(world.buffer(), uvs.buffer(), tris.buffer(), tris.size() / 3, region_uvs, out);
		return out;
	}
	return out;
}

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

#include "SpineTextureRegion.h"
#include "SpineAtlasRegion.h"

void SpineTextureRegion::_bind_methods() {
	ClassDB::bind_method(D_METHOD("setup", "texture", "uv_rect"), &SpineTextureRegion::setup, DEFVAL(Rect2(0, 0, 1, 1)));
	ClassDB::bind_method(D_METHOD("get_region"), &SpineTextureRegion::get_region);
}

SpineTextureRegion::~SpineTextureRegion() {
	// Drop the wrapper first so nothing dereferences `region` past this point.
	wrapper.unref();

	// AtlasRegion / AtlasPage are spine objects (operator delete is overridden
	// by spine's allocator). They don't own `renderer_object` — the page only
	// holds a void* into it — so we free that ourselves, last.
	if (region) {
		delete region;
		region = nullptr;
	}
	if (page) {
		delete page;
		page = nullptr;
	}
	if (renderer_object) {
		if (renderer_object->texture.is_valid()) renderer_object->texture.unref();
#if VERSION_MAJOR > 3
		if (renderer_object->canvas_texture.is_valid()) renderer_object->canvas_texture.unref();
#endif
		memdelete(renderer_object);
		renderer_object = nullptr;
	}
}

bool SpineTextureRegion::setup(Ref<Texture2D> texture, Rect2 uv_rect) {
	if (texture.is_null()) return false;
	if (region) return false;

	const int tex_w = texture->get_width();
	const int tex_h = texture->get_height();

	// Mirror SpineAtlasResource's page renderer-object construction: the draw
	// path reads region->getPage()->texture as a SpineRendererObject*, pulling
	// the diffuse RID from canvas_texture.
	renderer_object = memnew(SpineRendererObject);
	renderer_object->texture = texture;
	renderer_object->normal_map = Ref<Texture>(nullptr);
	renderer_object->specular_map = Ref<Texture>(nullptr);
#if VERSION_MAJOR > 3
	renderer_object->canvas_texture.instantiate();
	renderer_object->canvas_texture->set_diffuse_texture(renderer_object->texture);
	renderer_object->canvas_texture->set_normal_texture(renderer_object->normal_map);
	renderer_object->canvas_texture->set_specular_texture(renderer_object->specular_map);
#endif

	page = new (__FILE__, __LINE__) spine::AtlasPage(spine::String("hommlet_icon"));
	page->texture = (void *) renderer_object;
	page->width = tex_w;
	page->height = tex_h;

	const int region_w = (int) (uv_rect.size.width * tex_w + 0.5f);
	const int region_h = (int) (uv_rect.size.height * tex_h + 0.5f);

	// No trim, no rotation: original == packed, zero offset. RegionAttachment::
	// computeUVs reads u/v/u2/v2 plus offset/original/packed/degrees; with these
	// values it maps the (sub-)texture cleanly onto the attachment quad.
	region = new (__FILE__, __LINE__) spine::AtlasRegion();
	region->setPage(page);
	region->setName(spine::String("hommlet_icon"));
	region->setX((int) (uv_rect.position.x * tex_w + 0.5f));
	region->setY((int) (uv_rect.position.y * tex_h + 0.5f));
	region->setU(uv_rect.position.x);
	region->setV(uv_rect.position.y);
	region->setU2(uv_rect.position.x + uv_rect.size.width);
	region->setV2(uv_rect.position.y + uv_rect.size.height);
	region->setRegionWidth(region_w);
	region->setRegionHeight(region_h);
	region->setOffsetX(0);
	region->setOffsetY(0);
	region->setPackedWidth(region_w);
	region->setPackedHeight(region_h);
	region->setOriginalWidth(region_w);
	region->setOriginalHeight(region_h);
	region->setRotate(false);
	region->setDegrees(0);

	wrapper = Ref<SpineAtlasRegion>(memnew(SpineAtlasRegion));
	wrapper->set_region(region);
	return true;
}

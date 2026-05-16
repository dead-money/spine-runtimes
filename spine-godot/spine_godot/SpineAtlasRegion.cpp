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

#include "SpineAtlasRegion.h"

void SpineAtlasRegion::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &SpineAtlasRegion::get_name);
	ClassDB::bind_method(D_METHOD("get_x"), &SpineAtlasRegion::get_x);
	ClassDB::bind_method(D_METHOD("get_y"), &SpineAtlasRegion::get_y);
	ClassDB::bind_method(D_METHOD("get_width"), &SpineAtlasRegion::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &SpineAtlasRegion::get_height);
	ClassDB::bind_method(D_METHOD("get_original_width"), &SpineAtlasRegion::get_original_width);
	ClassDB::bind_method(D_METHOD("get_original_height"), &SpineAtlasRegion::get_original_height);
	ClassDB::bind_method(D_METHOD("get_offset_x"), &SpineAtlasRegion::get_offset_x);
	ClassDB::bind_method(D_METHOD("get_offset_y"), &SpineAtlasRegion::get_offset_y);
	ClassDB::bind_method(D_METHOD("get_u"), &SpineAtlasRegion::get_u);
	ClassDB::bind_method(D_METHOD("get_v"), &SpineAtlasRegion::get_v);
	ClassDB::bind_method(D_METHOD("get_u2"), &SpineAtlasRegion::get_u2);
	ClassDB::bind_method(D_METHOD("get_v2"), &SpineAtlasRegion::get_v2);
	ClassDB::bind_method(D_METHOD("get_degrees"), &SpineAtlasRegion::get_degrees);
}

String SpineAtlasRegion::get_name() const {
	if (!region) return String();
	String name;
#if (VERSION_MAJOR >= 4 && VERSION_MINOR >= 5)
	name = String::utf8(region->getName().buffer());
#else
	name.parse_utf8(region->getName().buffer());
#endif
	return name;
}

int SpineAtlasRegion::get_x() const { return region ? region->getX() : 0; }
int SpineAtlasRegion::get_y() const { return region ? region->getY() : 0; }
int SpineAtlasRegion::get_width() const { return region ? region->getPackedWidth() : 0; }
int SpineAtlasRegion::get_height() const { return region ? region->getPackedHeight() : 0; }
int SpineAtlasRegion::get_original_width() const { return region ? region->getOriginalWidth() : 0; }
int SpineAtlasRegion::get_original_height() const { return region ? region->getOriginalHeight() : 0; }
int SpineAtlasRegion::get_offset_x() const { return region ? (int) region->getOffsetX() : 0; }
int SpineAtlasRegion::get_offset_y() const { return region ? (int) region->getOffsetY() : 0; }
float SpineAtlasRegion::get_u() const { return region ? region->getU() : 0.0f; }
float SpineAtlasRegion::get_v() const { return region ? region->getV() : 0.0f; }
float SpineAtlasRegion::get_u2() const { return region ? region->getU2() : 0.0f; }
float SpineAtlasRegion::get_v2() const { return region ? region->getV2() : 0.0f; }
int SpineAtlasRegion::get_degrees() const { return region ? region->getDegrees() : 0; }

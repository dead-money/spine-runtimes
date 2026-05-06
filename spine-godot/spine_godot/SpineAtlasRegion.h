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
#include <spine/Atlas.h>

// Lightweight wrapper around spine::AtlasRegion*. AtlasRegions live on the
// underlying spine::Atlas, owned by SpineAtlasResource — as long as the
// resource is held alive, the region pointer is stable.
class SpineAtlasRegion : public REFCOUNTED {
	GDCLASS(SpineAtlasRegion, REFCOUNTED)

protected:
	static void _bind_methods();
	spine::AtlasRegion *region = nullptr;

public:
	void set_region(spine::AtlasRegion *r) { region = r; }
	spine::AtlasRegion *get_region() const { return region; }

	String get_name() const;
	int get_x() const;
	int get_y() const;
	int get_width() const;
	int get_height() const;
	int get_original_width() const;
	int get_original_height() const;
	int get_offset_x() const;
	int get_offset_y() const;
	float get_u() const;
	float get_v() const;
	float get_u2() const;
	float get_v2() const;
	int get_degrees() const;
};

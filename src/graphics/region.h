//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**

A REGION is a list of non-overlapping RECTs. We keep the RECTs 
non-overlapped on the assumption that minimizing overdraw
more than pays for the small extra complexity in addRect().
*/

typedef struct REGION {
	vector<RECT> rects;

	void addRect(RECT rect);
	void addRegion(const REGION& region);
	REGION intersect(RECT rect);
    bool intersects(RECT rect);

} REGION;


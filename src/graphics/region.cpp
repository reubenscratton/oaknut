//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

class RegionTest {
public:
	RegionTest() {
		REGION oregion;
		oregion.addRect(RECT_Make(0,0,10,10));
		
		// Verify that adding a rect already in the region has no effect
		REGION region = oregion;
		region.addRect(RECT_Make(2, 2, 1, 1));
		assert(region.rects.size()==1 && region.rects[0]==RECT_Make(0,0,10,10));

		// Overlap left edge only
		region = oregion;
		region.addRect(RECT_Make(-5, 2, 10, 5));
		assert(region.rects.size()==2 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(-5,2,5,5));

		// Overlap left & top & bottom edges
		region = oregion;
		region.addRect(RECT_Make(-1, -1, 2, 12));
		assert(region.rects.size()==4 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(-1,-1,2,1)
									  && region.rects[2]==RECT_Make(-1,10,2,1)
									  && region.rects[3]==RECT_Make(-1,0,1,10));

		// Overlap right & top & bottom edges
		/*region = oregion;
		region.addRect(RECT_Make(5, -1, 10, 12));
		assert(region.rects.size()==3 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(5,-1,10,1)
									  && region.rects[2]==RECT_Make(5,10,10,1));

		// Overlap left, right, top edges
		region = oregion;
		region.addRect(RECT_Make(-1, -1, 12, 2));
		assert(region.rects.size()==2 && region.rects[0]==RECT_Make(0,1,10,9)
									  && region.rects[1]==RECT_Make(-1,-1,12,2));

		// Overlap left, right, bottom edges
		region = oregion;
		region.addRect(RECT_Make(-1, 9, 12, 2));
		assert(region.rects.size()==2 && region.rects[0]==RECT_Make(0,0,10,9)
									  && region.rects[1]==RECT_Make(-1,9,12,2));*/

		// Overlap right edge only
		region = oregion;
		region.addRect(RECT_Make(5, 2, 10, 5));
		assert(region.rects.size()==2 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(10,2,5,5));

		// Add to top edge
		region = oregion;
		region.addRect(RECT_Make(2, -5, 5, 10));
		assert(region.rects.size()==2 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(2,-5,5,5));

		// Add to bottom edge
		region = oregion;
		region.addRect(RECT_Make(2, 5, 5, 10));
		assert(region.rects.size()==2 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(2,10,5,5));
		
		// Overlap top-left corner
		region = oregion;
		region.addRect(RECT_Make(5, 5, 10, 10));
		assert(region.rects.size()==3 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(5,10,10,5)
									  && region.rects[2]==RECT_Make(10,5,5,5));
		// Overlap top-right
		region = oregion;
		region.addRect(RECT_Make(-5, 5, 10, 10));
		assert(region.rects.size()==3 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(-5,10,10,5)
									  && region.rects[2]==RECT_Make(-5,5,5,5));

		// Overlap bottom-left corner
		region = oregion;
		region.addRect(RECT_Make(5, 5, 10, 10));
		assert(region.rects.size()==3 && region.rects[0]==RECT_Make(0,0,10,10)
									  && region.rects[1]==RECT_Make(5,10,10,5)
									  && region.rects[2]==RECT_Make(10,5,5,5));
	}
};


//static RegionTest _test;

typedef struct {
	bool tl;
	bool tr;
	bool bl;
	bool br;
} CORNERS;

static int findContainedCorners(const RECT& r1, const RECT& r2, CORNERS& corners) {
	// NB: Usage of >= & > is careful and deliberate for end-exclusive RECTs
	corners.tl = (r1.left()>=r2.left() && r1.left()<r2.right()) && (r1.top()>=r2.top() && r1.top()<r2.bottom());
	corners.tr = (r1.right()>r2.left() && r1.right()<r2.right()) && (r1.top()>=r2.top() && r1.top()<r2.bottom());
	corners.bl = (r1.left()>=r2.left() && r1.left()<r2.right()) && (r1.bottom()>r2.top() && r1.bottom()<r2.bottom());
	corners.br = (r1.right()>r2.left() && r1.right()<r2.right()) && (r1.bottom()>r2.top() && r1.bottom()<r2.bottom());
	int num=0;
	if (corners.tl) num++;
	if (corners.tr) num++;
	if (corners.bl) num++;
	if (corners.br) num++;
	return num;
}


void REGION::addRect(RECT arect) {

	arect.origin.x = floorf(arect.origin.x);
	arect.origin.y = floorf(arect.origin.y);
	arect.size.width = floorf(arect.size.width);
	arect.size.height = floorf(arect.size.height);
	if(arect.size.width<=0 || arect.size.height<=0) return;
	
#if 0
	rects.push_back(arect);
#else
	// Start a list with just the new rect
	vector<RECT> newrects;
	newrects.push_back(arect);
	
	// Iterate over new rects (initially just one)
	CORNERS corners;
	for (int i=0 ; i<newrects.size() ; i++) {
		RECT& nr = newrects[i];

		// Iterate existing region rects
		for (int j=0 ; j<rects.size() ; j++) {
			RECT& rr = rects.at(j);
			
			// If region rect wholly encloses the new rect, remove it from newrects and break the inner loop
			if (4 == findContainedCorners(nr, rr, corners)) {
				newrects.erase(newrects.begin() + i);
				j = (int)rects.size()-1;
				break;
			}
			
			// If newrect wholly encloses region rect then region rect is redundant, remove it
			if (4 == findContainedCorners(rr, nr, corners)) {
				rects.erase(rects.begin()+j);
				j--;
				continue;
			}
			
			
			// Rects overlap at all?
			if (rr.intersects(nr)) {
			
				// Get the rects to all four sides of rr that intersect nr
				RECT rAbove = RECT_Make(nr.left(), nr.top(), nr.size.width, rr.top()-nr.top());
				RECT rBelow = RECT_Make(nr.left(), rr.bottom(), nr.size.width, nr.bottom()-rr.bottom());
				float t = fmaxf(rr.top(), nr.top());
				float b = fminf(rr.bottom(), nr.bottom());
				RECT rLeft = RECT_Make(nr.left(), t, rr.left()-nr.left(), b-t);
				RECT rRight = RECT_Make(rr.right(), t, nr.right()-rr.right(), b-t);
				
				// Add the ones that aren't degenerate to the list
				if (!rAbove.isEmpty()) {
					newrects.push_back(rAbove);
				}
				if (!rBelow.isEmpty()) {
					newrects.push_back(rBelow);
				}
				if (!rLeft.isEmpty()) {
					newrects.push_back(rLeft);
				}
				if (!rRight.isEmpty()) {
					newrects.push_back(rRight);
				}
				
				// Remove the current newrect, it's been split
				newrects.erase(newrects.begin()+i);
				i--;
				break;
			
			}
		}
	}

	// Add the new rects to the region
	for (int i=0 ; i<newrects.size() ; i++) {
		RECT& nr = newrects[i];
		if (nr.size.width>0 && nr.size.height>0) {
			rects.push_back(nr);
		}
	}
#endif
}

REGION REGION::intersect(RECT rect) {
	REGION rgn;
	for (int i=0 ; i<rects.size() ; i++) {
		RECT rr = rects[i];
		if (rr.intersectWith(rect)) {
			rgn.rects.push_back(rr);
		}
	}
	return rgn;
}
bool REGION::intersects(RECT rect) {
    // TODO: implement a union rect for fast-path testing
    for (int i=0 ; i<rects.size() ; i++) {
        if (rects[i].intersectWith(rect)) {
            return true;
        }
    }
    return false;
}


void REGION::addRegion(const REGION& region) {
	for (int i=0; i<region.rects.size() ; i++) {
		addRect(region.rects[i]);
	}
}


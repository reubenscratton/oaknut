//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellsmodule.h"
#include "containermodule.h"
#include "../../model/_module.h"
#include "cell.h"
#include "cellitem.h"

DECLARE_DYNCREATE(BNCellsModule, const variant&);

BNCellsModule::BNCellsModule(const variant& json) : BNModule(json) {
    _limit = json.intVal("limit");
    _offset = json.intVal("offset");
    _primary = json.stringArrayVal("primary");
    _secondary = json.stringArrayVal("secondary");
    _format = json.stringArrayVal("format");
    _cellClass = json.stringVal("cellClass");
    _cellMargins = edgeInsetsVal(json, "cellMargins");
    _cellPadding = edgeInsetsVal(json, "cellPadding");
    _usesScrollviewOffset = json.intVal("usesScrollviewOffset")!=0;
    _cellsPerRow = json.intVal("cellsPerRow");
    if (_cellsPerRow == 0) {
        _cellsPerRow = 1;
    }
    _allowIncompleteRows = json.intVal("allowIncompleteRows")!=0;
    _maxTopics = 1;
    _maxTopics = json.intVal("maxTopics");
    _summaries = json.intVal("summaries")!=0;
    _hideTopics = json.intVal("hideTopics")!=0;
    _showMediaGlyphInHeadline = json.intVal("showMediaGlyphInHeadline")!=0;
    _imageWidthSpec = json.floatVal("imageWidth");
    if (_imageWidthSpec<=0.f) {
        _imageWidthSpec = 0.5f;
    }
    _H = json.intVal("H");
    if (_H <= 0) {
        _H = 3;
    }
    _tinyTimestamps = json.intVal("tinyTimestamps")!=0;
}

// Cloning
BNCellsModule::BNCellsModule(BNCellsModule* source) : BNModule(source) {
    _limit = source->_limit;
    _offset = source->_offset;
    _primary = source->_primary;
    _secondary = source->_secondary;
    _format = source->_format;
    _cellClass = source->_cellClass;
    _cellMargins = source->_cellMargins;
    _cellPadding = source->_cellPadding;
    _usesScrollviewOffset = source->_usesScrollviewOffset;
    _cellsPerRow = source->_cellsPerRow;
    _allowIncompleteRows = source->_allowIncompleteRows;
    _maxTopics = source->_maxTopics;
    _summaries = source->_summaries;
    _hideTopics = source->_hideTopics;
    _showMediaGlyphInHeadline = source->_showMediaGlyphInHeadline;
    _imageWidthSpec = source->_imageWidthSpec;
    _H = source->_H;
    _tinyTimestamps = source->_tinyTimestamps;

}
BNModule* BNCellsModule::clone() {
    return new BNCellsModule(this);
}

static bool timeSortFunc(BNRelationship* rel1, BNRelationship* rel2) {
    return rel1->_childObject->_lastUpdated < rel2->_childObject->_lastUpdated;
}
static bool secondarySortFunc(BNRelationship* rel1, BNRelationship* rel2) {
    return rel1->_secondaryType.compare(rel2->_secondaryType) < 0;
}
static bool displayOrderSortFunc(BNRelationship* rel1, BNRelationship* rel2) {
    return rel1->_displayOrder < rel2->_displayOrder;
}

void BNCellsModule::updateLayoutWithContentObject(BNContent* contentObject) {
    BNModule::updateLayoutWithContentObject(contentObject);
    for (auto cell: _cells) {
        cell->removeFromParent();
    }
    _cells.clear();
	
	// Some modules expect to use the given content object as its data source, others expect
	// to abct upon a filtered array of the content objects' children.
    vector<BNRelationship*> rels;
    BNItem* item = NULL;
	if (_primary.size()==0 && _secondary.size()==0) {
		item = _contentObject.as<BNItem>();
	}
	else {
		if (_secondary.size()==0) {
            rels = contentObject->findRelationships(_primary, {}, _format);
		} else{
            for (auto secondaryType : _secondary) {
                auto moreRels = contentObject->findRelationships(_primary, {secondaryType}, _format);
                if (moreRels.size()) {
                    rels.insert(rels.end(), moreRels.begin(), moreRels.end());
                }
			}
		}
		if (rels.size() ==0) {
			return;
		}
	}
    string orderBy = _json.stringVal("orderBy");

	if (orderBy == "avcarousel") {
		if (_secondary.size() > 0) {
			string firstType = _secondary[0];
			if (firstType.length() > 0) {
                auto secondItems = contentObject->findRelationships(_primary, {firstType}, _format);
				if (secondItems.size() > 0) {
                    rels.clear();
                    rels.insert(rels.begin(), secondItems.begin(), secondItems.end());
				}
			}
		}
	}
	
	
	// Stupid LEP video filtering... remove the isAvailable=NO videos.
	if (_json.intVal("filterIsAvailable")) {
		for (int i=0 ; i<rels.size() ; i++) {
			BNRelationship* rel = rels[i];
			if (rel->_childObject->isAV()) {
				BNAV* av = (BNAV*)rel->_childObject;
				if (!av->_isAvailable) {
                    rels.erase(rels.begin() + i);
                    i--;
				}
			}
		}
		
	}
	
	// Dedupe
	if (_json.intVal("dedupe")) {
		set<string> set;
		for (int i=0 ; i<rels.size() ; i++) {
			BNRelationship* rel = rels[i];
			if (set.find(rel->_childObject->_modelId) != set.end()) {
				rels.erase(rels.begin() + i);
            } else {
                set.insert(rel->_childObject->_modelId);
            }
		}
	}
	
	// Order by
	if (orderBy.length()) {
		if (orderBy == "time") {
            std::sort(rels.begin(), rels.end(), timeSortFunc);
		} else if (orderBy == "secondary") {
            std::sort(rels.begin(), rels.end(), secondarySortFunc);
		} else {
            std::sort(rels.begin(), rels.end(), displayOrderSortFunc);
		}
	}

	// Apply limit and offset
	int dummyNum = 0;
	bool dummyEndCell = false;
	if (_offset > 0 && _offset<rels.size()) {
		rels.erase(rels.begin(), rels.begin() + MIN(_offset, rels.size()));
	}
	if (_limit > 0 && rels.size() > _limit) {
		dummyNum = (int)rels.size() - _limit;
        rels.erase(rels.begin()+_limit, rels.end());
		dummyEndCell = _json.intVal("dummyEndCell")!=0;
	}
	
	// Sometimes we don't want incomplete rows
	if (_cellsPerRow > 1 && rels.size() > 0 && !_allowIncompleteRows) {
		int modulus = rels.size() % (int)_cellsPerRow;
		if (modulus > 0) {
			rels.erase(rels.begin() + rels.size()-modulus, rels.end());
		}
	}

	
	// Iterate over items to show in the module
	int count = (int)rels.size();
    if (!count && item) {
        BNCell* cell = (BNCell*)Object::createByName(_cellClass, this);
        cell->_cellIndex = _offset;
        cell->_isOnScreen = _isOnScreen;
        cell->setItem(item);
        _cells.push_back(cell);
    } else {
        
        for (int i=0 ; i<count ; i++) {
            BNRelationship* rel = rels[i];
            BNCell* cell = (BNCell*)Object::createByName(_cellClass, this);
            cell->_cellIndex = i + _offset;
            cell->_isOnScreen = _isOnScreen;
            if (item) {
                cell->setItem(item);
            } else {
                cell->setRelationship(rel);
            }
            _cells.push_back(cell);
            if (item) {
                break;
            }
        }
    }

    if (dummyEndCell) {
        app->log("TODO: dummy cell");
        //cell->setDummyInfo(string::format(" +%ld more", (long)dummyNum));
    }

}

void BNCellsModule::addToView(View* parent) {
    for (int i=0 ; i<_cells.size() ; i++) {
        BNCell* cell = _cells[i];
        cell->_margins = _cellMargins;
        parent->addSubview(cell);
        /*
        _cellMargins.applyToRect(cellRect);
        cellRect.size.height = floorf(cellRect.size.height);
        //cellRect = CGRectIntegral(cellRect);
        cell->measureForContainingRect(cellRect);
        cell->_frame.origin = cellRect.origin;
        cell->_frame.size.height = ceilf(cell->_frame.size.height);
        rowHeight = MAX(rowHeight, cell->_frame.size.height);
        cellRect = cell->_frame.copyWithUninsets(_cellMargins);
        bounds = bounds.unionWith(cellRect);
        cellsThisRow++;
        if (--cellsPerRow) {
            cellRect.origin.x += cellRect.size.width;
            continue;
        }
        
        // End of row. Now ensure all cells in the row have the same height
        if (cellsThisRow > 1) {
            for (int j=0 ; j<cellsThisRow ; j++) {
                BNCell* rowCell = _cells[i-j];
                rowCell->extendToHeight(rowHeight);
            }
            cellRect = cell->_frame.copyWithUninsets(_cellMargins);
        }
        
        // Advance to next row
        cellRect = _container->boundsAfter(cellRect);
        if (_cellsPerRow > 1) {
            cellRect.origin.x = rect.origin.x;
        }
        cellsPerRow = _cellsPerRow;
        cellsThisRow = 0;
        rowHeight = 0;*/
    }
    //bounds.size.width += _padding.right;
    //bounds.size.height += _padding.bottom;

}

/*
void BNCellsModule::layoutWithContainingRect(const RECT& arect) {
    RECT rect = arect;
    RECT bounds = {rect.origin.x, rect.origin.y, 0,0};
	if (_cells.size() > 0) {
		_padding.applyToRect(rect);
		RECT cellRect = rect;
		cellRect.size.width /= _cellsPerRow;
		int cellsPerRow = _cellsPerRow;
		int cellsThisRow = 0;
		float rowHeight = 0;
		for (int i=0 ; i<_cells.size() ; i++) {
			BNCell* cell = _cells[i];
			_cellMargins.applyToRect(cellRect);
			cellRect.size.height = floorf(cellRect.size.height);
			//cellRect = CGRectIntegral(cellRect);
			cell->measureForContainingRect(cellRect);
			cell->_frame.origin = cellRect.origin;
			cell->_frame.size.height = ceilf(cell->_frame.size.height);
			rowHeight = MAX(rowHeight, cell->_frame.size.height);
            cellRect = cell->_frame.copyWithUninsets(_cellMargins);
			bounds = bounds.unionWith(cellRect);
			cellsThisRow++;
			if (--cellsPerRow) {
				cellRect.origin.x += cellRect.size.width;
				continue;
			}
			
			// End of row. Now ensure all cells in the row have the same height
			if (cellsThisRow > 1) {
				for (int j=0 ; j<cellsThisRow ; j++) {
					BNCell* rowCell = _cells[i-j];
					rowCell->extendToHeight(rowHeight);
				}
				cellRect = cell->_frame.copyWithUninsets(_cellMargins);
			}
			
			// Advance to next row
			cellRect = _container->boundsAfter(cellRect);
			if (_cellsPerRow > 1) {
				cellRect.origin.x = rect.origin.x;
			}
			cellsPerRow = _cellsPerRow;
			cellsThisRow = 0;
			rowHeight = 0;
		}
		bounds.size.width += _padding.right;
		bounds.size.height += _padding.bottom;
	}
	_frame = bounds;
}

void BNCellsModule::extendToHeight(float height) {
	// NB: This is naive in that it assumes all cells are bottom-aligned.
	height -= _cellMargins.top + _cellMargins.bottom;
    for (BNCell* cell : _cells) {
		cell->extendToHeight(height);
	}
}
*/
void BNCellsModule::setIsOnScreen(bool isOnScreen) {
    BNModule::setIsOnScreen(isOnScreen);
    for (BNCell* cell : _cells) {
		cell->_isOnScreen = isOnScreen;
	}	
}

void BNCellsModule::onIsInitialContent() {
    BNModule::onIsInitialContent();
    for (BNCell* cell : _cells) {
        cell->onIsInitialContent();
	}
}


void BNCellsModule::insertSubview(View* superview, View* newView, int index) {
	superview->insertSubview(newView, index);
}

/*
void BNCellsModule::updateSubviews(View* superview) {
	RECT bounds = superview->getOwnRect();
	bounds.size.height += 180; // i.e. extend visible area below screen, thereby fetching nearby images early
    for (BNCell* cell : _cells) {
		if (superview && bounds.intersects(cell->_frame)) {
			if (!cell->_view) {
				//NSLog(@"Creating cell: %@", NSStringFromClass(cell.class));
				cell->createView(superview);
				// Add the cell view to superview (scrollview). It's important to keep cell views z-ordered
				// so that lower cells are on top of higher ones. Otherwise parallax and BNTextView clip
				// other views in unexpected ways.
				int index = 0;
                superview->iterateSubviews([&] (View* subview) -> bool {
                    if (!subview->isHidden() && subview->getTop() > cell->_frame.origin.y) {
                        return false;
                    }
                    index++;
                    return true;
                });
                insertSubview(superview, cell->_view, index);
			}
			if (_isOnScreen && cell->_prefetchLevel <= 0) {
				if (superview->getRect().intersects(cell->_frame)) {
					cell->setPrefetchLevel(1);
				}
			}
            app->log("Todo: need the scroll offset hack?");
			//if (cell->_usesScrollviewOffset && [superview isKindOfClass:[UIScrollView class]]) {
			//	UIScrollView* scrollView = (UIScrollView*)superview;
			//	[cell adviseScrollviewOffset:scrollView.contentOffset.y + scrollView.contentInset.top];
			//}
			
		} else {
			if (cell->_view) {
				//NSLog(@"Deleting cell: %@", NSStringFromClass(cell.class));
                cell->deleteView();
				if (cell->_prefetchLevel == 1) {
					cell->setPrefetchLevel(-1);
				}
			}
		}
	}
}

void BNCellsModule::removeAllViews() {
    for (BNCell* cell : _cells) {
		if (cell->_view) {
			//NSLog(@"Deleting cell: %@", NSStringFromClass(cell.class));
			cell->deleteView();
			if (cell->_prefetchLevel == 1) {
				cell->setPrefetchLevel(-1);
			}
		}
	}
    BNModule::removeAllViews();
}
*/
void BNCellsModule::addItemsToArray(vector<BNBaseModel*>& array) {
	if (_json.boolVal("openAllItemsOnTap")) { // wtf is this? dont recall it at all
        auto items = _contentObject->findChildren({BNModelTypeItem}, {}, {});
		array.insert(array.end(), items.begin(), items.end());
		return;
	}
    for (BNCell* cell : _cells) {
        if (!cell->_isDummy) {
            if (cell->isCellItem()) {
                BNCellItem* cellItem = (BNCellItem*)cell;
                array.push_back(cellItem->_item);
            }
        }
	}
}

void BNCellsModule::onSuperviewDidAppear(bool viewControllerIsMovingToParent) {
    for (BNCell* cell : _cells) {
		cell->onSuperviewDidAppear(viewControllerIsMovingToParent);
	}
}

void BNCellsModule::onSuperviewDidDisappear(bool viewControllerIsMovingFromParent) {
    for (BNCell* cell : _cells) {
		cell->onSuperviewDidDisappear(viewControllerIsMovingFromParent);
	}
}


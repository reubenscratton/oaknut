//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"
/*#import "BNCollections.h"
#import "BNRelationship.h"
#import "BNSearchHelper.h"
#import "BNItem.h"
*/

BNCollection::BNCollection(const string& modelId) : BNContent(BNModelTypeCollection, modelId) {
}

BNCollection::BNCollection(const variant& json) : BNContent(json) {
    _site = json.stringVal("site");
    if (canBeTopicLink()) {
        //TODO: BNSearchHelper::updateWithStub(getStub(), NULL);
    }
    // TODO: BNCollections::checkForUpdatedFollowedTopics(getStub());
    ensureChildItemsAreRelatedToSelf();
}


/**
 Helper called by initialisers to ensure that this collection's child items all have a relationship linking
 them together. This relationship is not always present in the trevor feed.
 */
void BNCollection::ensureChildItemsAreRelatedToSelf() {
    for (auto& childRelationship : _childRelationships) {
		BNBaseModel* childObject = childRelationship->_childObject;
		
		// Ensure that there's a relationship between collection and item.
		if (childObject->isItem()) {
			BNRelationship* missingRel = nullptr;
            for (BNRelationship* rel : childObject->_childRelationships) {
				if ((rel->_childObject->_modelId == _modelId) && (rel->_parentObject->_modelId == childObject->_modelId)) {
					missingRel = rel;
					break;
				}
			}
			if (!missingRel) {
				BNItem* item = (BNItem*)childObject;
				BNRelationship* missingRel = new BNRelationship();
				missingRel->_parentObject = item;
				missingRel->_childObject = this;
				missingRel->_primaryType = BNModelTypeCollection;
				missingRel->_secondaryType = BNModelTypeItem;
                item->_childRelationships.push_back(missingRel);
			}
		}
	}
}

bool BNCollection::canBeTopicLink() {
	if (_name.length() > 0 &&
		!(_modelId == BNModelIdTopStories) &&
		!(_modelId.hasPrefix(BNModelIdMyNews)) &&
		!(_modelId == BNModelIdMostPopular) &&
		!(_modelId == BNModelIdMostRead) &&
		!(_modelId == BNModelIdMostWatched) &&
		!(_modelId == BNModelIdHandrail)) {
		return true;
	}
	return false;
}



bool BNCollection::isFollowable() {
	return !((_modelId == BNModelIdTopStories)
		  || (_modelId == BNModelIdMostRead)
		  || (_modelId == BNModelIdMostWatched)
		  || (_modelId.hasPrefix(BNModelIdMyNews))
		  || (_modelId == BNModelIdMostPopular));
}

//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"
#include "../ui/articles/elementtext.h"
#include "../ui/articles/elementmedia.h"
#include "../ui/articles/articlebodyxmlparser.h"




BNItem::BNItem(const string& type, const string& modelId) : BNContent(type, modelId) {
}

BNItem::BNItem(const variant& json) : BNContent(json) {
    _shortName = json.stringVal("shortName");
    auto bodyData = json.stringVal("body");
    if (bodyData.length()) {
        bodyData.hadPrefix("<?xml version='1.0'?>"); // strip this cos it buggers up parsing.
        BNArticleBodyXmlParser *parser = new BNArticleBodyXmlParser(bodyData, this);
        //parser->parse();
        configureAfterParsing();
        delete parser;
    }
}



BNItem::BNItem(const string& modelId) : BNContent(BNModelTypeItem, modelId) {
}


vector<BNRelationship*> BNItem::findRelationships(const vector<string>& primaryTypes, const vector<string>& secondaryTypes, const vector<string>& formats) {

    
    auto results = BNContent::findRelationships(primaryTypes, secondaryTypes, formats);
	
	
	// NEWSAPPS-4708: If fetching "Related Topics", apply this god-awful hack to move the
    // home collection to the top of the list.
	if (primaryTypes.size()==1 && secondaryTypes.size()==2 && results.size()>0) {
		if (primaryTypes[0] == BNModelTypeCollection) {
			if (secondaryTypes[1] == "bbc.mobile.news.topic") {
                BNRelationship* lastRel = *results.rbegin();
				if (lastRel->_secondaryType == BNRelationshipTypeHomeCollection) {
                    results.pop_back();
					results.insert(results.begin(), lastRel);
				}
				
				// Remove duped topic names (generally place names, e.g. London)
                vector<BNRelationship*> deDuped;
                set<string> seenNames;
                for (auto relationship : results ){
					BNContent* content = (BNContent*)(relationship->_childObject);
					if (seenNames.find(content->_name)==seenNames.end()) {
						seenNames.insert(content->_name);
						deDuped.push_back(relationship);
						continue;
					}
					// Already got one with this name. If the new one is CPS, find the old one, and if it's LDP,
					// replace it with the CPS one.
					if (content->isCPSTopic()) {
                        int i=0;
                        for (auto otherRelationship : deDuped) {
							BNContent *otherContent = (BNContent*)(otherRelationship->_childObject);
							if (otherContent->_name == content->_name) {
								if (!otherContent->isCPSTopic()) {
									deDuped[i] = relationship;
								}
							}
                            i++;
						}
					}
				}
				results = deDuped;
			}
		}
	}
	
	return results;
}




BNImage* BNItem::getIndexImage() {
    if (!_indexImage) {
		_indexImage = (BNImage*)findChildObject(BNModelTypeImage, BNRelationshipTypePlacementIndex);
    }
    return _indexImage;
}

BNImage* BNItem::getPrimaryImage() {
	if (!_primaryImage) {
        auto objects = findChildren({BNModelTypeImage,BNModelTypeVideo,BNModelTypeAudio}, {BNRelationshipTypePlacementPrimary}, {});
		if (objects.size() > 0) {
			if (objects[0]->isImage()) {
				_primaryImage = (BNImage*)objects[0];
			} else if (objects[0]->isAV()) {
				BNAV* av = (BNAV*)objects[0];
				_primaryImage = av->posterImage();
			}
		}
	}
	return _primaryImage;
}


BNCollection* BNItem::getHomedCollection() {
    if (!_homedCollection) {
        _homedCollection = (BNCollection*)findChildObject(BNModelTypeCollection, BNRelationshipTypeHomeCollection);
	}
    return _homedCollection;
}

static bool displayOrderSortFunc(BNRelationship* rel1, BNRelationship* rel2) {
    return rel1->_displayOrder < rel2->_displayOrder;
}
vector<BNRelationship*> BNItem::findOrderedRelationships(const vector<string>& primaryTypes, const vector<string>& secondaryTypes) {
    auto relationships = findRelationships(primaryTypes, secondaryTypes, {});
    std::sort(relationships.begin(), relationships.end(), displayOrderSortFunc);
    return relationships;
}

vector<BNBaseModel*> BNItem::findOrderedChildren(const vector<string>& primaryTypes, const vector<string>& secondaryTypes) {
	auto relationships = findOrderedRelationships(primaryTypes, secondaryTypes);
	vector<BNBaseModel*> children;
    for (auto relationship : relationships) {
		children.push_back(relationship->_childObject);
	}
	return children;
}

vector<BNBaseModel*> BNItem::findImages(const vector<string>& secondaryTypes) {
    auto images = findOrderedChildren({BNModelTypeImage}, secondaryTypes);
    
    // Filter out horizontal rule images
    for (int i=0 ; i<images.size() ; i++) {
        BNImage* image = (BNImage*)images[i];
        if (image->isProbablyAHorizontalRule()) {
            images.erase(images.begin()+i);
            i--;
        }
    }
    return images;
}

vector<BNBaseModel*> BNItem::itemPictureGalleryImages() {
    return findImages({BNRelationshipTypePlacementPrimary, BNRelationshipTypePlacementBody});
}

vector<BNBaseModel*> BNItem::photoGalleryImages() {
    return findImages({BNRelationshipTypePlacementPhotoGallery, BNRelationshipTypePlacementIndexSlideshow});
}

vector<BNBaseModel*> BNItem::primaryAVs() {
    return findOrderedChildren({BNModelTypeVideo,BNModelTypeAudio}, {BNRelationshipTypePlacementPrimary});
}


static BNRelationship* findRelationshipByModelId(const vector<BNRelationship*>& relationships, const string& modelId) {
    for (auto rel : relationships) {
		if (rel->_childObject->_modelId == modelId) {
			return rel;
		}
	}
	return nullptr;
}

BNRelationship* BNItem::imageForMediaId(const string& mediaId) {
    auto bodyImages = findOrderedRelationships({BNModelTypeImage}, {});
	return findRelationshipByModelId(bodyImages, mediaId);
}
BNRelationship* BNItem::videoForMediaId(const string& mediaId) {
    auto bodyVideos = findOrderedRelationships({BNModelTypeVideo}, {});
    return findRelationshipByModelId(bodyVideos, mediaId);
}
BNRelationship* BNItem::audioForMediaId(const string& mediaId) {
    auto bodyAudios = findOrderedRelationships({BNModelTypeAudio}, {});
    return findRelationshipByModelId(bodyAudios, mediaId);
}



void BNItem::configureAfterParsing() {
	
	if (_elements.size()==0) {
		return;
	}
	
	// Remove any empty text elements (cos they cause crashes)
	int leadingImageCount = 0;
	bool seenFirstTextElement = false;
	for (uint32_t i=0 ; i<_elements.size() ; i++) {
		BNElement* element = (BNElement*)_elements[i];
		if (element->isElementText()) {
			BNElementText* elementText = (BNElementText*)element;
			if (elementText->_text.length() == 0) {
                _elements.erase(_elements.begin() + i);
				i--;
			}
			seenFirstTextElement = true;
		} else {
			if (!seenFirstTextElement) {
				leadingImageCount++;
			}
		}
	}
	
	// If article leads with multiple images (the prime example being In The Papers) demote
    // the primary image to be an ordinary body image
	if (leadingImageCount > 1) {
        auto rels = findRelationships({BNModelTypeImage}, {BNRelationshipTypePlacementPrimary}, {});
		if (rels.size() > 0) {
			BNRelationship* primaryImageRel = rels[0];
			primaryImageRel->_secondaryType = BNRelationshipTypePlacementBody;
		}
	}

	
}


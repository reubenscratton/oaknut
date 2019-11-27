//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"
#include "../ui/articles/elementtext.h"
#include "../ui/articles/elementmedia.h"



static BNRelationship* findRelationshipByModelId(const vector<BNRelationship*>& relationships, const string& modelId) {
    for (auto rel : relationships) {
        if (rel->_childObject->_modelId == modelId) {
            return rel;
        }
    }
    return nullptr;
}



BNItem::BNItem(const string& type, const string& modelId) : BNContent(type, modelId) {
}

BNItem::BNItem(const variant& json) : BNContent(json) {
    _shortName = json.stringVal("shortName");
    auto bodyData = json.stringVal("body");
    if (!bodyData.length()) {
        return;
    }
    
    bodyData.hadPrefix("<?xml version='1.0'?>"); // TODO: support these things in the xml parser

    
    vector<string> textElemNames = {"paragraph"_S, "heading"_S, "subheading"_S, "crosshead"_S, "list"_S };
        
    XmlParser xml(bodyData);
    xml.nextTag();
    bool listIsOrdered = false;
    int listOrdinal = 1;
    attributed_string currentPara;
    struct {
        uint32_t insert_point;
        string caption;
        string url;
        string platform;
    } link_props;
    int unknownTagDepth = 0;
    while (!xml.eof()) {
        string tag = xml.currentTag();
        bool isCloseTag = tag.hadPrefix("/");
        
        // Body tag
        if (tag == "body") {
        }
        
        // Paragraphs
        else if (tag.isOneOf(textElemNames)) {
            if (isCloseTag) {
                _elements.push_back(Element(currentPara));
                currentPara.clear();
            } else {
                if (tag == "list") {
                    listIsOrdered = xml.attributeValue("type") == "ordered";
                    listOrdinal = 1;
                }
                string style_name = tag;
                if (tag == "paragraph") {
                    style_name = xml.attributeValue("role");
                }
                currentPara.applyStyle(app->getStyle("article.text."+style_name));
            }
        }
        
        // Images and videos
        else if (tag == "image" || tag == "video" || tag == "audio") {
            string id = xml.attributeValue("id");
            const string modelType = "bbc.mobile.news." + tag;
            auto rels = findOrderedRelationships({modelType}, {});
            auto rel = findRelationshipByModelId(rels, id);
            if (rel) {
                if (tag == "image") {
                    _elements.push_back(Element((BNImage*)rel->_childObject));
                } else {
                    _elements.push_back(Element((BNAV*)rel->_childObject));
                }

            }
        }
        
        // List items
        else if (tag == "listItem") {
            if (isCloseTag) {
                currentPara.append("\n");
            } else {
                if (listIsOrdered) {
                    string itemPrefix = string::format("%lu.  ", (unsigned long)listOrdinal++);
                    currentPara.append(itemPrefix);
                } else {
                    auto traitsStart = currentPara.length();
                    currentPara.append("\u25A0   ");
                    currentPara.setAttribute(attributed_string::forecolor(0xFFCCCCCC), traitsStart, traitsStart+1);
                    /*BNTextTraitFontScale* shrink = [[BNTextTraitFontScale alloc] initWithStart:textElement.text.length];
                    shrink.scale = 0.5f;
                    shrink.end = greyColor.end;
                    [textElement.traits addObject:shrink];
                    BNTextTraitBaselineOffset* offset = [[BNTextTraitBaselineOffset alloc] initWithStartAndEnd:shrink.start end:shrink.end];
                    offset.distance = 0.3f;
                    [textElement.traits addObject:offset];*/

                }
            }
        }
        
        // Links, eg. "<link><caption>What is tactical voting?</caption>
        //             <url platform="newsapps" href="/cps/news/uk-politics-50249649"/></link>"
        else if (tag == "link") {
            if (isCloseTag) {
            }
        }
        else if (tag == "caption") {
            if (!isCloseTag) {
                link_props.insert_point = currentPara.lengthInBytes();
            }
        }
        else if (tag == "url") {
            if (!isCloseTag) {
                link_props.platform = xml.attributeValue("platform");
                link_props.url = xml.attributeValue("href");
            }
        }
        
        // Unknown!
        else {
            app->warn("Unknown tag: %s", tag.c_str());
            unknownTagDepth += isCloseTag? (-1) : 1;
        }

        // Consume text leading up to next tag
        string text = xml.nextTag();
        if (!unknownTagDepth) {
            text.trim();
            currentPara.append(text);
        }
    }
    

    
    configureAfterParsing();
    
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
	
	// Count leading images
	int leadingImageCount = 0;
	for (uint32_t i=0 ; i<_elements.size() ; i++) {
		Element& element = _elements[i];
		if (element.text.lengthInBytes()) {
            break;
		} else {
            leadingImageCount++;
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


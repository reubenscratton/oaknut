#include "_module.h"
#include "../policy/environment.h"

BNRelationship::BNRelationship() {
}
BNRelationship::BNRelationship(const variant& json, BNBaseModel* parentObject) {
    _primaryType = json.stringVal("primaryType");
    _secondaryType = json.stringVal("secondaryType");
    _displayOrder = json.intVal("displayOrder");
    auto& content = json.get("content");
    if (!content.isEmpty()) {
        _childObject = BNBaseModel::createModelObjectFromJson(content);
    }
    _parentObject = parentObject;
}


/*- (BOOL)isEqual:(id)object {
    BNRelationship* rel = (BNRelationship*)object;
    if  (![rel.childObject.modelId isEqualToString:self.childObject.modelId]) {
        return NO;
    }
    if  (![rel.primaryType isEqualToString:self.primaryType]) {
        return NO;
    }
    if  (![rel.secondaryType isEqualToString:self.secondaryType]) {
        return NO;
    }
    if ((rel.parentObject && !self.parentObject) || (!rel.parentObject && self.parentObject)) {
        return NO;
    }
    if (rel.parentObject && self.parentObject) {
        if  (![rel.parentObject.modelId isEqualToString:self.parentObject.modelId]) {
            return NO;
        }
    }
    return YES;
}*/


static map<string, std::function<BNBaseModel*(const variant&)>> s_factory;


BNBaseModel* BNBaseModel::createModelObjectFromJson(const variant& json) {
    if (!s_factory.size()) {
        s_factory[BNModelTypeItem] = [=](const variant& json) -> BNBaseModel* { return new BNItem(json); };
        s_factory[BNModelTypeCollection] = [=](const variant& json) -> BNBaseModel* { return new BNCollection(json); };
        s_factory[BNModelTypeAudio] = [=](const variant& json) -> BNBaseModel* { return new BNAudio(json); };
        s_factory[BNModelTypeVideo] = [=](const variant& json) -> BNBaseModel* { return new BNVideo(json); };
        s_factory[BNModelTypeImage] = [=](const variant& json) -> BNBaseModel* { return new BNImage(json); };
        s_factory[BNModelTypeLiveEvent] = [=](const variant& json) -> BNBaseModel* { return new BNLiveEvent(json); };
        s_factory[BNModelTypeLiveEventExt] = [=](const variant& json) -> BNBaseModel* { return new BNLiveEvent(json); };
        s_factory[BNModelTypeCommentary] = [=](const variant& json) -> BNBaseModel* { return new BNCommentaryItem(json); };
        s_factory[BNModelTypePerson] = [=](const variant& json) -> BNBaseModel* { return new BNPerson(json); };
        s_factory[BNModelTypeByline] = [=](const variant& json) -> BNBaseModel* { return new BNByline(json); };
        s_factory[BNModelTypeLink] = [=](const variant& json) -> BNBaseModel* { return new BNItem(json); };
    }
    
    string type = json.stringVal("type");
    if (type==BNModelTypeItem && json.stringVal("format")==".format.commentary") {
        type = BNModelTypeCommentary;
    }
    auto factoryFunc = s_factory.find(type);
    if (factoryFunc != s_factory.end()) {
        return factoryFunc->second(json);
    }
    log_warn("Unknown content type '%s'", type.c_str());
	return new BNBaseModel(json);
}

BNBaseModel::BNBaseModel(const string& type, const string& modelId) {
    _modelType = type;
    _modelId = modelId;
}


string BNBaseModel::url() {
    return BNEnvironment::URLforResourceSpec(_modelId);
}

BNBaseModel::BNBaseModel(const variant& json) {
    _modelId = json.stringVal("id");
    _modelType = json.stringVal("type");
    _lastUpdated = json.floatVal("lastUpdated") / 1000;

    auto relations = json.arrayRef("relations");
    for (auto relationDict : relations) {
        BNRelationship* relationship = new BNRelationship(relationDict, this);
        if (relationship->_childObject->_modelId.length() > 0) { // filter out objects with nil model ID.
            _childRelationships.push_back(relationship);
        }
    }
}

uint32_t BNBaseModel::getRamCost() const {
    uint32_t cost = sizeof(BNBaseModel);
    for (auto rel : _childRelationships) {
        cost += rel->_childObject->getRamCost();
    }
    return cost;
}

BNBaseModel* BNBaseModel::findChildObject(const string& primaryType, const string& secondaryType) {
    for (auto rel : _childRelationships) {
        if (rel->_primaryType == primaryType && rel->_secondaryType == secondaryType) {
            return rel->_childObject;
        }
    }
    return nullptr;
}

static bool contains(const vector<string>& vec, const string& val) {
    for (auto& it : vec) {
        
        // TODO: This is a terrible hack... the original iOS code uses rangeOfString with the
        // NSRegularExpressionSearch option. Oaknut desperately needs "proper" regex support...
        if (it.hasSuffix("+")) {
            string prefix = it;
            prefix.eraseLast();
            if (val == prefix) {
                return true;
            }
        } else {
            if (it == val) {
                return true;
            }
        }
    }
    return false;
}
vector<BNRelationship*> BNBaseModel::findRelationships(const vector<string>& primaryTypes, const vector<string>& secondaryTypes, const vector<string>& formats) {
    vector<BNRelationship*> matches;
    for (auto rel : _childRelationships) {
        if (primaryTypes.size() && !contains(primaryTypes, rel->_primaryType)) {
            continue;
        }
        if (secondaryTypes.size() && !contains(secondaryTypes, rel->_secondaryType)) {
            continue;
        }
		if (formats.size() && rel->_childObject->isContent()) {
            auto child = rel->_childObject.as<BNContent>();
            if (!contains(formats, child->_format)) {
				continue;
			}
		}
		matches.push_back(rel);
	}
	return matches;
}

vector<BNBaseModel*> BNBaseModel::findChildren(const vector<string>& primaryTypes, const vector<string>& secondaryTypes, const vector<string>& formats) {
	auto relationships = findRelationships(primaryTypes, secondaryTypes, formats);
    vector<BNBaseModel*> children;
    for (auto rel : relationships) {
		children.push_back(rel->_childObject);
	}
	return children;
}




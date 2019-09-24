//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once

#include <oaknut.h>
#include "constants.h"

class BNRelationship : public Object {
public:
    int32_t _displayOrder;
    string _primaryType;
    string _secondaryType;
    class BNBaseModel* _childObject;
    BNBaseModel* _parentObject;

    BNRelationship();
    BNRelationship(const variant& json, BNBaseModel* parentObject);
};



class BNBaseModel : public Object {
public:
    static BNBaseModel* createModelObjectFromJson(const variant& json);

    TIMESTAMP _lastUpdated;
    string _modelId;
    string _modelType;
    vector<BNRelationship*> _childRelationships;
    vector<BNRelationship*> _parentRelationships;

    string url();

    // Runtime type
    virtual bool isContent() { return false; }
    virtual bool isItem() { return false; }
    virtual bool isCollection() { return false; }
    virtual bool isImage() { return false; }
    virtual bool isAV() { return false; }

    BNBaseModel* findChildObject(const string& primaryType, const string& secondaryType);
protected:
    BNBaseModel(const string& type, const string& modelId);
    BNBaseModel(const string& modelId);
    BNBaseModel(const variant& json);

    virtual vector<BNRelationship*> findRelationships(const vector<string>& primaryTypes, const vector<string>& secondaryTypes, const vector<string>& formats);
    vector<BNBaseModel*> findChildren(const vector<string>& primaryTypes, const vector<string>& secondaryTypes, const vector<string>& formats);

    friend class BNCellsModule;
};

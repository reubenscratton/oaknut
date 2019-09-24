#pragma once
#include "content.h"
#include "../articles/element.h"

class BNCollection;
class BNImage;
class BNAV;

class BNItem : public BNContent {
public:
    
    BNItem(const variant& json);
    BNItem(const string& type, const string& modelId);
    BNItem(const string& modelId);
    
    vector<BNElement*> _elements;
    string _shortName;

    BNImage* getIndexImage();
    BNImage* getPrimaryImage();
    BNCollection* getHomedCollection();
    vector<BNBaseModel*> itemPictureGalleryImages();
    vector<BNBaseModel*> photoGalleryImages();
    vector<BNBaseModel*> primaryAVs();


    BNRelationship* imageForMediaId(const string& mediaId);
    BNRelationship* videoForMediaId(const string& mediaId);
    BNRelationship* audioForMediaId(const string& mediaId);

    void configureAfterParsing();

    bool isItem() override { return true; }
    virtual bool isLiveEvent() { return false; }

    vector<BNRelationship*> findRelationships(const vector<string>& primaryTypes, const vector<string>& secondaryTypes, const vector<string>& formats) override;
protected:
    sp<BNImage> _indexImage;
    sp<BNImage> _primaryImage;
    sp<BNCollection> _homedCollection;

    vector<BNRelationship*> findOrderedRelationships(const vector<string>& primaryTypes, const vector<string>& secondaryTypes);
    vector<BNBaseModel*> findOrderedChildren(const vector<string>& primaryTypes, const vector<string>& secondaryTypes);
    vector<BNBaseModel*> findImages(const vector<string>& secondaryTypes);
    
};

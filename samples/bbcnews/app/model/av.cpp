//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "_module.h"


BNAV::BNAV(const variant& json) : BNBaseModel(json) {
    _caption = json.stringVal("caption");
    _externalId = json.stringVal("externalId");
    _isEmbeddable = json.intVal("isEmbeddable")!=0;
    _isLive = json.intVal("isLive")!=0;
    _isAvailable = json.intVal("isAvailable")!=0;
    _duration = json.intVal("duration");
    _guidance = json.stringVal("guidance");
}


BNImage* BNAV::posterImage() {
    if (!_posterImage) {
        _posterImage = (BNImage*)findChildObject(BNModelTypeImage, BNRelationshipTypePlacementPosterImage);
    }
    return _posterImage;
}



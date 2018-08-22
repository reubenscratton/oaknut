//
//  Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//

#include "snapshot.h"


Snapshot::Snapshot() {
}

// ISerializeToVariant
void Snapshot::fromVariant(const Variant& v) {
    _diskInfo = v.getObject<DiskInfo>("diskInfo");
    _data = v.byteBufferVal("data");
    _timestamp = v.doubleVal("timestamp");
    _thumbnail = v.getObject<Bitmap>("thumbnail");
    _label = v.stringVal("label");
    _controllerId = v.stringVal("controllerId");

}
void Snapshot::toVariant(Variant& v) {
    v["diskInfo"] = _diskInfo._obj;
    v["data"] = _data._obj;
    v["timestamp"] = _timestamp;
    v["thumbnail"] = _thumbnail._obj;
    v["label"] = _label;
    v["controllerId"] = _controllerId;
}


void Snapshot::updateWithData(ByteBuffer* data, Bitmap* thumbnail, string controllerId) {
	_timestamp = app.currentMillis();
	_thumbnail = thumbnail;
	_controllerId = controllerId;
    _data = data;
	
	/*
	// Update the index file
	[s_snapshots removeObject:self];
	[s_snapshots insertObject:self atIndex:0];
	[Snapshot saveIndex];*/
}



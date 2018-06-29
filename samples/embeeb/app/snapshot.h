//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#include "app.h"
#include "diskinfo.h"


class Snapshot : public Object, public ISerializeToVariantMap {
public:
    ObjPtr<DiskInfo> _diskInfo;
    string _label;
    uint64_t _timestamp;
    ObjPtr<Bitmap> _thumbnail;
    ObjPtr<ByteBuffer> _data;
    string _controllerId;

    Snapshot();
    void updateWithData(ByteBuffer* data, Bitmap* thumbnail, string controllerId);

    // ISerializeToVariantMap
    Snapshot(const VariantMap& v);
    virtual void writeSelfToVariantMap(VariantMap& v);
};


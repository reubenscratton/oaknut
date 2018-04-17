//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 High-level interface for serializable types
 */
class ISerializable {
public:
    //virtual void readSelf(const class VariantMap* map) =0;
    virtual void writeSelf(class VariantMap* map) =0;
};


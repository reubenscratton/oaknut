//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 Base interface for all serializable things, i.e. that can be read from and written to a stream
 */
class ISerializableBase {
public:
    virtual bool readSelfFromStream(Stream* stream) =0;
    virtual bool writeSelfToStream(Stream* stream) const =0;
};


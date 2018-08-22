//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 The traditional free filesystem expected by many apps is not fully supported
 on Web and there's not a lot of reason to polyfill it. A NoSQL database
 such as IndexedDB a much better fit, AFAICS, for most app storage requirements.
 Presumably this reasoning lies behind W3C's deprecation of FileSystem.
 
 Using IndexedDB effectively is tricky for Oaknut. Any C++ object is merely a
 collection of one or more byte ranges within the global heap buffer, there is no corresponding
 Javascript object and so the serialization that underpins IndexedDB cannot be
 directly used. So what we do instead (on web) is construct ad-hoc Javascript objects
 that encapsulate the Variants at the point they are read and written to IndexedDB.
 For this to work the Variant type must be convertible to and from
 a native Javascript type.

 And of course for non-web platforms we need a tiny file-based NoSQL implementation :-)
 */


/**
 Persistant storage of same-structure Variants. Expected use is to have one LocalStore
 per persisted type, similar to IndexedDB. For simplicity's sake a primary key is mandatory.
 */
class LocalStore : public Object {
public:
    
    static LocalStore* create(const string& name, const string& primaryKeyName);
    
    // Open & close methods.
    virtual void open(std::function<void()> callback)=0;
    virtual void close()=0;
    virtual void flush()=0;
    
    // Querying
    virtual void getCount(std::function<void(int)> success)=0;
    virtual void getAll(std::function<void(Variant*)> success)=0;
    virtual void getOne(const Variant& primaryKeyVal, std::function<void(Variant*)> success)=0;

    // Update
    virtual void remove(const Variant& primaryKeyVal, std::function<void(void)> callback)=0;
    virtual void put(ISerializeToVariant* object, std::function<void(void)> callback)=0; // Insert a new record or replace an existing one
    
protected:
    LocalStore(const string& name, const string& primaryKeyName);
    
    string _name;
    string _primaryKeyName;
    
};



//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//



/**
 * @ingroup data_group
 * @class LocalStore
 * @brief Extremely simple data storage concept.

 As always, supporting the Web means making some non-obvious choices.
 
 IndexedDB is currently the standard web storage API, so Oaknut takes the
 basic idea and applies it to all platforms. The web implementation of
 LocalStore *is* IndexedDB, the non-web implementations are a tiny and very limited
 simulacrum that will be beefed up into something performant at some future date.
 
 Expected use is to have one LocalStore per persistable type, i.e. a class
 that implements `ISerializeToVariant`.
 
 NB: For simplicity's sake a primary key is mandatory, the key name must correspond
 to a variant field.

 */


/**
 */
class LocalStore : public Object {
public:
    
    static LocalStore* create(const string& name, const string& primaryKeyName);
    
    // Open & close methods.
    virtual void open(std::function<void()> callback)=0;
    virtual void close()=0;
    
    // Querying
    virtual void getCount(std::function<void(int)> success)=0;
    virtual void getAll(std::function<void(variant*)> success)=0;
    virtual void getOne(const variant& primaryKeyVal, std::function<void(variant*)> success)=0;

    // Update
    virtual void remove(const variant& primaryKeyVal, std::function<void(void)> callback)=0;
    virtual void put(ISerializeToVariant* object, std::function<void(void)> callback)=0; // Insert a new record or replace an existing one
    
protected:
    LocalStore(const string& name, const string& primaryKeyName);
    
    string _name;
    string _primaryKeyName;
    
};



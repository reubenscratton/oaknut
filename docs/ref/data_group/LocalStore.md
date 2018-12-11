---
layout: default
---

# LocalStore

```
class LocalStore
    : public Object
```


Extremely simple data storage concept.     

As always, supporting the Web means making some non-obvious choices.IndexedDB is currently the standard web storage API, so Oaknut takes the basic idea and applies it to all platforms. The web implementation of LocalStore *is* IndexedDB, the non-web implementations are a tiny and very limited simulacrum that will be beefed up into something performant at some future date.Expected use is to have one LocalStore per persistable type, i.e. a class that implements `ISerializeToVariant`.NB: For simplicity's sake a primary key is mandatory, the key name must correspond to a variant field.     
[`LocalStore`](/oaknut/ref/data_group/LocalStore)` * create(const `[`string`](/oaknut/ref/base_group/string)` & name, const `[`string`](/oaknut/ref/base_group/string)` & primaryKeyName)`<br>



`void open(std::function< void()> callback)`<br>

`void close()`<br>

`void flush()`<br>

`void getCount(std::function< void(int)> success)`<br>

`void getAll(std::function< void(variant *)> success)`<br>

`void getOne(const variant & primaryKeyVal, std::function< void(variant *)> success)`<br>

`void remove(const variant & primaryKeyVal, std::function< void(void)> callback)`<br>

`void put(ISerializeToVariant * object, std::function< void(void)> callback)`<br>



` LocalStore(const `[`string`](/oaknut/ref/base_group/string)` & name, const `[`string`](/oaknut/ref/base_group/string)` & primaryKeyName)`<br>



## Methods

| | |
|-|-|
| *LocalStore* | ` LocalStore(const `[`string`](/oaknut/ref/base_group/string)` & name, const `[`string`](/oaknut/ref/base_group/string)` & primaryKeyName)` |  |
| *close* | `void close()` |  |
| *create* | [`LocalStore`](/oaknut/ref/data_group/LocalStore)` * create(const `[`string`](/oaknut/ref/base_group/string)` & name, const `[`string`](/oaknut/ref/base_group/string)` & primaryKeyName)` |  |
| *flush* | `void flush()` |  |
| *getAll* | `void getAll(std::function< void(variant *)> success)` |  |
| *getCount* | `void getCount(std::function< void(int)> success)` |  |
| *getOne* | `void getOne(const variant & primaryKeyVal, std::function< void(variant *)> success)` |  |
| *open* | `void open(std::function< void()> callback)` |  |
| *put* | `void put(ISerializeToVariant * object, std::function< void(void)> callback)` |  |
| *remove* | `void remove(const variant & primaryKeyVal, std::function< void(void)> callback)` |  |

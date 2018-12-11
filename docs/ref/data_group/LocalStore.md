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
| | |
|-|-|
|[`LocalStore`](/ref/data_group/LocalStore)` * create(const `[`string`](/ref/base_group/string)` & name, const `[`string`](/ref/base_group/string)` & primaryKeyName)`||


| | |
|-|-|
|`void open(std::function< void()> callback)`||
|`void close()`||
|`void flush()`||
|`void getCount(std::function< void(int)> success)`||
|`void getAll(std::function< void(variant *)> success)`||
|`void getOne(const variant & primaryKeyVal, std::function< void(variant *)> success)`||
|`void remove(const variant & primaryKeyVal, std::function< void(void)> callback)`||
|`void put(ISerializeToVariant * object, std::function< void(void)> callback)`||


| | |
|-|-|
|` LocalStore(const `[`string`](/ref/base_group/string)` & name, const `[`string`](/ref/base_group/string)` & primaryKeyName)`||


## Methods

| | |
|-|-|
| *create* | [`LocalStore`](/ref/data_group/LocalStore)` * create(const `[`string`](/ref/base_group/string)` & name, const `[`string`](/ref/base_group/string)` & primaryKeyName)` |  |
| *open* | `void open(std::function< void()> callback)` |  |
| *close* | `void close()` |  |
| *flush* | `void flush()` |  |
| *getCount* | `void getCount(std::function< void(int)> success)` |  |
| *getAll* | `void getAll(std::function< void(variant *)> success)` |  |
| *getOne* | `void getOne(const variant & primaryKeyVal, std::function< void(variant *)> success)` |  |
| *remove* | `void remove(const variant & primaryKeyVal, std::function< void(void)> callback)` |  |
| *put* | `void put(ISerializeToVariant * object, std::function< void(void)> callback)` |  |
| *LocalStore* | ` LocalStore(const `[`string`](/ref/base_group/string)` & name, const `[`string`](/ref/base_group/string)` & primaryKeyName)` |  |

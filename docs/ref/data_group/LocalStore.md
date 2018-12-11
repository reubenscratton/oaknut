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
## 



## 

| `<a href="class_local_store.html">LocalStore</a> * create(const <a href="classstring.html">string</a> & name, const <a href="classstring.html">string</a> & primaryKeyName)` |  |


## 

| `void open(std::function< void()> callback)` |  |
| `void close()` |  |
| `void flush()` |  |
| `void getCount(std::function< void(int)> success)` |  |
| `void getAll(std::function< void(<a href="classvariant.html">variant</a> *)> success)` |  |
| `void getOne(const <a href="classvariant.html">variant</a> & primaryKeyVal, std::function< void(<a href="classvariant.html">variant</a> *)> success)` |  |
| `void remove(const <a href="classvariant.html">variant</a> & primaryKeyVal, std::function< void(void)> callback)` |  |
| `void put(<a href="class_i_serialize_to_variant.html">ISerializeToVariant</a> * object, std::function< void(void)> callback)` |  |


## 

| ` LocalStore(const <a href="classstring.html">string</a> & name, const <a href="classstring.html">string</a> & primaryKeyName)` |  |


# Methods

| *create* |  `<a href="class_local_store.html">LocalStore</a> * <a href="todo">create</a>(const <a href="classstring.html">string</a> & name, const <a href="classstring.html">string</a> & primaryKeyName)` |  |
| *open* |  `void <a href="todo">open</a>(std::function< void()> callback)` |  |
| *close* |  `void <a href="todo">close</a>()` |  |
| *flush* |  `void <a href="todo">flush</a>()` |  |
| *getCount* |  `void <a href="todo">getCount</a>(std::function< void(int)> success)` |  |
| *getAll* |  `void <a href="todo">getAll</a>(std::function< void(<a href="classvariant.html">variant</a> *)> success)` |  |
| *getOne* |  `void <a href="todo">getOne</a>(const <a href="classvariant.html">variant</a> & primaryKeyVal, std::function< void(<a href="classvariant.html">variant</a> *)> success)` |  |
| *remove* |  `void <a href="todo">remove</a>(const <a href="classvariant.html">variant</a> & primaryKeyVal, std::function< void(void)> callback)` |  |
| *put* |  `void <a href="todo">put</a>(<a href="class_i_serialize_to_variant.html">ISerializeToVariant</a> * object, std::function< void(void)> callback)` |  |
| *LocalStore* |  ` <a href="todo">LocalStore</a>(const <a href="classstring.html">string</a> & name, const <a href="classstring.html">string</a> & primaryKeyName)` |  |

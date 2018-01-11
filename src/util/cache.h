//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

template <typename K>
class MruCache {
public:
	typedef struct {
		K key;
		Object* value;
		size_t cost;
	} ENTRY;
	typedef typename std::list<ENTRY>::iterator list_iterator_t;

	MruCache(size_t max) : _max(max) {
	}
	
	bool get(const K& key, Object** p) {
		auto it = _map.find(key);
		if (it == _map.end()) {
			*p = NULL;
			return false;
		}
		_list.splice(_list.begin(), _list, it->second);
		*p = it->second->value;
		return true;
	}

	void put(const K& key, Object* value, size_t cost) {
		ENTRY entry;
		entry.key = key;
		entry.value = value;
		entry.cost = cost;
        value->retain();
		_list.push_front(entry);
		_map[key] = _list.begin();
		_used += cost;		
		while (_used > _max) {
			auto last = _list.end();
			last--;
			_used -= last->cost;
            last->value->release();
			_map.erase(last->key);
			_list.pop_back();
		}
	}
	
	
	
private:
	list<ENTRY> _list;
	unordered_map<K, list_iterator_t> _map;
	size_t _max;
	size_t _used;
};


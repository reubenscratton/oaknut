//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

template <typename K, typename V>
class MruCache {
public:
	typedef struct {
		K key;
		V value;
		size_t cost;
	} ENTRY;
	typedef typename std::list<ENTRY>::iterator list_iterator_t;

	void setSize(size_t max) {
        _max = max;
        applyLimit();
	}
	
	bool get(const K& key, V& p) {
		auto it = _map.find(key);
		if (it == _map.end()) {
			return false;
		}
		_list.splice(_list.begin(), _list, it->second);
		p = it->second->value;
		return true;
	}

	void put(const K& key, V& value, size_t cost) {
		ENTRY entry;
		entry.key = key;
		entry.value = value;
		entry.cost = cost;
        value->retain();
		_list.push_front(entry);
		_map[key] = _list.begin();
		_used += cost;
        applyLimit();
    }
    
    void applyLimit() {
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


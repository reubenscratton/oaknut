//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// ItemPool is a dynamically-sized pool of fixed-size items that can be alloc'd singly or in groups.
// Allocations may be moved without warning as the pool grows and defrags itself, clients must access
// allocated item memory via an Alloc* rather than a raw pointer.
//
class ItemPool : public Object {
public:
    uint8_t* _base;
    int _itemSize;
    int _itemCount;
    int _itemsPerPage;
    
    class Alloc {
    public:
        //uint8_t* base;
        ItemPool* pool;
        int offset;
        int count;
        bool used;
        list<Alloc*>::iterator it;
        
        Alloc(ItemPool* pool, int offset, int count) {
            this->pool = pool;
            this->offset = offset;
            this->count = count;
            used = false;
        }
        uint8_t* addr() {
            return pool->_base + pool->_itemSize * offset;
        }

    };
    
    list<Alloc*> _blocks; // this is kept in item index order
    
    ItemPool(int itemSize, int itemsPerPage) {
        _itemSize = itemSize;
        _itemsPerPage = itemsPerPage;
        _itemCount = 0;
    }
    
    
    Alloc* alloc(int n, Alloc* existingAlloc);
    virtual void resize(int newItemCount);
    void free(Alloc* alloc);
};


class QuadBuffer : public ItemPool {
public:
    
    GLshort* _indexes;
    GLuint _indexBufferId;
    GLuint _vertexBufferId;
    GLuint _vao;
    
    QuadBuffer();
    void bind();
    void upload();
    
    virtual void resize(int newItemCount);
    
    
};


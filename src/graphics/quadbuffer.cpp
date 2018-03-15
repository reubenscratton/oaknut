//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

ItemPool::Alloc* ItemPool::alloc(int n, ItemPool::Alloc* existingAlloc) {
    
    // Try to trivially grow the existing alloc
    if (existingAlloc) {
        auto it = existingAlloc->it;
        it++;
        if (it!=_blocks.end()) {
            Alloc* nextBlock = *it;
            if (!nextBlock->used) {
                if (n<=nextBlock->count) {
                    existingAlloc->count += n;
                    nextBlock->offset += n;
                    nextBlock->count -= n;
                    if (!nextBlock->count) {
                        _blocks.erase(nextBlock->it);
                        delete nextBlock;
                    }
                    return existingAlloc;
                }
            }
        }
    }
    
    // Look for the first free block that's big enough
    Alloc* a = NULL;
    for (auto it=_blocks.begin() ; it!=_blocks.end() ; it++) {
        Alloc* freeBlock = *it;
        if (freeBlock->used) continue;
        int xs = freeBlock->count - n;
        if (xs >= 0) {
            if (xs) {
                a = new Alloc(this, freeBlock->offset, n);
                a->used = true;
                a->it = _blocks.insert(it, a);
                freeBlock->offset += n;
                freeBlock->count -= n;
            } else {
                a = freeBlock;
                a->used = true;
            }
            break;
        }
    }
    if (!a) {
        resize(_itemCount + max(n, _itemsPerPage));
        a = alloc(n, NULL);
    }

    assert(a->offset+a->count <= _itemCount);

    if (existingAlloc && existingAlloc!=a) {
        memcpy(a->addr(), existingAlloc->addr(), existingAlloc->count*_itemSize);
        free(existingAlloc);
    }
    //_usedList.push_back(a);
    return a;
}

void ItemPool::resize(int newItemCount) {
    int d = newItemCount - _itemCount;
    uint8_t* newBase = (uint8_t*)malloc(newItemCount*_itemSize);
    for (auto it=_blocks.begin() ; it != _blocks.end() ; it++) {
        ItemPool::Alloc* a = *it;
        if (a->used) {
            memcpy(newBase + a->offset*_itemSize, _base + a->offset*_itemSize, a->count*_itemSize);
        }
    }
    ::free(_base);
    _base = newBase;
    Alloc* a = new Alloc(this, _itemCount, d);
    _blocks.push_back(a);
    a->it = _blocks.end();
    a->it--;
    _itemCount = newItemCount;
}

void ItemPool::free(ItemPool::Alloc* alloc) {
    alloc->used = false; // this is all thats really needed
    // Consolidate with previous block if its also free
    auto it = alloc->it;
    it--;
    if (it!=_blocks.end()) {
        Alloc* pre = *it;
        if (!pre->used) {
            pre->count += alloc->count;
            _blocks.erase(alloc->it);
            delete alloc;
            alloc = pre;
        }
    }
    // Consolidate with next block if its also free
    it = alloc->it;
    it++;
    if (it!=_blocks.end()) {
        Alloc* post = *it;
        if (!post->used) {
            post->offset -= alloc->count;
            post->count += alloc->count;
            _blocks.erase(alloc->it);
            delete alloc;
            alloc = post;
        }
    }
}


QuadBuffer::QuadBuffer() : ItemPool(sizeof(QUAD), 256) {
}

void QuadBuffer::bind() {
    if (_indexBufferId == 0) {
#ifdef HAS_VAO
        check_gl(glGenVertexArrays, 1, &_vao);
        check_gl(glBindVertexArray, _vao);
#endif
        check_gl(glGenBuffers, 1, &_indexBufferId);
        check_gl(glGenBuffers, 1, &_vertexBufferId);
        check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        check_gl(glBindBuffer, GL_ARRAY_BUFFER, _vertexBufferId);
    }
#ifndef HAS_VAO
    check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    check_gl(glBindBuffer, GL_ARRAY_BUFFER, _vertexBufferId);
#else
    check_gl(glBindVertexArray, _vao);
#endif
}
void QuadBuffer::upload() {
    bind();
    if (_itemCount > 0 && _indexBufferId > 0) {
        check_gl(glBufferData, GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * 6 * _itemCount, _indexes, GL_STATIC_DRAW);
        // TODO: On first use buffer is unused so pass NULL instead of _base
        check_gl(glBufferData, GL_ARRAY_BUFFER, sizeof(QUAD) * _itemCount, _base, GL_DYNAMIC_DRAW);
    }
}


void QuadBuffer::resize(int newItemCount) {
    int oldItemCount = _itemCount;
    ItemPool::resize(newItemCount);
    int cbIndexes = sizeof(GLshort) * 6 * newItemCount;
    GLshort* newIndexes = (GLshort*) malloc(cbIndexes);
    if (oldItemCount) {
        memcpy(newIndexes, _indexes, sizeof(GLshort) * 6 * oldItemCount);
        ::free(_indexes);
    }
    
    /*
     
     C------D
     |     /|
     |    / |
     |   /  |
     |  /   |
     | /    |
     A------B
     
     Clockwise triangles are ABC & CBD
     */
    _indexes = newIndexes;
    for (int i=oldItemCount ; i<newItemCount ; i++) {
        _indexes[i*6+0] = i*4+0; // A
        _indexes[i*6+1] = i*4+2; // C
        _indexes[i*6+2] = i*4+3; // D
        _indexes[i*6+3] = i*4+0; // A
        _indexes[i*6+4] = i*4+3; // D
        _indexes[i*6+5] = i*4+1; // B
    }
    
    upload();
    
}




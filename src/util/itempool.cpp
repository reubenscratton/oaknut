//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

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
        int newItemCount = _itemCount + MAX(n, _itemsPerPage);
        if (_resizeHook) {
            _resizeHook(_itemCount, newItemCount);
        }
        resize(newItemCount);
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





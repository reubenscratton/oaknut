//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#import <XCTest/XCTest.h>
#include "oaknut.h"

@interface app_nativeTests : XCTestCase

@end

@implementation app_nativeTests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testItemPool {

    ItemPool* pool = new ItemPool(1, 10);
    assert(pool->_blocks.size() == 0);
    
    // Allocate a single item
    ItemPool::Alloc* a = pool->alloc(1, NULL);
    assert(pool->_blocks.size() == 2);
    assert(a->count == 1);
    
    // Free it
    pool->free(a);
    assert(pool->_blocks.size() == 1);
    
    // Allocate 10 items to completely exhaust the pool
    a = pool->alloc(4, NULL);
    ItemPool::Alloc* b = pool->alloc(1, NULL);
    ItemPool::Alloc* c = pool->alloc(2, NULL);
    ItemPool::Alloc* d = pool->alloc(3, NULL);
    assert(a->count==4);
    assert(b->count==1);
    assert(c->count==2);
    assert(d->count==3);
    assert(pool->_blocks.size()==4);
    
    // Free two allocs that won't consolidate
    pool->free(b);
    assert(pool->_blocks.size()==4);
    pool->free(d);
    assert(pool->_blocks.size()==4);
    
    // Freeing c will consolidate with b and d
    pool->free(c);
    assert(pool->_blocks.size()==2);

    // Freeing a takes us back to empty pool
    pool->free(a);
    assert(pool->_blocks.size()==1);
    
    // Allocate something larger than a page
    a = pool->alloc(25, NULL);
    assert(pool->_blocks.size()==2);
    
    // Realloc
    a = pool->alloc(26, a);
    assert(pool->_blocks.size()==2);
    
}


@end

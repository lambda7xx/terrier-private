#include "test_util/test_harness.h"
#include "storage/index/bplustree.h"
#include "storage/storage_defs.h"

#include <stdlib.h> 
#include <unordered_map>
#include <set>

namespace terrier::storage::index {

struct BPlusTreeTests : public TerrierTest {};

void BasicNodeInitializationInsertReadAndFreeTest(){
  auto bplustree = new BPlusTree<int, TupleSlot>;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p2;

  // Get inner Node
  auto node = BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>::KeyNodePointerPair>::Get(10, BPlusTree<int, TupleSlot>::NodeType::LeafType, 0, 10, p1, p2);
  
  // To check if we can read what we inserted
  std::vector<BPlusTree<int, TupleSlot>::KeyNodePointerPair> values;
  for(unsigned i = 0; i < 10; i++) {
    BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
    p1.first = i;
    values.push_back(p1);
    node->PushBack(p1);
    EXPECT_EQ(node->GetSize(), i+1);
  }

  using ElementType = BPlusTree<int, TupleSlot>::KeyNodePointerPair;

  unsigned i = 0;
  for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i++;
  }

  // To Check if we are inserting at the correct place
  EXPECT_EQ(reinterpret_cast<char *>(node) + 
    sizeof(BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>>), 
    reinterpret_cast<char *>(node->Begin()));

  EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
  EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
  EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
  EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
  EXPECT_NE(&p1, &(node->GetLowKeyPair()));
  EXPECT_NE(&p2, &(node->GetHighKeyPair()));

  // Free the node - should not result in an ASAN
  node->FreeElasticNode();
  delete bplustree;

}

void InsertElementInNodeTest(){
  auto bplustree = new BPlusTree<int, TupleSlot>;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p2;

  // Get inner Node
  auto node = BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>::KeyNodePointerPair>::Get(10, BPlusTree<int, TupleSlot>::NodeType::LeafType, 0, 10, p1, p2);
  
  for(unsigned i = 0; i < 10; i++) {
    BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
    p1.first = i;
    EXPECT_EQ(node->InsertElementIfPossible(p1, node->Begin()), true);
    EXPECT_EQ(node->GetSize(), i+1);
  }

  using ElementType = BPlusTree<int, TupleSlot>::KeyNodePointerPair;

  unsigned i = 9;
  for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i--;
  }

  // To Check if we are inserting at the correct place
  EXPECT_EQ(reinterpret_cast<char *>(node) + 
    sizeof(BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>>), 
    reinterpret_cast<char *>(node->Begin()));

  EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
  EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
  EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
  EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
  EXPECT_NE(&p1, &(node->GetLowKeyPair()));
  EXPECT_NE(&p2, &(node->GetHighKeyPair()));

  // Free the node - should not result in an ASAN
  node->FreeElasticNode();
  delete bplustree;

}

void InsertElementInNodeRandomTest(){
  auto bplustree = new BPlusTree<int, TupleSlot>;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p2;

  // Get inner Node
  auto node = BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>::KeyNodePointerPair>::Get(10, BPlusTree<int, TupleSlot>::NodeType::LeafType, 0, 10, p1, p2);
  
  std::map<int, int> positions;
  for(unsigned i = 0; i < 10; i++) {
    BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
    p1.first = i;
    int k;
    k = rand() % (node->GetSize() + 1);
    while(positions.find(k) != positions.end()) k = (k+1) % (node->GetSize()+1);
    EXPECT_EQ(node->InsertElementIfPossible(p1, node->Begin() + k), true);
    positions[k] = i;
    EXPECT_EQ(node->GetSize(), i+1);
  }

  using ElementType = BPlusTree<int, TupleSlot>::KeyNodePointerPair;

  unsigned i = 0;
  for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
    EXPECT_EQ(element_p->first, positions[i]);
    i++;
  }

  // To Check if we are inserting at the correct place
  EXPECT_EQ(reinterpret_cast<char *>(node) + 
    sizeof(BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>>), 
    reinterpret_cast<char *>(node->Begin()));

  EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
  EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
  EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
  EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
  EXPECT_NE(&p1, &(node->GetLowKeyPair()));
  EXPECT_NE(&p2, &(node->GetHighKeyPair()));

  // Free the node - should not result in an ASAN
  node->FreeElasticNode();
  delete bplustree;

}

void SplitNodeTest(){
  auto bplustree = new BPlusTree<int, TupleSlot>;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p2;

  // Get inner Node
  auto node = BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>::KeyNodePointerPair>::Get(10, BPlusTree<int, TupleSlot>::NodeType::LeafType, 0, 10, p1, p2);
  
  for(unsigned i = 0; i < 10; i++) {
    BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
    p1.first = i;
    EXPECT_EQ(node->InsertElementIfPossible(p1, node->End()), true);
    EXPECT_EQ(node->GetSize(), i+1);
  }

  auto newnode = node->SplitNode();

  using ElementType = BPlusTree<int, TupleSlot>::KeyNodePointerPair;


  unsigned i = 0;
  for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i++;
  }

  EXPECT_EQ(i, 5);

  for (ElementType *element_p = newnode->Begin(); element_p != newnode->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i++;
  }

  EXPECT_EQ(i, 10);

  EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
  EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
  EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
  EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
  EXPECT_NE(&p1, &(node->GetLowKeyPair()));
  EXPECT_NE(&p2, &(node->GetHighKeyPair()));

  EXPECT_EQ(&(newnode->GetLowKeyPair()), newnode->GetElasticLowKeyPair());
  EXPECT_EQ(&(newnode->GetHighKeyPair()), newnode->GetElasticHighKeyPair());
  EXPECT_EQ(newnode->GetLowKeyPair().first, p1.first);
  EXPECT_EQ(newnode->GetHighKeyPair().first, p2.first);
  EXPECT_NE(&p1, &(newnode->GetLowKeyPair()));
  EXPECT_NE(&p2, &(newnode->GetHighKeyPair()));

  // Free the node - should not result in an ASAN
  node->FreeElasticNode();
  newnode->FreeElasticNode();
  delete bplustree;
}

void FindLocationTest(){
  auto bplustree = new BPlusTree<int, TupleSlot>;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p1;
  BPlusTree<int, TupleSlot>::KeyNodePointerPair p2;

  // Get inner Node
  auto node = BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>::KeyNodePointerPair>::Get(10, BPlusTree<int, TupleSlot>::NodeType::LeafType, 0, 10, p1, p2);

  std::set<unsigned> s;
  while(node->GetSize() < node->GetItemCount()) {
    int k = rand();
    while(s.find(k) != s.end()) k++;
    s.insert(k);
    BPlusTree<int, TupleSlot>::KeyNodePointerPair p;
    p.first = k;
    EXPECT_EQ(node->InsertElementIfPossible(p, node->FindLocation(k, bplustree)),true);
  }
  auto iter = node->Begin();
  for(auto & elem: s) {
    EXPECT_EQ(iter->first, elem);
    iter++;
  }

  // To Check if we are inserting at the correct place
  EXPECT_EQ(reinterpret_cast<char *>(node) + 
    sizeof(BPlusTree<int, TupleSlot>::ElasticNode<BPlusTree<int, TupleSlot>>), 
    reinterpret_cast<char *>(node->Begin()));

  EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
  EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
  EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
  EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
  EXPECT_NE(&p1, &(node->GetLowKeyPair()));
  EXPECT_NE(&p2, &(node->GetHighKeyPair()));

  // Free the node - should not result in an ASAN
  node->FreeElasticNode();
  delete bplustree;

}


// NOLINTNEXTLINE
TEST_F(BPlusTreeTests, NodeStructuralTests) {

  BasicNodeInitializationInsertReadAndFreeTest();
  InsertElementInNodeTest();
  InsertElementInNodeRandomTest();
  SplitNodeTest();
  FindLocationTest();
}

void BasicBPlusTreeInsertTestNoSplittingOfRoot() {

  auto bplustree = new BPlusTree<int, TupleSlot>;
  for(unsigned i=0; i<100; i++) {
    BPlusTree<int, TupleSlot>::KeyValuePair p1;
    p1.first = i;
    bplustree->Insert(p1);
  }

  using ElementType = BPlusTree<int, TupleSlot>::KeyValuePair;

  auto node = reinterpret_cast<BPlusTree<int, TupleSlot>::ElasticNode<ElementType> *>(bplustree->GetRoot()); 
  unsigned i = 0;
  for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i++;
  }
  EXPECT_EQ(i, 100);
  node->FreeElasticNode();
  delete bplustree;
}

void BasicBPlusTreeInsertTestSplittingOfRootOnce() {

  auto bplustree = new BPlusTree<int, TupleSlot>;
  for(unsigned i=0; i<129; i++) {
    BPlusTree<int, TupleSlot>::KeyValuePair p1;
    p1.first = i;
    bplustree->Insert(p1);
  }

  using ElementType = BPlusTree<int, TupleSlot>::KeyValuePair;
  using KeyPointerType = BPlusTree<int, TupleSlot>::KeyNodePointerPair;

  auto node = reinterpret_cast<BPlusTree<int, TupleSlot>::ElasticNode<ElementType> *>
    (bplustree->GetRoot()->GetLowKeyPair().second);
  auto noderoot = reinterpret_cast<BPlusTree<int, TupleSlot>::ElasticNode<KeyPointerType> *>
    (bplustree->GetRoot());
  auto node2 =  reinterpret_cast<BPlusTree<int, TupleSlot>::ElasticNode<ElementType> *>
    (noderoot->Begin()->second);
  unsigned i = 0;
  for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i++;
  }
  EXPECT_EQ(i, 64);
  for (ElementType *element_p = node2->Begin(); element_p != node2->End(); element_p++) {
    EXPECT_EQ(element_p->first, i);
    i++;
  }
  EXPECT_EQ(i, 129);

  // Count no of elements in root node - should be 1
  i = 0;
  for (KeyPointerType * element_p = noderoot->Begin(); element_p!=noderoot->End(); element_p ++)
  	i++;

  EXPECT_EQ(i, 1);

  // Only freeing these should free us of any ASAN
  node->FreeElasticNode();
  node2->FreeElasticNode();
  noderoot->FreeElasticNode();
  delete bplustree;
}


// NOLINTNEXTLINE
TEST_F(BPlusTreeTests, InsertTests) {

  BasicBPlusTreeInsertTestNoSplittingOfRoot();
  BasicBPlusTreeInsertTestSplittingOfRootOnce();
}

} // namespace terrier::storage::index
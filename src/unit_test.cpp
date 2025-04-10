#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "BloomFilter.h"
#include "Trie.h"

// ====================== BLOOM FILTER TESTS ======================

// Test constructor and basic operations
TEST(BloomFilterTest, ConstructorAndBasicOperations) {
	// Create a BloomFilter with 3 hash functions
	BloomFilter<1024> filter(3);

	// Test that a newly created filter doesn't contain anything
	EXPECT_FALSE(filter.possiblyContains("test"));

	// Add an item
	filter.add("test");

	// Check if the item might be in the filter
	EXPECT_TRUE(filter.possiblyContains("test"));

	// Check that an item we never added is not in the filter
	EXPECT_FALSE(filter.certainlyContains("not_added"));
}

// Test copy constructor
TEST(BloomFilterTest, CopyConstructor) {
	BloomFilter<1024> filter1(3);
	filter1.add("test1");
	filter1.add("test2");

	// Create a copy
	BloomFilter<1024> filter2(filter1);

	// Both filters should contain the same items
	EXPECT_TRUE(filter2.possiblyContains("test1"));
	EXPECT_TRUE(filter2.possiblyContains("test2"));
	EXPECT_FALSE(filter2.certainlyContains("not_added"));
}

// Test move constructor
TEST(BloomFilterTest, MoveConstructor) {
	BloomFilter<1024> filter1(3);
	filter1.add("test1");
	filter1.add("test2");

	// Move construct filter2 from filter1
	BloomFilter<1024> filter2(std::move(filter1));

	// filter2 should contain the items from filter1
	EXPECT_TRUE(filter2.possiblyContains("test1"));
	EXPECT_TRUE(filter2.possiblyContains("test2"));
}

// Test reset functionality
TEST(BloomFilterTest, Reset) {
	BloomFilter<1024> filter(3);
	filter.add("test1");
	filter.add("test2");

	// Both items should be in the filter
	EXPECT_TRUE(filter.possiblyContains("test1"));
	EXPECT_TRUE(filter.possiblyContains("test2"));

	// Reset the filter
	filter.reset();

	// After reset, items should no longer be in the filter
	EXPECT_FALSE(filter.possiblyContains("test1"));
	EXPECT_FALSE(filter.possiblyContains("test2"));
}

// Test operator& (intersection)
TEST(BloomFilterTest, IntersectionOperator) {
	BloomFilter<1024> filter1(3);
	filter1.add("common");
	filter1.add("only_in_filter1");

	BloomFilter<1024> filter2(3);
	filter2.add("common");
	filter2.add("only_in_filter2");

	// Create intersection
	BloomFilter<1024> result = filter1 & filter2;

	// The intersection should possibly contain common items
	EXPECT_TRUE(result.possiblyContains("common"));
}

// Test operator| (union)
TEST(BloomFilterTest, UnionOperator) {
	BloomFilter<1024> filter1(3);
	filter1.add("common");
	filter1.add("only_in_filter1");

	BloomFilter<1024> filter2(3);
	filter2.add("common");
	filter2.add("only_in_filter2");

	// Create union
	BloomFilter<1024> result = filter1 | filter2;

	// The union should contain all items
	EXPECT_TRUE(result.possiblyContains("common"));
	EXPECT_TRUE(result.possiblyContains("only_in_filter1"));
	EXPECT_TRUE(result.possiblyContains("only_in_filter2"));
}

// Test operator() (functional operator)
TEST(BloomFilterTest, FunctionalOperator) {
	BloomFilter<1024> filter(3);
	filter.add("test");

	// Functional operator should work like possiblyContains
	EXPECT_TRUE(filter("test"));
}


// Test adding items from file
TEST(BloomFilterTest, AddFromFile) {
	// Create a temporary file with test words
	std::ofstream file("temp_words.txt");
	file << "word1, word2, word3";
	file.close();

	BloomFilter<1024> filter(3);
	filter.add("temp_words.txt");

	// Check if words from file are in the filter
	EXPECT_TRUE(filter.possiblyContains("word1"));
	EXPECT_TRUE(filter.possiblyContains("word2"));
	EXPECT_TRUE(filter.possiblyContains("word3"));

	// Clean up
	std::remove("temp_words.txt");
}

// Test certainlyContains method
TEST(BloomFilterTest, CertainlyContains) {
	BloomFilter<1024> filter(3);
	filter.add("definitely_present");

	EXPECT_TRUE(filter.certainlyContains("definitely_present"));
	EXPECT_FALSE(filter.certainlyContains("definitely_not_present"));
}

// ====================== TRIE TESTS ======================

// Test constructor and basic operations
TEST(TrieTest, ConstructorAndBasicOperations) {
	Trie trie;

	// Test empty trie
	EXPECT_FALSE(trie.search("apple"));

	// Insert and search
	trie.insert("apple");
	EXPECT_TRUE(trie.search("apple"));
	EXPECT_FALSE(trie.search("app"));

	// Test startsWith
	EXPECT_TRUE(trie.startsWith("app"));
	EXPECT_FALSE(trie.startsWith("banana"));
}

// Test copy constructor
TEST(TrieTest, CopyConstructor) {
	Trie trie1;
	trie1.insert("apple");
	trie1.insert("banana");

	// Create a copy
	Trie trie2(trie1);

	// Copy should contain the same words
	EXPECT_TRUE(trie2.search("apple"));
	EXPECT_TRUE(trie2.search("banana"));
	EXPECT_FALSE(trie2.search("orange"));
}

// Test move constructor
TEST(TrieTest, MoveConstructor) {
	Trie trie1;
	trie1.insert("apple");
	trie1.insert("banana");

	// Move construct trie2 from trie1
	Trie trie2(std::move(trie1));

	// trie2 should contain the words from trie1
	EXPECT_TRUE(trie2.search("apple"));
	EXPECT_TRUE(trie2.search("banana"));
}

// Test initializer list constructor
TEST(TrieTest, InitializerListConstructor) {
	Trie trie{"apple", "banana", "cherry"};

	// All initialized words should be found
	EXPECT_TRUE(trie.search("apple"));
	EXPECT_TRUE(trie.search("banana"));
	EXPECT_TRUE(trie.search("cherry"));
	EXPECT_FALSE(trie.search("orange"));
}

// Test assignment operators
TEST(TrieTest, AssignmentOperators) {
	Trie trie1{"apple", "banana"};
	Trie trie2;

	// Copy assignment
	trie2 = trie1;
	EXPECT_TRUE(trie2.search("apple"));
	EXPECT_TRUE(trie2.search("banana"));

	// Move assignment
	Trie trie3;
	trie3 = std::move(trie2);
	EXPECT_TRUE(trie3.search("apple"));
	EXPECT_TRUE(trie3.search("banana"));
}

// Test remove operation
TEST(TrieTest, Remove) {
	Trie trie{"apple", "banana", "bar"};

	// Remove a word
	trie.remove("banana");

	// Check it's gone but other words remain
	EXPECT_FALSE(trie.search("banana"));
	EXPECT_TRUE(trie.search("apple"));
	EXPECT_TRUE(trie.search("bar"));

	// Remove another word
	trie.remove("bar");
	EXPECT_FALSE(trie.search("bar"));
	EXPECT_TRUE(trie.search("apple"));

	// Try to remove a non-existent word (should not crash)
	trie.remove("orange");
	EXPECT_TRUE(trie.search("apple"));
}

// Test BFS traversal
TEST(TrieTest, BFSTraversal) {
	Trie trie{"apple", "banana", "app"};

	std::vector<char> visited;
	trie.bfs([&visited](Trie::Node*& node) {
		if (node->data != '\0') {  // Skip root node
			visited.push_back(node->data);
		}
	});

	// Check if all characters are visited (exact order depends on
	// implementation)
	EXPECT_EQ(visited.size(), 11);	// a,p,p,l,e,b,a,n,a,n,a

	// Check if all expected characters are in the result
	std::string chars = "applebanana";
	for (char c : chars) {
		EXPECT_NE(std::find(visited.begin(), visited.end(), c), visited.end());
	}
}

// Test for operator+ (Trie union)
TEST(TrieTest, UnionOperator) {
	Trie trie1{"apple", "banana"};
	Trie trie2{"cherry", "apple"};

	Trie result = trie1 + trie2;

	// Union should contain all words
	EXPECT_TRUE(result.search("apple"));
	EXPECT_TRUE(result.search("banana"));
	EXPECT_TRUE(result.search("cherry"));
}

// Test for operator+= (Trie union in-place)
TEST(TrieTest, UnionAssignmentOperator) {
	Trie trie1{"apple", "banana"};
	Trie trie2{"cherry", "apple"};

	trie1 += trie2;

	// trie1 should now contain all words
	EXPECT_TRUE(trie1.search("apple"));
	EXPECT_TRUE(trie1.search("banana"));
	EXPECT_TRUE(trie1.search("cherry"));
}

// Test for operator- (Trie difference)
TEST(TrieTest, DifferenceOperator) {
	Trie trie1{"apple", "banana", "cherry"};
	Trie trie2{"apple", "orange"};

	Trie result = trie1 - trie2;

	// Result should contain only words in trie1 but not in trie2
	EXPECT_FALSE(result.search("apple"));
	EXPECT_TRUE(result.search("banana"));
	EXPECT_TRUE(result.search("cherry"));
	EXPECT_FALSE(result.search("orange"));
}

// Test for operator-= (Trie difference in-place)
TEST(TrieTest, DifferenceAssignmentOperator) {
	Trie trie1{"apple", "banana", "cherry"};
	Trie trie2{"apple", "orange"};

	trie1 -= trie2;

	// trie1 should now not contain apple
	EXPECT_FALSE(trie1.search("apple"));
	EXPECT_TRUE(trie1.search("banana"));
	EXPECT_TRUE(trie1.search("cherry"));
}

// Test for operator() (functional operator)
TEST(TrieTest, FunctionalOperator) {
	Trie trie{"apple", "banana"};

	// Functional operator should work like search
	EXPECT_TRUE(trie("apple"));
	EXPECT_TRUE(trie("banana"));
	EXPECT_FALSE(trie("cherry"));
}

// Test for operator== and operator!= (equality)
TEST(TrieTest, EqualityOperators) {
	Trie trie1{"apple", "banana"};
	Trie trie2{"apple", "banana"};
	Trie trie3{"apple", "cherry"};

	// Equal tries
	EXPECT_TRUE(trie1 == trie2);
	EXPECT_FALSE(trie1 != trie2);

	// Unequal tries
	EXPECT_FALSE(trie1 == trie3);
	EXPECT_TRUE(trie1 != trie3);
}

// Test I/O operators
TEST(TrieTest, IOOperators) {
	Trie trie1{"apple", "banana"};

	// Write to string stream
	std::stringstream ss;
	ss << trie1;

	// Read back to a new trie
	Trie trie2;
	ss >> trie2;

	// The new trie should contain the same words
	EXPECT_TRUE(trie2.search("apple"));
	EXPECT_TRUE(trie2.search("banana"));
	EXPECT_FALSE(trie2.search("cherry"));
}

// Optional: Test DFS traversal (if implemented for bonus)
TEST(TrieTest, DFSTraversal) {
	Trie trie{"app", "apple"};

	std::vector<char> visited;
	// Uncomment if DFS is implemented
	trie.dfs([&visited](Trie::Node*& node) {
		if (node->data != '\0') {  // Skip root node
			visited.push_back(node->data);
		}
	});

	// Check if all characters are visited
	// Order will be different from BFS
	EXPECT_EQ(visited.size(), 5);  // a,p,p,l,e (from "apple", might include
								   // "app" chars in different order)

	// Check if all expected characters are in the result
	std::string chars = "apple";
	for (char c : chars) {
		EXPECT_NE(std::find(visited.begin(), visited.end(), c), visited.end());
	}
}

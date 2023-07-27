// Dictionary.cpp

#include "Dictionary.h"
#include <algorithm>
#include <functional> // for hash
#include <string>
#include <list>
#include <cctype>
//#include <cassert> if you want to do asserts
#include <utility>  // for swap
using namespace std;

void removeNonLetters(string& s);

  // This class does the real work of the implementation.
  // we are using a semi-resizable hashtable that maintains a constant load factor until maxBuckets is reached
class DictionaryImpl
{
  public:
    DictionaryImpl(int maxBuckets);
    ~DictionaryImpl();
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
  private:
    const unsigned int m_maxB; // once maxBuckets is set, it cannot be changed.
    unsigned int m_B, m_N; // B is number of buckets and N is number of items in the hashtable
    list<string>** m_words; // a pointer to a dynamically allocated array of pointers to lists of strings
    unsigned int hash(string key) const;
    void tryRehash();
};

// Constructor that sets maxBuckets and N
DictionaryImpl::DictionaryImpl(int max_buckets) : m_maxB(max_buckets), m_N(0) {
    m_B = m_maxB > 10 ? 10 : m_maxB; // set B to 10 initially unless maxBuckets is smaller
    m_words = new list<string>*[m_B];
    for (int i = 0; i < m_B; i++) { // initialize the pointers to nullptr
        m_words[i] = nullptr;
    }
}

// Destructor
DictionaryImpl::~DictionaryImpl() {
    // note: it is safe to delete a nullptr
    for (int i = 0; i < m_B; i++) {
        delete m_words[i];
    }
    delete[] m_words;
}

// this function presumes the key is already sorted, if so
// this hash maps all anagrams to the same bucket (it returns the bucket index)
// we'll make it inline since it's relatively simple
inline unsigned int DictionaryImpl::hash(string sortedkey) const {
    return static_cast<unsigned int>(std::hash<string>()(sortedkey) % m_B);
}

// resize and rehash the table if needed to maintain a load factor of 0.7 (if possible)
void DictionaryImpl::tryRehash() {
    // if load factor exceeds 0.7 and we can increase number of buckets:
    if (m_B < m_maxB && m_N / static_cast<float>(m_B) > 0.7f) {
        unsigned int oldSize(m_B);
        m_B = 2*m_B < m_maxB ? 2*m_B : m_maxB; // we double the size (if possible, else increase it up to maxBuckets)
        
        // create our new container with more buckets
        list<string>** newArr = new list<string>*[m_B];
        for (int i = 0; i < m_B; i++) {
            newArr[i] = nullptr;
        }
        
        // transfer old hashtable into new one
        string word;
        for (int i = 0; i < oldSize; i++) {
            if (m_words[i]) {
                for (list<string>::const_iterator it = m_words[i]->begin(); it != m_words[i]->end(); it++) {
                    word = *it;
                    std::sort(word.begin(), word.end()); // ensures that all anagrams are treated the same
                    unsigned int bucket(hash(word)); // get the bucket index for the new hashtable
                    if (!newArr[bucket]) { newArr[bucket] = new list<string>;}
                    newArr[bucket]->push_back(*it); // insert the word into the new hashtable
                }
            }
            delete m_words[i];
        }
        delete[] m_words;
        m_words = newArr;
    }
}

// insert a word into the hashtable
void DictionaryImpl::insert(string word)
{
    m_N++; // increase wordcount
    
    tryRehash();
        
    removeNonLetters(word);
    if (!word.empty()) {
        string temp(word);
        std::sort(temp.begin(), temp.end()); // ensures that all anagrams are treated the same
        unsigned int bucket(hash(temp));
        if (!m_words[bucket]) {m_words[bucket] = new list<string>;}
        m_words[bucket]->push_back(word);
    }
    
    // various assert statements you can uncomment to show it is working as intended
//    assert(m_B <= m_maxB);
//    assert(m_maxB == 50000);
//    assert(m_B == 50000);
}

// apply callback to all anagrams of letters
void DictionaryImpl::lookup(string letters, void callback(string)) const
{
    // callback should be a function
    if (callback == nullptr)
        return;

    // scrap numbers, spaces, etc.
    removeNonLetters(letters);
    if (letters.empty())
        return;
    
    // let's get the bucket index. Anagrams (if any) must be in this bucket.
    std::sort(letters.begin(), letters.end());
    unsigned int bucket = hash(letters);
    if (!m_words[bucket])
        return;
    
    // compare sorted letters to sorted words. If they are equal, they are anagrams
    string test;
    for (list<string>::const_iterator it = m_words[bucket]->begin(); it != m_words[bucket]->end(); it++) {
        test = *it;
        std::sort(test.begin(), test.end());
        if (test == letters) {
            callback(*it);
        }
    }
}

// provided function that removes non alphabetical characters
void removeNonLetters(string& s)
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop everything off from "to" to end.
} 

//******************** Dictionary functions ******************************

// These functions simply delegate to DictionaryImpl's functions
// You probably don't want to change any of this code

Dictionary::Dictionary(int maxBuckets)
{
    m_impl = new DictionaryImpl(maxBuckets);
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters,callback);
}

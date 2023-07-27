#ifndef DICTIONARY_INCLUDED
#define DICTIONARY_INCLUDED

#include <string>

class DictionaryImpl;

class Dictionary
{
  public:
    Dictionary(int maxBuckets = 50000);
    ~Dictionary();
    void insert(std::string word);
    void lookup(std::string letters, void callback(std::string)) const;
      // We prevent Dictionary objects from being copied or assigned
    Dictionary(const Dictionary&) = delete;
    Dictionary& operator=(const Dictionary&) = delete;

  private:
    DictionaryImpl* m_impl;
};

#endif // DICTIONARY_INCLUDED

#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

//========================================================================
// Timer t;                 // create and start a timer
// t.start();               // restart the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

class Timer
{
  public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::chrono::high_resolution_clock::now();
	}
	double elapsed() const
	{
		std::chrono::duration<double,std::milli> diff =
							std::chrono::high_resolution_clock::now() - m_time;
		return diff.count();
	}
  private:
	std::chrono::high_resolution_clock::time_point m_time;
};

void outputCallback(string anagram);
void emptyCallback(string anagram);

void printWord(string w)
{
    cout << w << endl;
}

vector<string> v;  // global variable

void appendWord(string w)
{
    v.push_back(w);
}

//int main()
//{
//    Dictionary d;
//    d.insert("cat");
//    d.insert("dog");
//    d.lookup("tca", printWord);  // writes  cat
//    assert(v.empty());
//    d.lookup("tca", appendWord);
//    assert(v.size() == 1  &&  v[0] == "cat");
//}



int main()
{
	Dictionary dict;

	  // Build dictionary
	{
		cout << "Enter name of dictionary file: ";
		string filename;
		getline(cin, filename);
		ifstream wordFile(filename);
		if ( ! wordFile)
		{
			cout << "Cannot open " << filename << endl;
			return 1;
		}

		string word;
		while (getline(wordFile, word))
			dict.insert(word);
	}

	  // Find and print anagrams
	for (;;)
	{
		cout << "Enter a set of letters: ";
		string letters;
		getline(cin,letters);
		if ( ! cin  ||  letters.empty())
			break;

		cout << "Anagrams of " << letters << ":\n";

		dict.lookup(letters, outputCallback);

		cout << endl;

		Timer t;
		dict.lookup(letters, emptyCallback);
		double result = t.elapsed();

		cout << "The lookup of anagrams takes " << result << " milliseconds." << endl;
	}
    return 0;
}

void outputCallback(string anagram)
{
	cout << "  " << anagram;
}

void emptyCallback(string)
{
	  // For timing purposes, we use a callback function that does nothing,
	  // so that the cost of doing output or whatever else a callback function
	  // might do doesn't enter into the timing of the lookup functionality.
}

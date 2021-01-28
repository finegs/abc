#pragma once

#include <vector>

using namespace std;
class Node {
	public:
		Node();
		~Node();
		char content();
		void setContent(char c);
		bool wordMarker();
		void setWordMarker();
		void appendChild(Node* child);
		Node* findChild(char c);

	private:
		char mContent;
		bool mMarker;
		vector<Node*> mChildren;
};

class Trie {
	public:
		Trie();
		~Trie();
		void addWord(string s);
		bool searchWord(string s);
	private:
		Node* root;
};

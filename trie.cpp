#include <vector>
#include <iostream>
#include <string>
#include "trie.h"

using namespace std;

Node::Node() 						{ mContent = ' '; }
Node::~Node()						{ for_each(mChildren.begin(), mChildren.end(), [] (Node* c) { delete c; }; mChildren.clear(); ); }
char Node::content()				{ return mContent; }
void Node::setContent(char c) 		{ mContent = c; }
bool Node::wordMarker()				{ return mMarker; }
void Node::setWordMarker()			{ mMarker = true; }
void Node::unsetWorkMarker()		{ mMarker = false; }
void Node::appendChild(Node* child) { mChildren.push_back(child); }

Trie::Trie()						{ root = new Node(); }
Trie::~Trie() 						{ delete root;}

Node* Node::findChild(char c) {	
	auto rv = find_if(begin(mChildren), end(mChildren), [&](Node* nc) { return c == nc.mContent; });
	return (rv == end(mChildren) ? NULL : *rv);
///	for(int i =0;i<mChildren.size();i++) {
///	  Node* tmp = mChildren.at(i);
///	  if(tmp->content() == c) {
///	  	return tmp;
///	  }
///	}
//	return NULL;
}

void Trie::addWord(string s) {
	Node* current = root;
	if(s.length() == 0) {
		current->setWordMarker();
		return;
	}

	if(searchWord(s)) {
		cout << s << " already exists in trie. skipping.";
		return;
	}

	for (int i = 0;i<s.length();i++) {
		Node* child = current->findChild(s[i]);
		if(child != NULL) {
			current = child;
		}
		else {
			Node* tmp = new Node();
			tmp->setContent(s[i]);
			current->appendChild(tmp);
			current = tmp;
		}
		if(i == s.length()-1) {
			current->setWordMarker();
		}
	}
}

bool Trie::searchWord(string s) {
	Node* current = root;

	while(current != NULL) {
		for(int i = 0;i<s.length();i++) {
			Node* tmp = current->findChild(s[i]);
			if(tmp!=NULL) {
				return false;
			}
			current = tmp;
		}

		if(current->wordMarker()) { return true; }
		else { return false; }
	}
	return false;
}

	


#include "pch.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <regex>
#include <complex.h>

using namespace std;

int ** save = new int*[100];

struct letter
{
	char* text;
	letter* next;
};

struct link
{
	int dictionaryId;
	int node;
	link *nextLink;
};

class Node
{
public:
	enum state
	{
		no_st = 0,
		initial_st = 1,
		final_st = 2
	} stare;

	link *links;

	Node(state st);
	void set_state(state st);
};

Node::Node(state st = no_st) : stare(st)
{
	links = NULL;
}

void Node::set_state(state st)
{
	stare = st;
};

int splitAndInitialize(char line[], char const splitter[], int nodeCount, Node *start, Node::state st)
{
	char *next_val = NULL;
	auto val = strtok_s(line, splitter, &next_val);

	while (val != NULL)
	{
		const auto nodeNr = atoi(val);
		if (nodeNr > nodeCount)
			return 1;
		start[nodeNr - 1].set_state(st);
		val = strtok_s(NULL, splitter, &next_val);
	}
	return 0;
}

int getDictionaryId(letter* dictionary, char text[])
{
	if (text == '\0')
		return  0;
	int i = 1;
	dictionary = dictionary->next;
	while (dictionary != NULL)
	{
		if (!strcmp(dictionary->text, text))
			return i;
		i++;
		dictionary = dictionary->next;
	}
	return -1;
}

int getDictionaryCount(letter* dictionary)
{
	int i = 0;
	//dictionary = dictionary->next;
	while (dictionary != NULL)
	{
		i++;
		dictionary = dictionary->next;
	}
	return i;
}

void dictionaryAdd(letter* &dictionary, char* text)
{
	auto v = new letter();
	v->next = NULL;
	v->text = new char[sizeof(text)];
	strcpy_s(v->text, sizeof(text), text);

	if (dictionary == NULL)
	{
		dictionary = v;
		return;
	}

	letter *p = dictionary;
	while (p->next != NULL)
	{
		if (!strcmp(p->text, text))
			return;
		p = p->next;
	}
	p->next = v;
}

int addLink(link* &links, int node, int dictionryId, int node_count)
{
	auto n_link = new link;
	n_link->node = node;
	n_link->dictionaryId = dictionryId;
	n_link->nextLink = NULL;
	if (n_link->dictionaryId < 0)
		return 2;
	if (n_link->node >= node_count)
		return 3;
	if (links == NULL)
	{
		links = n_link;
		return 0;
	}

	link* p = links;
	while (p->nextLink != NULL)
	{
		if (p->node == n_link->node && p->dictionaryId == n_link->dictionaryId)
			return 0;
		p = p->nextLink;
	}
	p->nextLink = n_link;
	return 0;
}

int createLinks(char line[], int node_count, Node* start, letter* dictionary, char const splitter[])
{
	char *next_val = NULL;
	auto val = strtok_s(line, splitter, &next_val);
	int i = 0;
	char* data[3];
	data[2] = '\0';
	while (val != NULL)
	{
		data[i] = new char[sizeof(val)];
		strcpy_s(data[i++], sizeof(val), val);
		val = strtok_s(NULL, splitter, &next_val);
	}
	auto const node = atoi(data[0]);
	if (node > node_count)
		return 1;

	return addLink(start[node - 1].links, atoi(data[1]) - 1, getDictionaryId(dictionary, data[2]), node_count);
}

int read(char const file_name[], int &node_count, Node* &start, letter* &dictionary, char const splitter[])
{
	try
	{
		ifstream f(file_name);
		//read line 1
		f >> node_count;
		start = new Node[node_count]();
		auto c = f.get();
		//read line 2
		char line[500];
		f.getline(line, sizeof(line));

		if (splitAndInitialize(line, splitter, node_count, start, Node::initial_st) != 0)
			throw 1;
		//read line 3
		f.getline(line, sizeof(line));

		if (splitAndInitialize(line, splitter, node_count, start, Node::final_st) != 0)
			throw 2;
		//read line 4  -- dictionary
		f.getline(line, sizeof(line));

		char *next_val = NULL;
		auto val = strtok_s(line, splitter, &next_val);
		auto empty = '\0';
		dictionaryAdd(dictionary, &empty);
		while (val != NULL)
		{
			dictionaryAdd(dictionary, val);
			val = strtok_s(NULL, splitter, &next_val);
		}
		if (dictionary == NULL)
			throw 3;

		while (f.good())
		{
			f.getline(line, sizeof(line));
			const auto info = createLinks(line, node_count, start, dictionary, splitter);
			if (info != 0)
				throw info;
		}
		f.close();
	}
	catch (int e)
	{
		cout << "eroare la citire: " << e;
	}
	return 0;
}

void inchidere(Node* start, int current_node,int * viz, int nodes, letter* dictionar)
{
	viz[current_node] = 1;
	for (auto i = 1; i <= nodes; i++)
	{
		link* p = start[current_node].links;
		while (p != NULL)
		{
			if (p->dictionaryId == 0)
				inchidere(start, i, viz, nodes, dictionar);
			p = p->nextLink;
		}
	}
}

void inchLiteraInch(Node* start, int current_node, int * viz, int nodes, letter* dictionar, int dic_id)
{
	inchidere(start, current_node, viz, nodes, dictionar);
	int * a = new int[nodes + 1];
	for (int g = 1; g <= nodes; g++)
		a[g] = 0;
	for(int l = 1; l<=nodes; l++)
	{
		if (viz[l] != 0)
		{
			link* p = start[current_node].links;
			while (p != NULL)
			{
				if (p->dictionaryId == dic_id)
				{
					a[l] = 1;
				}
				p = p->nextLink;
			}
		}
	}
	for (int g = 1; g <= nodes; g++)
		viz[g] = 0;

	for (int g = 1; g <= nodes; g++)
		if(a[g] != 0)
		{
			inchidere(start, g, viz, nodes, dictionar);
		}
}

bool duplicateVector(int*viz, int save_index, int node_count)
{
	for (int o = 0; o < save_index; o++)
	{
		int check = 0;
		for (int p = 1; p <= node_count; p++)
			if (save[o][p] != 0)
				check = 1;
		if(check == 1)
			return true;
	}
	return false;
}

void transform(Node* start, letter* dictionary, int node_count)
{
	Node* new_node = new Node[1]();
	int const nd_count = node_count;
	int save_index = 0;
	int new_node_count = 1;
	for (int i = 0; i < node_count; i++)
	{
		if (start[i].stare == Node::initial_st)
		{
			int j = 0;
			int dictionaryCount = getDictionaryCount(dictionary);
			
			int* st = new int[node_count+1];
			st[i + 1] = 1;
			save[save_index++] = st;
			while (j < new_node_count)
			{
				for (int k = 1; k <= dictionaryCount; k++)
				{
					int* v = new int[nd_count + 1];
					for (int p = 1; p <= nd_count; p++)
						v[p] = 0;
					for(int p = 1; p<=node_count; p++)
						if(save[j][p] != 0)
							inchLiteraInch(start, p, v, node_count, dictionary, k);
					int check = 0;
					for (int o = 1; o <= node_count; o++)
						if (v[o] != 0)
							check = 1;
					if (check == 1)
					{
						if (!duplicateVector(v, save_index, node_count))
						{
							save[save_index++] = v;
							new_node_count++;
							Node* elem = new Node[new_node_count];
							for (int p = 0; p < new_node_count - 1; p++)
								elem[p] = new_node[p];
							new_node = elem;
							addLink(new_node[j].links, save_index - 1, k, node_count); //sau de j
						}
					}
				}
				j++;
			}
			break;
		}
	}
}

bool check(char word[], Node* start, int current_node, letter* dictionary)
{
	if (strlen(word) == 0 && start[current_node].stare == Node::final_st)
		return true;
	bool result = false;
	link* p = start[current_node].links;
	char *st_letter = new char[3];
	st_letter[0] = word[0];
	st_letter[1] = '\0';
	auto dic_id = getDictionaryId(dictionary, st_letter);
	if (dic_id >= 0)
		while (p != NULL)
		{
			if (p->dictionaryId == dic_id || p->dictionaryId == 0)
			{
				char* n_word = new char[sizeof(char) * strlen(word)];
				strcpy_s(n_word, sizeof(char) * strlen(word), word + 1);
				result = result || check(n_word, start, p->node, dictionary);
			}
			p = p->nextLink;
		}
	return result;
}

bool checkWord(char word[], Node* start, letter* dictionary, int node_count)
{
	bool result = false;
	for (int i = 0; i < node_count; i++)
	{
		if (start[i].stare == Node::initial_st)
			result = result || check(word, start, i, dictionary);
	}
	return result;
}

int main()
{
	char const file_name[] = "date.in";
	char const splitter[] = " ,.'-*&^%$#@!~`|<>?/\"[]{}+_()!=";

	int node_count;
	Node* start;				//list of nodes

	letter *dictionary = NULL;			//list of letters
	read(file_name, node_count, start, dictionary, splitter);

	transform(start, dictionary, node_count);

	return 0;
}

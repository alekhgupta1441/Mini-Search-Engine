#ifndef MY_LIST_H
#define MY_LIST_H

#include "posting_list.h"

using namespace std;

class my_trie
{
    private:
        struct node
        {
            char ch;
            node * right;
            node * down;
            posting_list * pl_ptr; // a pointer to its posting list
        };

        unsigned int trie_size;
        node * first;
        void destroy(node * n);
        void print_subtrie(node * n, bool full_print);
        node * create_node(char ch);


    public:
        my_trie();
        ~my_trie();
        void print(bool full_print);
        unsigned int get_size();
        void insert_word(char * word, unsigned int line_id);
        posting_list * search_word(const char * query);

};

#endif // MY_LIST_H

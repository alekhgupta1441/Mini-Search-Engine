#ifndef POSTING_LIST_H
#define POSTING_LIST_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>


using namespace std;

class posting_list
{
    public:
        posting_list();
        ~posting_list();
        void update(char * word, unsigned int doc_id);
        void print(bool full_print);
        unsigned int get_size();
        unsigned int get_term_freq(unsigned int doc_id);
        unsigned int * add_doc_id_to_array(unsigned int * unq_doc_ids, unsigned int & unq_doc_ids_size);



    private:
        struct node
        {
            unsigned int doc_id;
            unsigned int freq;
            node * next;
        };

        unsigned int pl_size; //df
        node * first;
        void destroy();
        char * word;
        node * create_node(unsigned int doc);




};

#endif // POSTING_LIST_H

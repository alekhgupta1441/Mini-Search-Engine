#ifndef MINISEARCH_H
#define MINISEARCH_H

#include "trie.h"


FILE * open_file(char * fn);
unsigned int get_number_of_lines(FILE * fp);
bool is_number(const char *str) ;
bool parse_docs(char **docs, FILE * fp);
unsigned int insert_docs_into_trie(my_trie * my_trie_ptr, char ** docs,
    unsigned int * docs_word_counter, unsigned int nlines);
char ** strip_line_into_words(char * cmd, unsigned int & cmd_nwords);
void delete_cmd_words(char ** cmd_words, unsigned int cmd_nwords);
void delete_docs(char ** docs, unsigned int * docs_word_counter, unsigned int nlines);
char * read_str(unsigned int str_len);
double get_inv_doc_freq(unsigned int ndocs, unsigned int doc_freq);
double get_relevance_score(unsigned int term_freq, unsigned int doc_freq,
    double avgdl, unsigned int doc_nwords, unsigned int ndocs, double k1, double b);
unsigned int * get_unq_doc_ids(char ** cmd_words, posting_list ** pl_array, my_trie * trie_ptr,
    unsigned int nqs, unsigned int & unq_doc_ids_size);
unsigned int get_number_of_digits(int num);
unsigned int calc_whitespace(char * str);
int ** reallocate_underline_info(int ** underline_info, unsigned int underline_info_size, unsigned int ul_start, unsigned int ul_end);
void init_newline(unsigned int init_llen, unsigned int & curr_llen);
bool manage_whitespace(char * rest_line, unsigned int lwidth, unsigned int init_llen, unsigned int & curr_llen,
                       int ** underline_info, int & underline_info_size);
char * check_program_args(char ** argv, int argc, unsigned int & K);
void underline(unsigned int init_llen, unsigned int & curr_llen, int ** underline_info, int & underline_info_size, bool exit);
void print_k_docs(char ** docs, char ** queries, unsigned int * unq_doc_ids, unsigned int * docs_word_counter, posting_list ** pl_array,
                  unsigned int unq_doc_ids_size, unsigned int nqs, double avgdl, unsigned int ndocs, unsigned int K, double k1,
                  double b, unsigned int lwidth);
bool search_doc_word_into_queries(char * doc_word, char ** queries, unsigned int nqs);


#endif

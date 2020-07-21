#include "minisearch.h"

FILE * open_file(char * fn)
{
    FILE* random_file;
    random_file = fopen(fn, "r");

    if (!random_file)
    {
        fprintf(stderr, "-! ERROR - docfile does not exist !-\n");
        exit(1);
    }

    return random_file;
}


unsigned int get_number_of_lines(FILE * fp)
{
    unsigned int lines = 0;
    int ch ;

    while(!feof(fp))
    {
      ch = fgetc(fp);
      if(ch == '\n')
      {
        lines++;
      }
    }

    rewind(fp);
    return lines ;
}


bool is_number(const char *str)
{
    for(unsigned int i=0; i < strlen(str); i++)
    { //iterating the whole string character by character
        if(!isdigit(str[i]))
        {// and checking if every character is a number
            return 0;
        }
    }

    return 1;
}


bool parse_docs(char **docs, FILE * fp)
{// storing docfile's lines into docs array - checking docfile for errors
    char * line = NULL;
    size_t len = 0;
    ssize_t nchars;
    int line_index = 0;

    while ((nchars = getline(&line, &len, fp)) != -1)
    {
        if ( line[nchars - 1] == '\n')
        { // removing newline character from string
            line[nchars - 1] = '\0';
            nchars--;
        }

        char *content;
        char *line_id_str = strtok_r(line, " \t", &content);

        if(line_id_str == NULL)
        { // checking if there is a line id
            cout<<"-! ERROR line is empty (without id) !-"<<endl;
            free(line);
            return 1;
        }

        int line_id = strtol(line_id_str, NULL, 10);

        if(is_number(line_id_str) == 1)
        { //checking if line id is a number
            if(line_id != line_index)
            { //checking if indexing is correct
                cout<<"-! ERROR at line indexing !-"<<endl;
                free(line);
                return 1;
            }
        }
        else
        {
            cout<<"-! ERROR line id is not a number !-"<<endl;
            free(line);
            return 1;
        }

        docs[line_index] = (char *) malloc((strlen(content) + 1) * sizeof(char));  //plus one for \0
        strcpy(docs[line_index], content);  //storing docfile's line into doc array
        line_index++;
    }

    free(line);
    return 0;
}


unsigned int insert_docs_into_trie(my_trie * my_trie_ptr, char ** docs,
    unsigned int * docs_word_counter, unsigned int nlines)
{//inserting every doc's word into trie, counting each doc's number of words - returns total number of all docs' words
    unsigned int total_word_counter = 0;

    for(unsigned int i = 0; i < nlines; i++)
    { //for each doc
        char * line_cp = (char *) malloc((strlen(docs[i]) + 1) * sizeof(char));
        strcpy(line_cp, docs[i]);
        char * word = strtok (line_cp, " \t");
        unsigned int line_word_counter = 0;

        while (word != NULL)
        {//for each doc's word
            line_word_counter++;
            my_trie_ptr->insert_word(word, i);
            word = strtok (NULL, " \t");
        }

        free(line_cp);

        docs_word_counter[i] = line_word_counter;
        total_word_counter += line_word_counter;
    }

    return total_word_counter;

}


char ** strip_line_into_words(char * cmd, unsigned int & cmd_nwords)
{//returns a char * array which contains each cmd's word
    char ** cmd_words = NULL;
    char * word = strtok (cmd, " \t");

    unsigned int word_counter = 0;

    while (word != NULL)
    { //for each cmd's word
        cmd_words = (char **) realloc(cmd_words, (word_counter + 1) * sizeof(char *));
        cmd_words[word_counter] = (char *) malloc((strlen(word) + 1) * sizeof(char));  // plus one for \0
        strcpy(cmd_words[word_counter], word);
        word_counter++;
        word = strtok (NULL, " \t");
    }

    cmd_nwords = word_counter;
    return cmd_words;
}


void delete_cmd_words(char ** cmd_words, unsigned int cmd_nwords)
{
    for(unsigned int i = 0; i < cmd_nwords; i++)
    {
        free(cmd_words[i]);
    }

    free(cmd_words);
}


void delete_docs(char ** docs, unsigned int * docs_word_counter, unsigned int nlines)
{
    for(unsigned int i = 0; i < nlines; i++)
    {
        if (docs[i] != NULL)
        {
            free(docs[i]);
        }
    }

    free(docs);
    free(docs_word_counter);
}


char * read_str(unsigned int str_len)
{ //reads and returns string of unknown length
    char * str = (char *) malloc((str_len) * sizeof(char));
    int ch;
    unsigned int i = 0;

    while(((ch = getchar()) != '\n') && (ch != EOF))
    {
        str[i] = ch;
        i++;
        if(i == str_len)
        {
            str_len++;
            str = (char *) realloc(str, str_len * sizeof(char));
        }
    }

    str[i] = '\0';  // add \0 at the end of the string
    return str;
}


double get_inv_doc_freq(unsigned int ndocs, unsigned int doc_freq)
{
    return log10((ndocs - doc_freq + 0.5) / (doc_freq + 0.5));
}


double get_relevance_score(unsigned int term_freq, unsigned int doc_freq,
    double avgdl, unsigned int doc_nwords, unsigned int ndocs, double k1, double b)
{
    double inv_doc_freq = get_inv_doc_freq(ndocs, doc_freq);
    return inv_doc_freq * ((term_freq * (k1 + 1)) / (term_freq + k1 * (1 - b + b * (doc_nwords / avgdl))));
}


unsigned int * get_unq_doc_ids(char ** cmd_words, posting_list ** pl_array, my_trie * trie_ptr,
    unsigned int nqs, unsigned int & unq_doc_ids_size)
{//returns an array that contains every unique doc id taken from each query's posting list
 //also all queries' posting lists pointers are stored in an array
    unsigned int * unq_doc_ids = NULL;
    unq_doc_ids_size = 0;

    for(unsigned int i = 1; i <= nqs; i++)
    { // for each query (max = 10)
        posting_list * pl_ptr = trie_ptr->search_word(cmd_words[i]);  //check if query exists in trie
        pl_array[i-1] = pl_ptr;  //store query's posting list pointer

        if(pl_ptr != NULL)
        { // if query exists
            unq_doc_ids = pl_ptr->add_doc_id_to_array(unq_doc_ids, unq_doc_ids_size);  //update unique doc ids array from query's posting list
        }
    }

    return unq_doc_ids;
}


unsigned int get_number_of_digits(int num)
{
    if (num == 0)
    {
        return 1;
    }
    else
    {
        return floor(log10(abs(num))) + 1;   //https://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c
    }
}


unsigned int calc_whitespace(char * str)
{//returns number of spaces at the beginning of the string
    if(str == NULL)
    {
        return 0;
    }

    unsigned int whitespace_counter = 1;
    bool all_whitespace = 1;

    for(unsigned int i = 0; i < strlen(str); i++)
    {
        if(str[i] == ' ')
        {
            whitespace_counter++;
        }
        else
        {
            all_whitespace = 0;
            break;
        }
    }

    if(all_whitespace == 1)
    { // if the whole string is whitespace
        return 0;
    }
    else
    {
        return whitespace_counter;
    }
}


void underline(unsigned int init_llen, unsigned int & curr_llen, int ** underline_info, int & underline_info_size, bool exit)
{//underlining words based on the underline info array

    cout<<endl;
    for(int i = 0; i < *underline_info[0]; i++)
    {//set cursor to the correct spot
        cout<<" ";
    }

    for(int i = 0; i <= underline_info_size - 2; i+=2)
    {
        unsigned int start_ul = *underline_info[i];
        unsigned int end_ul = *underline_info[i+1];

        for(unsigned int j = start_ul; j < end_ul; j++)
        {//underline word
            cout<<"^";
        }

        if(i != (underline_info_size - 2))
        {//if there are more than one words that needs underlining set cursor to the correct spot
            unsigned int start_sp = end_ul;
            unsigned int end_sp = *underline_info[i+2];

            for(unsigned int j = start_sp; j < end_sp; j++)
            {
                cout<<" ";
            }
        }
    }

    for(int i = 0; i < underline_info_size; i++)
    {
        free(underline_info[i]);
    }

    underline_info = NULL;
    underline_info_size = 0;

    if(exit == 0)
    {// if there are more words to be printed set cursor to the correct spot
        init_newline(init_llen, curr_llen);
    }
}


void init_newline(unsigned int init_llen, unsigned int & curr_llen)
{//go to new line and set cursor to the correct spot for printing
    cout<<endl;
    curr_llen = init_llen;
    for(unsigned int i = 0; i < init_llen; i++)
    {
        cout<<" ";
    }
}


bool manage_whitespace(char * rest_line, unsigned int lwidth, unsigned int init_llen, unsigned int & curr_llen,
                       int ** underline_info, int & underline_info_size)
{
    unsigned int whitespace = calc_whitespace(rest_line); //calculate the whitespace from the beginning of the rest content
    if (whitespace == 0)
    {//the whole rest content is whitespace
        if (underline_info_size > 0)
        {//if there are some words to be underlined
            underline(init_llen, curr_llen, underline_info, underline_info_size, 1);
        }

        return 1;  //send stop signal
    }
    else
    {
        for(unsigned int i = 0; i < whitespace; i++)
        {//print whitespace space by space
            if ((curr_llen + 1) > lwidth)
            {// if there is no free space left in terminal's line
                if (underline_info_size > 0)
                {
                    underline(init_llen, curr_llen, underline_info, underline_info_size, 0);
                }
                else
                {
                    init_newline(init_llen, curr_llen);
                }
            }

            curr_llen++;
            cout<<" ";
        }

        return 0;  //keep printing
    }
}


char * check_program_args(char ** argv, int argc, unsigned int & K)
{
    if ((argc < 3) || (argc > 5))
    {
        cout<<"-! ERROR - Wrong number of arguments is given !-"<<endl;
        exit(1);
    }

    char * docfile = NULL;
    int kappa;
    bool k_found = 0;

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-i") == 0)
        {// if -i parameter found the next argument must be docfile's name
            if((i + 1) == argc)
            {
                cout<<"-! ERROR - Docfile's name not found !-"<<endl;
                exit(1);
            }
            else
            {
                docfile = argv[i + 1];
                i++;
            }
        }
        else if(strcmp(argv[i], "-k") == 0)
        {
            if((i + 1) == argc)
            {
                cout<<"-! ERROR - K's value not found !-"<<endl;
                exit(1);
            }
            else
            {
                if(!is_number(argv[i + 1]))
                {
                    cout<<"-> WARNING - K's value is not a number. K will be set by default with 10 <-"<<endl;
                    kappa = 10;
                }
                else
                {
                    kappa = atoi(argv[i + 1]);
                }

                k_found = 1;
                i++;
            }
        }
        else
        {
            cout<<"-! ERROR - invalid parameters given !-"<<endl;
            exit(1);
        }
    }

    if(docfile == NULL)
    {
        cout<<"-! ERROR - Docfile's -i parameter not found !-"<<endl;
        exit(1);
    }

    if(k_found == 0)
    {
        cout<<"-> WARNING - K's -k parameter not found. K will be set by default with 10 <-"<<endl;
        K = 10;
    }
    else
    {
        if (kappa < 1)
        {
            cout<<"-> WARNING - K's value is less than 1. K will be set by default with 10 <-"<<endl;
            K = 10;
        }
        else
        {
            K = kappa;
        }
    }

    return docfile;

}


int ** reallocate_underline_info(int ** underline_info, unsigned int underline_info_size, unsigned int ul_start, unsigned int ul_end)
{
    underline_info = (int **) realloc(underline_info, underline_info_size * sizeof(int *));
    underline_info[underline_info_size - 2] = (int *) malloc(sizeof(int));
    underline_info[underline_info_size - 1] = (int *) malloc(sizeof(int));
    *underline_info[underline_info_size - 2] = ul_start;
    *underline_info[underline_info_size - 1] = ul_end;
    return underline_info;
}


bool search_doc_word_into_queries(char * doc_word, char ** queries, unsigned int nqs)
{//returns 1 if doc_word is contained in queries and 0 otherwise
    for(unsigned int i = 1; i <= nqs; i++)
    {
        if (strcmp(queries[i], doc_word) == 0)
        {
            return 1;
        }
    }

    return 0;
}


void print_k_docs(char ** docs, char ** queries, unsigned int * unq_doc_ids, unsigned int * docs_word_counter, posting_list ** pl_array,
     unsigned int unq_doc_ids_size, unsigned int nqs, double avgdl, unsigned int ndocs, unsigned int K, double k1,
     double b, unsigned int lwidth)
{
    double * unq_docs_relev_score = (double *) malloc(unq_doc_ids_size * sizeof(double)); //stores each unique doc's score

    for(unsigned int i = 0; i < unq_doc_ids_size; i++)
    {//for each unique doc
        double total_relev_score = 0.0 ;
        for(unsigned int j = 0 ; j < nqs; j++)
        {//for each query
            posting_list * query_pl_ptr = pl_array[j];
            unsigned int term_freq, doc_freq ;

            if(query_pl_ptr == NULL)
            {
                term_freq = 0;
                doc_freq = 0;
            }
            else
            {
                term_freq = query_pl_ptr->get_term_freq(unq_doc_ids[i]);
                doc_freq = query_pl_ptr->get_size();
            }

            total_relev_score += get_relevance_score(term_freq, doc_freq, avgdl, docs_word_counter[unq_doc_ids[i]], ndocs, k1, b);
        }

        unq_docs_relev_score[i] = total_relev_score;
    }


    //sorting unique doc ids and their corresponding relevance scores at the same time based on scores' values
    for(unsigned int i = 0; i < unq_doc_ids_size; i++)
    {
        for(unsigned int j = i+1; j < unq_doc_ids_size; j++)
        {
            if(unq_docs_relev_score[i] < unq_docs_relev_score[j])
            {
                double temp = unq_docs_relev_score[i];
                unsigned int temp2 = unq_doc_ids[i];

                unq_docs_relev_score[i] = unq_docs_relev_score[j];
                unq_docs_relev_score[j] = temp;

                unq_doc_ids[i] = unq_doc_ids[j];
                unq_doc_ids[j] = temp2;
            }
        }
    }

    for(unsigned int i = 0 ; i < unq_doc_ids_size; i++)
    {//for each unique doc
        if (i == K) //print only the top K docs
        {
            break;
        }

        //calculating the width for doc's info (score, doc id, index, etc )
        unsigned int doc_id_len = get_number_of_digits(int(unq_doc_ids[i]));
        unsigned int i_len = get_number_of_digits(int(i));
        double integral;
        modf(unq_docs_relev_score[i], &integral);
        unsigned int score_dec_part_len = get_number_of_digits(int(integral));
        unsigned int init_llen= doc_id_len + i_len + score_dec_part_len + 8 + 5 + 1;
        if(unq_docs_relev_score[i] < 0)
        {
            init_llen++;
        }

        char * doc = docs[unq_doc_ids[i]];
        char * doc_cpy = (char *)malloc((strlen(doc) + 1) * sizeof(char));
        strcpy(doc_cpy, doc);
        char *rest_line;
        char *curr_word = strtok_r(doc_cpy, " \t", &rest_line);

        int ** underline_info = NULL; //indicates which words needs underlining
        int underline_info_size = 0;
        unsigned int curr_llen = init_llen; //indicates how nuch of terminal's usefull printing space is occupied at every moment
        bool next_word = 0;

        cout.precision(5);
        cout<<i+1<<".( "<<unq_doc_ids[i]<<") ["<<fixed<< unq_docs_relev_score[i]  <<"] ";

            while(curr_word != NULL)
            {
                if ((strlen(curr_word) > (lwidth - init_llen)) && (curr_llen != lwidth))
                {//if word's length is bigger than terminal's printing space
                    unsigned int init_curr_llen = curr_llen;
                    bool new_line = 0 ;
                    bool word_exist = search_doc_word_into_queries(curr_word, queries, nqs);

                    for(unsigned int i = 0; i < strlen(curr_word); i++)
                    {//printing word letter by letter - word will be seperated in different lines
                        if ((curr_llen + 1) > lwidth)
                        {
                            if (word_exist == 1)
                            {
                                if (new_line == 0)
                                {//if word has not yet been seperated in different lines
                                    underline_info_size += 2;
                                    underline_info = reallocate_underline_info(underline_info, underline_info_size, init_curr_llen, lwidth);
                                }
                                else
                                {
                                    underline_info_size = 2;
                                    underline_info = reallocate_underline_info(underline_info, underline_info_size, init_llen, lwidth);
                                }

                                underline(init_llen, curr_llen, underline_info, underline_info_size, 0);
                            }
                            else
                            {
                                if (underline_info_size > 0)
                                {
                                    underline(init_llen, curr_llen, underline_info, underline_info_size, 0);
                                }
                                else
                                {
                                    init_newline(init_llen, curr_llen);
                                }
                            }

                            new_line = 1;
                        }

                        curr_llen++;
                        cout<<curr_word[i];
                    }

                    if (word_exist == 1)
                    {
                        underline_info_size = 2;
                        underline_info = reallocate_underline_info(underline_info, underline_info_size, init_llen, curr_llen);
                    }

                    bool exit = manage_whitespace(rest_line, lwidth, init_llen, curr_llen, underline_info, underline_info_size);
                    if(exit == 1)
                    {
                        break;
                    }

                    next_word = 1;
                }
                else if (strlen(curr_word) <= (lwidth - curr_llen))
                {//if word can fit in remaining terminal's line space
                    bool word_exist = search_doc_word_into_queries(curr_word, queries, nqs);
                    if (word_exist == 1)
                    {
                        underline_info_size += 2;
                        underline_info = reallocate_underline_info(underline_info, underline_info_size, curr_llen, curr_llen + strlen(curr_word));
                    }

                    cout<<curr_word;
                    curr_llen += strlen(curr_word);
                    next_word = 1;

                    bool exit = manage_whitespace(rest_line, lwidth, init_llen, curr_llen, underline_info, underline_info_size);
                    if (exit == 1)
                    {
                        break;
                    }
                }
                else
                {//if word can not fit in remaining terminal's line space
                    next_word = 0;

                    if (underline_info_size > 0)
                    {
                        underline(init_llen, curr_llen, underline_info, underline_info_size, 0);
                    }
                    else
                    {
                        init_newline(init_llen, curr_llen);
                    }
                }

                if(next_word == 1)
                {//take next word for printing
                    curr_word = strtok_r(rest_line, " \t", &rest_line);
                }
            }

            cout<<endl;
            for(unsigned int i = 0; i < lwidth; i++)
            {
                cout<<"=";
            }
            cout<<endl;

            free(underline_info);
            free(doc_cpy);
    }

    free(unq_docs_relev_score);
}

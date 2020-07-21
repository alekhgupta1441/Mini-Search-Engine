#include "trie.h"

my_trie::my_trie()
{
    first = NULL;
    trie_size = 0 ;
}


my_trie::~my_trie()
{
    destroy(first); //destroy trie recursively starting from its root
}


void my_trie::destroy(node * n)
{
    if (n != NULL)
    {
        destroy(n->down);
        destroy(n->right);
        if(n->pl_ptr != NULL)
        { //delete node's posting list before deleting the node itself
            delete n->pl_ptr;
        }
        delete n;
        trie_size--;
    }
}


my_trie::node * my_trie::create_node(char ch)
{
    node * new_node = new node;
    new_node->ch = ch;
    new_node->down = NULL;
    new_node->right = NULL;
    new_node->pl_ptr = NULL;
    return new_node;
}


void my_trie::insert_word(char * word, unsigned int doc_id)
{ // inseting a word into the trie or updating its posting list if word is already contained
  // every orizontal list is sorted alphabetically
    node * parent_node = NULL; //will indicate the parent of the first node of an orizontal list
    node * start_node = first; //will indicate the first node of an orizontal list

    for(unsigned int i = 0; i < strlen(word); i++)
    { //iterating word letter by letter
        node * prev_temp = NULL;  // will be used for iterating an orizontal list
        node * temp = start_node; // will be used for iterating an orizontal list
        node * n = NULL;
        bool reallocate_parent_flag = 0;

        if(temp == NULL)
        { // a new orizontal list will be created
            n = create_node(word[i]);  //this will be its first node
            reallocate_parent_flag = 1;
        }
        else
        { // a orizontal list already exists
            while(1) //iterating the orizontal list
            {
                if(temp == NULL)
                { //we have iterated the whole list and no same letter has been found. new letter will be added at the end of the list
                    n = create_node(word[i]);
                    prev_temp->right = n;
                    break;
                }

                if(temp->ch == word[i])
                { // we have found a same letter
                    parent_node = temp;
                    start_node = temp->down; //we will continue looking downwards
                    break;
                }
                else if(temp->ch < word[i])
                {// keep searching the orizontal list
                    prev_temp = temp;
                    temp = temp->right;
                }
                else
                {// no same letter found so far and no same letter will be found in this orizontal list because it is sorted alphabetically
                 // and current letter is bigger than the inserting one
                    n = create_node(word[i]);
                    n->right = temp;

                    if(prev_temp == NULL) //adding new letter at the start of the orizontal list
                    {
                        reallocate_parent_flag = 1;
                    }
                    else //adding new letter in the middle of the orizontal list
                    {
                        prev_temp->right = n ;
                    }

                    break;
                }
            }
        }

        if(n != NULL)  // a new node has been created (a new letter has been added)
        { // checking if parent's node down pointer needs to be reallocated
            if (reallocate_parent_flag == 1)
            {
                if (parent_node != NULL)
                {
                    parent_node->down = n;
                }
                else
                {
                    first = n;
                }
            }

            trie_size++;
            parent_node = n;
            start_node = n->down; //NULL
        }
    }

    //all letters of the inserting word have been consumed
    if(parent_node->pl_ptr == NULL)
    { // if the last's letter node does not have an existing posting list (inserting word is not contained in the trie)
        parent_node->pl_ptr = new posting_list(); //create one
    }

    parent_node->pl_ptr->update(word, doc_id);  //inserting word already exists, therefore update its posting list

}


void my_trie::print(bool full_print)
{
    if(first == NULL)
    {
        cout<<"-> Trie cannot be printed because it is empty !"<<endl;
        return;
    }

    if(full_print == 1)
    {
        cout<<"-----> Printing whole trie <-----"<<endl;
    }
    else
    {
        cout<<"---> Printing document frequency vector <---"<<endl;
    }

    print_subtrie(first, full_print); //print trie recursively starting from the root
}


void my_trie::print_subtrie(node * n, bool full_print)
{
    if (n != NULL)
    {
        if(n->pl_ptr != NULL)
        {
            n->pl_ptr->print(full_print);
        }
        print_subtrie(n->down, full_print);
        print_subtrie(n->right, full_print);
    }
}


posting_list * my_trie::search_word(const char * query)
{ //if word exists in the trie return its posting list or NULL otherwise
    node * start_node = first;
    node * last_char_node = NULL;

    for(unsigned int i = 0; i < strlen(query); i++)
    { //iterating query word letter by letter
        node * temp = start_node;

        while(1)
        {
            if (temp == NULL) //we have interated the whole orizontal list and a query word's letter does not exist,
                              // therefore the whole word does not exist
            {
                return NULL;  //query not found
            }

            if(temp->ch == query[i])
            {// a query word's letter has been found so keep searching downwards
                last_char_node = temp;
                start_node = temp->down;
                break;
            }
            else if(temp->ch > query[i])
            {//stop searching. orizontal list is sorted alphabetically
                return NULL;  // query not found
            }
            else
            {//keep iterating the orizontal list
                temp = temp->right;
            }
        }
    }

    return last_char_node->pl_ptr;
}


unsigned int my_trie::get_size()
{
    return trie_size;
}

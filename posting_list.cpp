#include "posting_list.h"

posting_list::posting_list()
{
    first = NULL;
    word = NULL;
    pl_size = 0;
}


void posting_list::print(bool full_print)
{
    if (full_print == 0) //only print df
    {
        cout<<word<<"  "<<pl_size<<endl;
        return;
    }

    node * temp = first;
    if (first == NULL)
    {
        cout<<"Posting list is empty"<<endl;
        return;
    }

    cout<<"-> word:'"<<word<<"'  "<<pl_size<<"  ";

    while(temp != NULL) //print whole posting list
    {
        cout<<"["<<temp->doc_id<<", "<<temp->freq<<"]"<<"  ";
        temp = temp->next;
    }

    cout<<endl;
}


unsigned int posting_list::get_size()
{
    return pl_size; //df
}


posting_list::~posting_list()
{
    node * temp = first;
    node * temp2;

    while(temp != NULL) //iterate list
    {
        temp2 = temp->next;
        delete temp;
        temp = temp2;
        pl_size--;
    }

    free(word);
}


posting_list::node * posting_list::create_node(unsigned int doc_id)
{
    node * new_node = new node;
    new_node->doc_id = doc_id;
    new_node->freq = 1;
    new_node->next = NULL ;
    return new_node;
}


void posting_list::update(char * word, unsigned int doc_id)
{ //create new list node if doc id is not contained in the list or increase its term frequency otherwise
  // posting list is sorted
    node * temp = first;
    node * prev_temp;
    node * n;

    if(first == NULL)
    { // list is empty
        n = create_node(doc_id);
        first = n;
        this->word = (char *) malloc((strlen(word) + 1) * sizeof(char));
        strcpy(this->word, word);
        pl_size++;
    }
    else
    { // list already exists
        while(1)
        {
            if(temp == NULL)
            {// we have iterated the whole list and doc id does not exist. so it will be added at the end of the list
                n = create_node(doc_id);
                pl_size++;
                prev_temp->next = n;
                break;
            }

            if (temp->doc_id == doc_id)
            { // doc id exists
                temp->freq++;     //increase its term frequency
                break;
            }
            else if(temp->doc_id < doc_id)
            { //continue searching
                prev_temp = temp;
                temp = temp->next;
            }
            else
            {//because the list is sorted and the current doc id is bigger than the inserting one
             //we do not have to keep looking the rest of the list (the rest stored doc ids will be different than the inserting one)
             // doc id will be added here
                n = create_node(doc_id);
                pl_size++;
                n->next = temp;

                if(prev_temp == NULL)
                { //we be added at the start of the list
                    first = n;
                }
                else
                {
                    prev_temp->next = n;
                }

                break;
            }
        }
    }
}


unsigned int posting_list::get_term_freq(unsigned int doc_id)
{
    node * temp = first;

    while(temp != NULL) //iterating posting list
    {
        if(temp->doc_id == doc_id)
        {
            return temp->freq;  //found it
        }
        else if(temp->doc_id > doc_id)
        { // there is no point keep searching because list is sorted
            return 0; //doc id not found
        }
        else
        {
            temp = temp->next;
        }
    }

    return 0; // we have searched the whole list and doc id not found
}


unsigned int * posting_list::add_doc_id_to_array(unsigned int * unq_doc_ids, unsigned int & unq_doc_ids_size)
{// adding posting list's doc ids to an array. if a posting list's doc id is already contained in the array do not add it again
    node * temp = first;
    while(temp != NULL) //iterating posting list
    {
        bool doc_id_exists = 0;
        for(unsigned int i = 0; i < unq_doc_ids_size; i++)
        { // checking if current doc id is already contained in the array
            if (unq_doc_ids[i] == temp->doc_id)
            {
                doc_id_exists = 1;  // already exists
                break;
            }
        }

        if(doc_id_exists == 0)
        { //if current doc id is not already contained in the array add it now
            unq_doc_ids_size++;
            unq_doc_ids = (unsigned int *) realloc(unq_doc_ids, unq_doc_ids_size * sizeof(unsigned int));
            unq_doc_ids[unq_doc_ids_size - 1] = temp->doc_id;
        }

        temp = temp->next;
    }

    return unq_doc_ids;
}

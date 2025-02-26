#include "markov_chain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constants
#define SUCCESS 0
#define FAILURE 1
#define INITIAL_FREQUENCY 1
#define MIN_TWEET_LENGTH 2

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}
int is_last_char_dot(const char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '.')
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
    if (markov_chain == NULL || data_ptr == NULL) {
        return NULL;
    }

    Node *current_node = markov_chain->database->first;

    while (current_node != NULL) {
        if (strcmp(current_node->data->
        data, data_ptr) == 0) {
            return current_node;
        }
        current_node = current_node->next;
    }
    return NULL;
}

void free_frequency_lst(MarkovNode *markov_node)
{
    if(markov_node==NULL)
    {
        return;
    }
    free(markov_node->data);
    free(markov_node->frequency_list);
    markov_node->frequency_list=NULL;
    markov_node->frequency_list_length=0;
}
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr)
{
    if (markov_chain == NULL || data_ptr == NULL) {
        return NULL;
    }
    Node *node_exist = get_node_from_database(markov_chain, data_ptr);
    if (node_exist != NULL) {
        return node_exist;
    }
    MarkovNode *markov_node = calloc(1, sizeof(MarkovNode));
    if (markov_node == NULL) {
        return NULL;
    }
    markov_node->data= malloc(strlen(data_ptr)+1);
    if(!markov_node->data)
    {
        free(markov_node);
        return NULL;
    }
    strcpy(markov_node->data,data_ptr);
    markov_node->frequency_list_length=0;
    if(add(markov_chain->database,
           markov_node)==1)
    {
        free(markov_node->data);
        free(markov_node);
        return NULL;
    }
    return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{
    if (first_node == NULL || second_node == NULL) {
        return FAILURE;
    }
    if(is_last_char_dot(first_node->data) == SUCCESS)
    {
        return SUCCESS;
    }
    for (size_t i = 0; i < first_node->
    frequency_list_length; i++) {
        if (strcmp(first_node->frequency_list[i].
        markov_node->data,second_node->data)==0) {
            first_node->frequency_list[i].frequency++;
            return SUCCESS;
        }
    }
    MarkovNodeFrequency *new_frequency_list =
            realloc(first_node->frequency_list,
                    sizeof(MarkovNodeFrequency) *
                    (first_node->frequency_list_length + 1));
    if (new_frequency_list == NULL) {
        return FAILURE;
    }
    first_node->frequency_list = new_frequency_list;
    first_node->frequency_list[first_node->frequency_list_length]
    .markov_node = second_node;
    first_node->frequency_list[first_node->frequency_list_length]
    .frequency = INITIAL_FREQUENCY;
    first_node->frequency_list_length++;
    return SUCCESS;
}

void free_database(MarkovChain **ptr_chain)
{
    if(ptr_chain==NULL || (*ptr_chain)==NULL)
    {
        return;
    }
    if((*ptr_chain)->database==NULL)
    {
        free(*ptr_chain);
        (*ptr_chain)=NULL;
        return;
    }
    if((*ptr_chain)->database->first==NULL)
    {
        free((*ptr_chain)->database);
        free(*ptr_chain);
        (*ptr_chain)=NULL;
        return;
    }
    Node *temp_markov_chain=(*ptr_chain)->database->first;
    while (temp_markov_chain!=NULL)
    {
        Node *current_node=temp_markov_chain;
        temp_markov_chain=temp_markov_chain->next;
        if(current_node->data!=NULL)
        {
            free_frequency_lst
            (current_node->data);
        }
        free(current_node->data);
        free(current_node);
    }
    free(((*ptr_chain)->database));
    free(*ptr_chain);
    (*ptr_chain)=NULL;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    if(markov_chain==NULL || markov_chain->database==NULL
    || markov_chain->database->size==0)
    {
        return NULL;
    }
    int flag = 1;
    MarkovNode *ret_node = NULL;
    while (flag == 1) {
        int random_index = get_random_number
                (markov_chain->database->size);
        Node *current_node=markov_chain->database->first;
        for (int i = 0; i < random_index; i++) {
            current_node=current_node->next;
        }
        ret_node=current_node->data;
        if(is_last_char_dot(ret_node->data)==1)
        {
            flag=0;
        }
    }
    return ret_node;
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    if (cur_markov_node == NULL || cur_markov_node->
    frequency_list_length == 0) {
        return NULL;
    }
    int total_frequency = 0;
    for (size_t i = 0; i < cur_markov_node->
    frequency_list_length; i++) {
        total_frequency += cur_markov_node->
                frequency_list[i].frequency;
    }
    if(total_frequency==0)
    {return NULL;}
    int random_index = get_random_number
            (total_frequency);
    for(size_t i=0;i<cur_markov_node->
    frequency_list_length;i++)
    {
        random_index -= cur_markov_node->
                frequency_list[i].frequency;

        if(random_index < 0)
        {
            return cur_markov_node->
            frequency_list[i].markov_node;
        }
    }
    return NULL;
}

void generate_tweet(MarkovNode *first_node, const int max_length)
{
    MarkovNode *current_node = first_node;
    printf("%s", current_node->data);
    int length = 1;
    while (length < max_length) {
        MarkovNode *next_node= get_next_random_node
                (current_node);
        if(next_node==NULL)
        {
            break;
        }
        printf(" %s", next_node->data);
        length++;
        if (is_last_char_dot(next_node->data)
        == SUCCESS) {
            break;
        }
        current_node = next_node;
    }
    printf("\n");
}

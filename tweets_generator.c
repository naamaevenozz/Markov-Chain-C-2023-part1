#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"
#define MAX_TWEET_LENGTH 20
#define ALLOCATION_ERROR_MESSAGE "Error: memory allocation failed"
#define INITIAL_WORDS_TO_READ -1
#define LINE_BUFFER_SIZE 1001
#define MIN_ARGC 4
#define MAX_ARGC 5
#define EXIT_FAIL 1
#define EXIT_SUCCESS 0
#define BASE 10


#define DELIMITERS " \n\t\r"

int fill_database(FILE *fp, int words_to_read,
                  MarkovChain *markov_chain) {
    if (fp == NULL || markov_chain == NULL) {
        return EXIT_FAIL;
    }
    char line[LINE_BUFFER_SIZE];
    char *word;
    int words_read = 0;
    Node *previous_node = NULL;

    while ((words_to_read == INITIAL_WORDS_TO_READ ||
    words_read < words_to_read) && fgets(line, sizeof(line), fp) != NULL) {
        word = strtok(line, DELIMITERS);
        while ((words_to_read == INITIAL_WORDS_TO_READ ||
        words_read < words_to_read) && word != NULL) {
            Node *added_node = add_to_database
                    (markov_chain, word);
            if (added_node == NULL) {
                return EXIT_FAIL;
            }
            if (previous_node != NULL) {
                add_node_to_frequency_list
                (previous_node->data,
                 added_node->data);
            }
            previous_node = added_node;
            words_read++;
            word = strtok(NULL, DELIMITERS);
        }
    }
    if (words_read < words_to_read) {
        return EXIT_FAIL;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != MIN_ARGC && argc != MAX_ARGC) {
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    const unsigned int seed = (unsigned int) strtol
            (argv[1], NULL, BASE);
    int num_tweets = (int) strtol(argv[2],
                                  NULL, BASE);
    const char *file_path = argv[3];
    int words_to_read = INITIAL_WORDS_TO_READ;
    if (argc == MAX_ARGC) {
        words_to_read = (int) strtol((argv[4]),
                                     NULL, BASE);
    }
    srand(seed);
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("%s\n", FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (markov_chain == NULL) {
        printf(ALLOCATION_ERROR_MESSAGE);
        fclose(fp);
        return EXIT_FAILURE;
    }
    markov_chain->database = calloc(1,
                                    sizeof(LinkedList));
    if (markov_chain->database == NULL) {
        printf(ALLOCATION_ERROR_MESSAGE);
        free(markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }
    if (fill_database(fp, words_to_read, markov_chain)
    != EXIT_SUCCESS) {
        printf(ALLOCATION_ERROR_MESSAGE);
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }fclose(fp);
    for (int i = 1; i <= num_tweets; i++) {
        MarkovNode *first_node = get_first_random_node(markov_chain);
        printf("Tweet %d: ", i);
        generate_tweet(first_node, MAX_TWEET_LENGTH);
//        printf("\n");
    }
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}

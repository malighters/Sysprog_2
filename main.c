#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct Automata {
    int alphabet_size;
    char *alphabet;
    int states_size;
    int start_state;
    int *final_states;
    int final_states_size;
    int **transitions;
};

struct Automata* read_automata(const FILE* file);
bool accepts_word(struct Automata* automata, const char* word);
bool accepts_all_words_of_length(struct Automata* automata, int length);
void generate_all_words(struct Automata* automata, int length, char* word, int index, bool* accepts);

int main() {
    printf("Lab task: check if automata accepts all words of length length\n");
    printf("Chernenko Yevhenii, TTP-32\n");

    char filename[100];
    printf("Enter file name: \n");
    FILE *file;

    do{
        scanf("%s", filename);
        if (strcmp(filename, "exit") == 0){
            return 1;
        }

        file = fopen(filename, "r");
        if (file == NULL) {
            printf("The file name is incorrect, please try again. If you want quit the program, type 'exit'\n");
        }
        else {
            break;
        }
    }while (true);

    struct Automata* automata = read_automata(file);

    int length;
    printf("Enter the length of the words to check:\n");
    scanf("%d", &length);

    printf("Can the automata accept all words with length %d?\n", length);

    if (accepts_all_words_of_length(automata, length))
        printf("Can\n");
    else
        printf("Can't\n");

    free(automata->alphabet);
    free(automata->final_states);
    for (int i = 0; i < automata->states_size; i++)
        free(automata->transitions[i]);
    free(automata->transitions);
    free(automata);

    return 0;
}

struct Automata* read_automata(const FILE * file) {

    struct Automata* automata = malloc(sizeof(struct Automata));
    if (!automata) {
        printf("Problem with memory accessing. Please try later");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d\n", &automata->alphabet_size);

    automata->alphabet = malloc((automata->alphabet_size + 1) * sizeof(char));
    if (!automata->alphabet) {
        printf("Problem with memory accessing. Please try later");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d\n", &automata->states_size);

    automata->transitions = malloc(automata->states_size * sizeof(int*));
    for (int i = 0; i < automata->states_size; i++) {
        automata->transitions[i] = malloc(256 * sizeof(int));
        if (!automata->transitions[i]) {
            printf("Problem with memory accessing. Please try later");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < 256; j++) automata->transitions[i][j] = -1;
    }

    fscanf(file, "%d\n", &automata->start_state);

    fscanf(file, "%d", &automata->final_states_size);
    automata->final_states = malloc(automata->final_states_size * sizeof(int));
    for (int i = 0; i < automata->final_states_size; i++) fscanf(file, "%d", &automata->final_states[i]);

    int i = 0;
    char symbol;
    int state, next_state;
    while (fscanf(file, "%d %c %d\n", &state, &symbol, &next_state) == 3) {
        automata->transitions[state][(int)symbol] = next_state;
        if (strchr(automata->alphabet, symbol) == NULL) automata->alphabet[i++] = symbol;
    }
    automata->alphabet[i] = '\0';

    fclose(file);

    return automata;
}

bool accepts_word(struct Automata* automata, const char* word) {
    int current_state = automata->start_state;
    for (int i = 0; i < strlen(word); i++) {
        char symbol = word[i];
        if (automata->transitions[current_state][(int)symbol] != -1)
            current_state = automata->transitions[current_state][(int)symbol];
        else
            return false;
    }
    for (int i = 0; i < automata->final_states_size; i++)
        if (automata->final_states[i] == current_state) return true;

    return false;
}

bool accepts_all_words_of_length(struct Automata* automata, int length) {
    bool accepts_all = true;
    char* word = malloc((length + 1) * sizeof(char));
    word[length] = '\0';

    generate_all_words(automata, length, word, 0, &accepts_all);

    free(word);
    return accepts_all;
}

void generate_all_words(struct Automata* automata, int length, char* word, int index, bool* accepts_all) {
    if (!(*accepts_all)) return;
    if (index == length) {
        if (!accepts_word(automata, word)) *accepts_all = false;
        return;
    }

    for (int i = 0; i < strlen(automata->alphabet); i++) {
        word[index] = automata->alphabet[i];
        generate_all_words(automata, length, word, index + 1, accepts_all);
    }
}
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct line {
        char *start;
        char *end;
};

struct document {
        char *text_data;
        int text_data_size;
        struct line *text_lines;
        int number_of_lines;
};

/* GLOBALS */
struct document *current_doc;

void die(char *reason) {
        endwin();
        printf("ERROR: %s", reason);
        exit(1);
}

void rebuild_lines_document(struct document* doc)
{
        int i;

        doc->number_of_lines = 0;

        if (doc->text_data[0] == '\0') {

                if (doc->text_lines != NULL) {
                        free(doc->text_lines);
                }
        } else {
                for(i=0; i < doc->text_data_size; i++) {
                        if (doc->text_data[i] == '\n')
                                doc->number_of_lines++;
                        
                        if (doc->text_data[i] == '\0') {
                                doc->number_of_lines++;
                                break;
                        }
                }



                if (doc->text_lines != NULL)
                        free(doc->text_lines);
                        
                doc->text_lines = calloc(sizeof(struct line), doc->number_of_lines);

                if (doc->text_lines == NULL)
                        die("calloc doc->text_lines");

                doc->text_lines[0].start = doc->text_data;

                for (i=0; i < doc->number_of_lines; i++) {
                        /* if there isnt any new line run to \0 */
                        doc->text_lines[i].end = strchr(doc->text_lines[i].start, '\n') == 0 ? strchr(doc->text_lines[i].start, '\0') : strchr(doc->text_lines[i].start, '\n');
                        
                        if (i+1 < doc->number_of_lines)
                                doc->text_lines[i+1].start = (doc->text_lines[i].end + 1);
                }
        }
}

void init_document()
{
        int INIT_TEXT_SIZE = 4096;
        
        current_doc = malloc(sizeof(struct document));
        current_doc->text_data = calloc(sizeof(char), INIT_TEXT_SIZE);
        current_doc->text_data_size = INIT_TEXT_SIZE;
        current_doc->text_lines = NULL;

        rebuild_lines_document(current_doc);
}

int text_data_length(struct document *doc)
{
        char *the_end = strchr(doc->text_data, '\0');
        int data_length = the_end - doc->text_data;
        return data_length;
}

void expand_text_data(struct document *doc, int increase)
{
        int increase_in_bytes = ((increase / 4096) <= 0 ? 1 : increase / 4096) * 4096;

        char *newmemory = calloc(sizeof(char), doc->text_data_size + increase_in_bytes);
        if (newmemory == NULL)
                die("couldn't expand text_data");

        memmove(newmemory, doc->text_data, doc->text_data_size);

        free(doc->text_data);

        doc->text_data = newmemory;
        doc->text_data_size += increase_in_bytes;
}

void append_to_document(struct document *doc, char *str)
{
        char *the_end = strchr(doc->text_data, '\0');
        /* ^ Get the end of text stored in text_data */
        int len = strlen(str);
        int data_length = the_end - doc->text_data;
        /* Get length of text */

        /* if the new data plus the old data is larger than current buffer size expand the buffer */
        if ((data_length + len) >= doc->text_data_size)
                expand_text_data(doc, data_length + len - doc->text_data_size);
        
        /* Append text to end of text_data */
        strcpy(the_end, str);

        rebuild_lines_document(doc);

}

void insert_into_document_at(struct document *doc, char *str, bool ischar, int line, int column)
{
        int line_length;
        int len = ischar ? 1 : strlen(str);
        int current_text_len = text_data_length(doc); 
        char *place_to_insert = NULL;
        char *place_to_move_to = NULL;

        if ((current_text_len + len) >= doc->text_data_size)
                expand_text_data(doc, (current_text_len + len) - doc->text_data_size);
        
        if (line > doc->number_of_lines)
                die("try to insert on a line that doesnt exist");

        line_length = doc->text_lines[line].end - doc->text_lines[line].start;


        if (column > line_length)
                die("try to insert on a column that doesn't exist");

        place_to_insert = doc->text_lines[line].start + column;
        place_to_move_to = place_to_insert + len;

        /* WRONG LENGTH MOVING. SHOULD BE FROM THERE TO END LENGTH*/
        memmove(place_to_move_to, place_to_insert, len + 1);
        /* TODO: NO ERROR HANDLING */
        
        strncpy(place_to_insert, str, len);

        rebuild_lines_document(doc);

}


int TO_CTRL(int x) { return x & 0x1F; }

int main(int argc, char *argv[])
{
        int ch;
        int cursor = 0;

        init_document();

        append_to_document(current_doc, "test");

        insert_into_document_at(current_doc, "x", true, 0, 2);

        printf("%s\n", current_doc->text_data);

        /*
        initscr();
        raw();
        keypad(stdscr, 1);
        noecho();

        while (1) {
                ch = getch();

                if (ch == TO_CTRL('q'))
                        break;
                else if (!iscntrl(ch))
                        printw("%c", ch);
                refresh();
        }

        endwin();
        */

        return 0;
}
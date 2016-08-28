/**
 * \file bln.c
 */

#include "bln.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LINE_LENGHT 250

/**
 * \brief Read a bln file.
 * \param filename
 * \return the content of the file
 */
bln_data_t *bln_read_file(char *filename){

    bln_data_t *result = NULL;
    FILE *fd;
    int i=0; // line number
    char buf_read[LINE_LENGHT]; /* one file's line */
    bln_data_t *current = NULL; // the next struct to add to result


    fd=fopen(filename, "r");
    if(fd == NULL) {
        fprintf(stderr, "Unable to read file %s\n", filename);
        return NULL;
    }




    /* Lecture ligne par ligne */
    while(fgets(buf_read, LINE_LENGHT, fd)!=NULL){

        char X[20], Y[20]; // to read integers or floats

        //fprintf(stdout, "lecture : %s", buf_read);

        if(i==0 || current == NULL) { /* ligne d'entete */

            char pays[LINE_LENGHT];
            char region[LINE_LENGHT];

            current = malloc(sizeof(bln_data_t));
            if(current == NULL) {
                fprintf(stderr, "Failed allocating memory\n");
                continue;
            }

            sscanf(buf_read, "%d,%d,%[a-zA-Z'\" -],%s", &(current->nbPoints), &(current->closed), pays, region);
            if (current->nbPoints<=0) {
                free(current);
                current=NULL;
                continue; /* i reste à 0 pour la lecture de la prochaine ligne */
            } else {
                current->name = malloc(sizeof(char) * (strlen(pays)+1));
                current->description = malloc(sizeof(char) * (strlen(region)+1));
                current->x = malloc(sizeof(float)*current->nbPoints);
                current->y = malloc(sizeof(float)*current->nbPoints);
                current->next = NULL;

                if(current->name == NULL || current->description == NULL || current->x == NULL || current->y == NULL) {
                    fprintf(stderr, "Failed allocating memory for %s\n", pays);
                    bln_destroy(current);
                    continue;
                }

                strcpy(current->name, pays);
                strcpy(current->description, region);
            }

        } else { /* ligne de donnée : coordonnée d'un point */

            /* lecture d'un enregistrement */
            if(sscanf(buf_read, "%[0-9.-],%[0-9.-]", X, Y)==0) {
                bln_destroy(current);
                bln_destroy(result);
                fclose(fd);
                fprintf(stderr, "Error while reading bln file : %s\n", buf_read);
                return NULL;
            }
            current->x[i-1]=atof(X);
            current->y[i-1]=atof(Y);
        }


        i++;

        // Validate data
        //fprintf(stdout, "%d points OK\n", current->nbPoints);
        if(i>current->nbPoints) {
            if(result == NULL) {
                result = current;
            } else {
                bln_data_t *last = result;
                while (last->next != NULL) last = last->next;
                last->next = current;
            }

            i=0; current=NULL;
        }
    } //reached end of file

    if(current!=NULL) bln_destroy(current);

    return result;
}


/**
 * \brief free the memory used by the bln data.
 */
void bln_destroy(bln_data_t *data){

    bln_data_t *current = data;

    while(current != NULL) {

        //fprintf(stderr, "free du %s pour %d points\n", current->name, current->nbPoints);

        bln_data_t *next;

        if(current->name != NULL) free(current->name);
        if(current->description != NULL) free(current->description);
        if(current->x != NULL) free(current->x);
        if(current->y != NULL) free(current->y);

        next = current->next;
        free(current);
        current=next;
    }
}


/**
 * \brief Write the data in a file.
 */
int bln_write_file(bln_data_t *data, char *filename){

    bln_data_t *current;
    FILE *fd;

    fd=fopen(filename, "w");
    if(fd == NULL) {
        fprintf(stderr, "Unable to create file %s\n", filename);
        return -1;
    }

    current = data;

    while(current!=NULL) {

        int closed = 0;
        int i;
        char *name = "";
        char *desc = "";

        if(current->closed) closed=1;
        if(current->name !=NULL) name = current->name;
        if(current->description != NULL) desc = current->description;

        fprintf(fd, "%d,%d,\"%s\",\"%s\"\n", current->nbPoints, closed, name, desc);

        for(i=1; i<=current->nbPoints; i++) {

            fprintf(fd, "%f,%f\n", current->x[i-1], current->y[i-1]);
        }

        current = current->next;
    }

    fclose(fd);
    return 0;
}




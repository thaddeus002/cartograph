/**
 * \file bln.c
 */

#include "bln.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LINE_LENGHT 250


static void suppress_quotes(char buf[]) {
    int len = strlen(buf);

    if(len<=1) {
        return;
    }

    if((buf[0] == '"' && buf[len-1] == '"') || (buf[0] == '\'' && buf[len-1] == '\'')) {
        int i;
        for(i = 1; i < len-1; i++) {
            buf[i-1] = buf[i];
        }
        buf[len-2] = '\0';
    }
}


/**
 * \brief Read a bln file.
 * \param filename
 * \return the content of the file
 */
bln_data_t *bln_read_file(char *filename, geodetic_system_t type){

    bln_data_t *result = NULL;
    FILE *fd;
    int i=0; // line number
    char buf_read[LINE_LENGHT]; // one file's line
    bln_data_t *current = NULL; // the next struct to add to result


    fd=fopen(filename, "r");
    if(fd == NULL) {
        fprintf(stderr, "Unable to read file %s\n", filename);
        return NULL;
    }


    /* reading line by line */
    while(fgets(buf_read, LINE_LENGHT, fd)!=NULL){

        char X[20], Y[20]; // to read integers or floats

        if(i==0 || current == NULL) { /* header line */

            char pays[LINE_LENGHT];
            char region[LINE_LENGHT];
            int nbRead;

            current = malloc(sizeof(bln_data_t));
            if(current == NULL) {
                fprintf(stderr, "Failed allocating memory\n");
                continue;
            }

            nbRead = sscanf(buf_read, "%d,%d,%[a-zA-Z'\" -],%s", &(current->nbPoints), &(current->closed), pays, region);
            if (nbRead < 1 || current->nbPoints<=0) {
                free(current);
                current=NULL;
                continue; // i remain to 0 for next line reading
            } else {
                if(nbRead < 4) {
                    region[0]='\0';
                }
                if(nbRead < 3) {
                    pays[0] = '\0';
                }
                if(nbRead < 2) {
                    current->closed=0;
                }
                suppress_quotes(pays);
                suppress_quotes(region);
                current->name = malloc(sizeof(char) * (strlen(pays)+1));
                current->description = malloc(sizeof(char) * (strlen(region)+1));
                current->x = malloc(sizeof(float)*current->nbPoints);
                current->y = malloc(sizeof(float)*current->nbPoints);
                current->type = type;
                current->next = NULL;

                if(current->name == NULL || current->description == NULL || current->x == NULL || current->y == NULL) {
                    fprintf(stderr, "Failed allocating memory for %s\n", pays);
                    bln_destroy(current);
                    continue;
                }

                strcpy(current->name, pays);
                strcpy(current->description, region);
            }

        } else { // data line containing a point's coordinates

            // reading data
            if(sscanf(buf_read, "%[0-9.-],%[0-9.-]", X, Y)==0) {
                bln_destroy(current);
                bln_destroy(result);
                fclose(fd);
                fprintf(stderr, "Error while reading bln file : %s\n", buf_read);
                return NULL;
            }
            current->x[i-1]=atof(X);
            current->y[i-1]=atof(Y);
            if(i==1){
                current->xmin=current->x[i-1];
                current->xmax=current->x[i-1];
                current->ymin=current->y[i-1];
                current->ymax=current->y[i-1];
            } else {
                if(current->x[i-1]<current->xmin) current->xmin = current->x[i-1];
                if(current->x[i-1]>current->xmax) current->xmax = current->x[i-1];
                if(current->y[i-1]<current->ymin) current->ymin = current->y[i-1];
                if(current->y[i-1]>current->ymax) current->ymax = current->y[i-1];
            }
        }

        i++;

        // Validate data
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



int bln_find_data_boundaries(bln_data_t *data, bln_boundaries_t *bound){

    bln_data_t *cur;

    bound->xmin=9999.;
    bound->xmax=-9999.;
    bound->ymin=9999.;
    bound->ymax=-9999.;
    bound->result=1;

    if(data == NULL) return -1;

    cur = data;
    while(cur != NULL) {

        if(cur->xmin < bound->xmin) bound->xmin = cur->xmin;
        if(cur->xmax > bound->xmax) bound->xmax = cur->xmax;
        if(cur->ymin < bound->ymin) bound->ymin = cur->ymin;
        if(cur->ymax > bound->ymax) bound->ymax = cur->ymax;

        cur = cur->next;
    }

    bound->result=0;
    return 0;
}


/**
 * \brief free the memory used by the bln data.
 */
void bln_destroy(bln_data_t *data){

    bln_data_t *current = data;

    while(current != NULL) {

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


/**
 * create a new variable to fit a given geodetic system.
 * \param line the data struct to adapt
 * \param type the target geodetic system
 * \return a newly allocated struct or NULL if memory allocation failed
 */
bln_data_t *bln_toGS(bln_data_t *line, geodetic_system_t type) {

    bln_data_t *result = malloc(sizeof(bln_data_t));

    if(result==NULL) {
        return NULL;
    }

    result->nbPoints = line->nbPoints;
    result->closed = line->closed;
    result->name = malloc(sizeof(char)*(strlen(line->name)+1));
    result->description = malloc(sizeof(char)*(strlen(line->description)+1));
    result->x = malloc(sizeof(float)*line->nbPoints);
    result->y = malloc(sizeof(float)*line->nbPoints);
    result->type = type;
    result->next = NULL;

    if(result->name==NULL || result->description==NULL || result->x == NULL || result->y ==  NULL) {
        bln_destroy(result);
        return NULL;
    }

    strcpy(result->name, line->name);
    strcpy(result->description, line->description);

    int i;
    
    for(i = 0; i < line->nbPoints; i++) {

        coord_plane ori = { .X=line->x[i], .Y=line->y[i] };
        coord_plane res = coordinates_convert(ori, line->type, type);

        result->x[i] = res.X;
        result->y[i] = res.Y;

        if(i==0) {
            result->xmin = res.X;
            result->xmax = res.X;
            result->ymin = res.Y;
            result->ymax = res.Y;
        } else {
            if(res.X < result->xmin) {
                result->xmin=res.X;
            } else if(res.X > result->xmax) {
                result->xmax=res.X;
            }
            if(res.Y < result->ymin) {
                result->ymin=res.Y;
            } else if(res.Y > result->ymax) {
                result->ymax=res.Y;
            }
        }
    }

    return result;
}

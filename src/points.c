/**
 * \file points.c
 * Functions for reading points files in CSV format.
 * The columns are : lambx, lamby, prefecture, departement number.
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "points.h"

static poste_t *lit_enregistrement_csv(FILE *fd, int *err) {
    char buf_read[300]; // reading buffer
    int i,j; // number of read data, counters
    char com[250]; // city's name
    int MAJ; // requires upercase (treatment of name character by character)
    poste_t *read_point;

    if(fgets(buf_read, 255, fd)== NULL) {
        *err = 1;
        return(NULL);
    }

    read_point = malloc(sizeof(poste_t));

    i=sscanf(buf_read, "%f,%f,%[a-zA-Z- \"\'],%d\n",&(read_point->X),&(read_point->Y), com, &(read_point->departement));

    if(i<3){
        fprintf(stderr,"\nincorrect line :\n%s\n\n", buf_read);
        *err=2;
        free(read_point);
        return(NULL);
    }

    //traitement du nom de la commune
    //pas de guillemets et majuscules seulement en début de noms propres
    MAJ=1; //majuscule requise pour le premier caractère
    j=0;
    for(i=0;i+j<=strlen(com)-1;i++){
        if(com[i+j]=='"') { j++; i--; continue; }  // suppression des guillemets
        read_point->commune[i]=com[i+j];
        if(MAJ){
            if((read_point->commune[i]>='a')&&(read_point->commune[i]<='z')) read_point->commune[i]+='A'-'a';
            MAJ=0;
        }
        else {
            if((read_point->commune[i]>='A')&&(read_point->commune[i]<='Z')) read_point->commune[i]+='a'-'A';
        }
        if((read_point->commune[i]=='-')||(read_point->commune[i]=='\'')||(read_point->commune[i]==' ')) MAJ=1;
    }
    read_point->commune[i]='\0';

    read_point->next = NULL;

    *err = 0;
    return(read_point);
}



poste_t *read_points_file(char *csvDataFile){
    FILE *fd;
    poste_t *enregistrement;
    poste_t *list, *last;
    int err;

    // open file
    fd=fopen(csvDataFile, "r");
    if(!fd) {
        fprintf(stderr, "Could not open file %s\n", csvDataFile);
        return(NULL);
    }

    // header line currently not managed
    char header_line[1000];
    fscanf(fd, "%s\n", header_line);

    // reading the data
    list = NULL;
    enregistrement=lit_enregistrement_csv(fd, &err);
    while(!err) {

        if(list==NULL) {
            list = enregistrement;
        } else {
            last->next = enregistrement;
        }

        last=enregistrement;

        enregistrement = lit_enregistrement_csv(fd, &err);
    }

    // close file
    fclose(fd);
    return(list);
}



int find_data_boundaries(poste_t *list, data_boundaries_t *bound){

    poste_t *cur;
    int result = 1;

    bound->xmin=9999.;
    bound->xmax=-9999.;
    bound->ymin=9999.;
    bound->ymax=-9999.;

    cur = list;
    while(cur != NULL) {

        if(cur->X < bound->xmin) bound->xmin = cur->X;
        if(cur->X > bound->xmax) bound->xmax = cur->X;
        if(cur->Y < bound->ymin) bound->ymin = cur->Y;
        if(cur->Y > bound->ymax) bound->ymax = cur->Y;

        cur = cur->next;
    }

    if(bound->xmin != 9999.) {
        result = 0;
    }

    return result;
}


void destroy_points_data(poste_t *points) {
    poste_t *current = points;
    while(current != NULL){
        poste_t *next =current->next;  
        free(current);
        current = next;
    }
}

/* outils.h */

#ifndef __OUTILS_H__
#define __OUTILS_H__ 1


/* formes pour le pointage */
typedef enum
{
	CARRE=0,
	TRIANGLE,
	ROND
} forme_t;


/* transforme les coordonn�es planes (Lambert, lon-lat, ...) en coordonn�es fenetre ou grille */
/* fcts utilis�es dans toutes les op�rations de trac� */
//int transforme_xLambert(float xLamb, int w, float x1, float x2);
//int transforme_yLambert(float yLamb, int h, float y1, float y2);
/* A remplacer par ...*/
int transforme_x(float x, int w, float x1, float x2);
int transforme_y(float y, int h, float y1, float y2);

/* transformations inverses */
float transforme_i(int i, int w, float x1, float x2);
float transforme_j(int j, int h, float y1, float y2);



#endif

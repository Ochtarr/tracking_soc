/*---------------------- 
	Librairies 
------------------------*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "def.h"
#include "nrio.h"
#include "nrarith.h"
#include "nralloc.h"

/*---------------------- 
    Variables globales 
------------------------*/

#define SEUIL 80
#define NB_IMAGE_TOTAL 50
#define NB_IMAGE_LUES_SIMULT 3

/*---------------------- 
	Structures 
------------------------*/

typedef struct s_mask
{
	int pixels[3][3];
} mask;

/*---------------------- 
	Prototypes 
------------------------*/
int andLogic(mask element);
int orLogic(mask element);
mask fillMask(byte** I, int i, int j);
void dilatation(int l, byte** I, byte** R, long nrl, long nrh, long ncl, long nch);
void erosion(int l, byte** I, byte** R, long nrl, long nrh, long ncl, long nch);
void operation(int op, int l, byte** I, byte** R, long nrl, long nrh, long ncl, long nch);
void etiquettage(byte** binary, rgb8 ** image, char nomImageResultat[255], long nrh, long nrl, long nch, long ncl);

/**
morphologie mathématique, Erosion/Dilatation / Ouverture/Fermeture
**/
void question3();

/**
Détection de mouvement par différence d’images consécutives
**/
void question4_fomd();
void question4_lbox();

/**
Extraction d’une image de Référence
**/
void question5_fomd();
void question5_lbox();

/**
Détection de mouvement avec image de référence
**/
void question6_fomd();
void question6_lbox();

/**
Etiquettage et Caractérisation des formes binaires
**/
void question7();

float** getPixels(rgb8** Image, long nrh, long nch);
float getMoyenne(float** tabPixels, long nrh, long nch);
int getMedian(int* tabPixels, long nch);
int compare_int(const void* a, const void* b);

int main(void)
{

	printf("Debut programme\n");

	/* Question 3 : Remplissage du carré */
	question3();

	/* Question 4 : Détection de mouvement par différence d’images consécutives */
	//question4_fomd();
	printf("Checkpoint fin Q4 fomd\n");	
	//question4_lbox();
	printf("Checkpoint fin Q4 lbox\n");	

	/* Question 5  : Extraction d’une image de Référence */  
	question5_fomd();
	printf("Checkpoint fin Q5 fomd\n");
	question5_lbox();
	printf("Checkpoint fin Q5 lbox\n");

	/* Question 6 : */
	//question6_fomd();
	printf("Checkpoint fin Q6 fomd\n");
	//question6_lbox();
	printf("Checkpoint fin Q6 lbox\n");
	
	/* Question 7 */
	question7();

	printf("Fin du programme\n");
	return 1;
}

mask fillMask(byte** I, int i, int j)
{
	mask element;
	int a,b,c,d;

	for(c=0;c<=2;c++) //ligne
	{
		for(d=0;d<=2;d++) //colonne
		{
			element.pixels[c][d] = I[i + (c-1)][j + (d-1)];
			//printf("rempl. mask : pixel[%d][%d] = I[%d][%d]\n", c,d,i-1+c,j-1+d);
		}
	}
	return element;
}

int andLogic(mask element){
	int i,j;
	int result = element.pixels[1][1];

	for(i=0 ; i<3 ; i++)
	{
		for(j=0 ; j<3 ; j++)
		{
			if(element.pixels[i][j] == 255)
				element.pixels[i][j] = 1;

			if(j!=1 || i!=1)
				result = (result & element.pixels[i][j]);
		}
	}

	return result;
}

int orLogic(mask element){
	int i,j;
	int result = element.pixels[1][1];

	for(i=0 ; i<3 ; i++)
	{
		for(j=0 ; j<3 ; j++)
		{
			if(element.pixels[i][j] == 255)
				element.pixels[i][j] = 1;

			if(j!=1 || i!=1)
				if (element.pixels[i][j] == 1) return 1;
		}
	}

	return 0;
}

/**
morphologie mathématique, Erosion/Dilatation / Ouverture/Fermeture
**/
void question3()
{
	byte **I;
	byte **R;
	
	long nrh,nrl,nch,ncl;

	I = LoadPGM_bmatrix("Images/Test/carreTrou.pgm",&nrl,&nrh,&ncl,&nch);
	R = bmatrix(nrl,nrh,ncl,nch);
	
	dilatation(20, I, R, nrl, nrh, ncl, nch);
	erosion(20, I, R, nrl, nrh, ncl, nch);
	
	SavePGM_bmatrix(R,nrl,nrh,ncl,nch,"Images/Test/carreTrou1.pgm");
	free_bmatrix(I,nrl,nrh,ncl,nch);

}

void dilatation(int l, byte** I, byte** R, long nrl, long nrh, long ncl, long nch) {
	operation(1,l, I, R, nrl, nrh, ncl, nch);
}

void erosion(int l, byte** I, byte** R, long nrl, long nrh, long ncl, long nch) {
	operation(0,l, I, R, nrl, nrh, ncl, nch);
}

void operation(int op, int l, byte** I, byte** R, long nrl, long nrh, long ncl, long nch) {
	int i,j,k;
	mask element;

	for (k=0; k<l; k++)
	{
		for(i=1;i<nrh;i++)
		{
			for(j=1;j<=nch;j++)
			{
				element = fillMask(I, i, j);
				int logic = (op == 1) ? orLogic(element) : andLogic(element);

				if(logic == 1)
					R[i][j] = 255;
				else
					R[i][j] = 0;
			}
		}

		//for(i=1;i<nrh;i++) //COPY
		//	for(j=1;j<=nch;j++)
		//		I[i][j] = R[i][j];
	}
}

void question4_fomd(){
	int cmp;

	for( cmp=1; cmp<=(NB_IMAGE_TOTAL-1); cmp++)
	{
		//Initialisation des fichiers images à lire
		rgb8 **Image1, **Image2;
		char nomImage1[255], nomImage2[255];
		long nrh,nrl,nch,ncl;

		sprintf(nomImage1, "Images/Sequences/Fomd/ppm/fomd%03d.ppm", cmp);
		sprintf(nomImage2, "Images/Sequences/Fomd/ppm/fomd%03d.ppm", cmp+1);

		char nomImageResultat[255];
		char nomImageFinale[255];
		sprintf(nomImageResultat, "Images/Resultats/Fomd/fomd%d-%d.pgm", cmp, cmp+1);
		sprintf(nomImageFinale, "Images/Resultats/Fomd/Optimisees/fomd%d-%d.pgm", cmp, cmp+1);

		//Lecture Image
		Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);
		Image2 = LoadPPM_rgb8matrix(nomImage2, &nrl,&nrh,&ncl,&nch);

		//Allocation Matrice
		byte **ImageResultat; 
		ImageResultat = bmatrix(nrl,nrh,ncl,nch);
		byte **ImageFinale;
		ImageFinale = bmatrix(nrl,nrh,ncl,nch);

		//Soustraction
		int lig, col;
		for(lig=0; lig<=nrh; lig++)
		{
			for(col=0; col<=nch+1; col++)
			{
				//Récupération des valeurs des pixels
				int moy1 = (Image1[lig][col].r +  Image1[lig][col].g + Image1[lig][col].b) / 3;
				int moy2 = (Image2[lig][col].r +  Image2[lig][col].g + Image2[lig][col].b) / 3;
				//Binarisation	
				if(abs(moy2 - moy1) > SEUIL)
					ImageResultat[lig][col] = 255;	
				else
					ImageResultat[lig][col] = 0;
			}
		}


		/*-----------------------------
			Optimisation
		-----------------------------*/
		//fermeture
		dilatation(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		erosion(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//ouverture
		erosion(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		dilatation(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//étiquettage des régions
		//etiquettage(ImageFinale, Image2, nomImageResultat, nrh, nrl, nch, ncl);

		//Ecriture de l'image résultat (non optimisée) et de l'image finale (optimisée)
		SavePGM_bmatrix(ImageResultat,nrl,nrh,ncl,nch,nomImageResultat);
		SavePGM_bmatrix(ImageFinale,nrl,nrh,ncl,nch,nomImageFinale);
		
		//Libération des ressources
		free_rgb8matrix(Image1, nrl,nrh,ncl,nch);
		free_rgb8matrix(Image2, nrl,nrh,ncl,nch);
		free_bmatrix(ImageResultat, nrl,nrh,ncl,nch);
		free_bmatrix(ImageFinale, nrl,nrh,ncl,nch);
	}
}

void question4_lbox(){
	int cmp;

	for( cmp=1; cmp<=(NB_IMAGE_TOTAL-1); cmp++)
	{
		//Initialisation des fichiers images à lire
		rgb8 **Image1, **Image2;
		char nomImage1[255], nomImage2[255];
		long nrh,nrl,nch,ncl;

		sprintf(nomImage1, "Images/Sequences/Lbox/ppm/lbox%03d.ppm", cmp);
		sprintf(nomImage2, "Images/Sequences/Lbox/ppm/lbox%03d.ppm", cmp+1);

		char nomImageResultat[255];
		char nomImageFinale[255];
		sprintf(nomImageResultat, "Images/Resultats/Lbox/lbox%d-%d.pgm", cmp, cmp+1);
		sprintf(nomImageFinale, "Images/Resultats/Lbox/Optimisees/lbox%d-%d.pgm", cmp, cmp+1);

		//Lecture Image
		Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);
		Image2 = LoadPPM_rgb8matrix(nomImage2, &nrl,&nrh,&ncl,&nch);

		//Allocation Matrice
		byte **ImageResultat; 
		ImageResultat = bmatrix(nrl,nrh,ncl,nch);
		byte **ImageFinale;
		ImageFinale = bmatrix(nrl,nrh,ncl,nch);

		//Soustraction
		int lig, col;
		for(lig=0; lig<=nrh; lig++)
		{
			for(col=0; col<=nch+1; col++)
			{
				//Récupération des valeurs des pixels
				int moy1 = (Image1[lig][col].r +  Image1[lig][col].g + Image1[lig][col].b) / 3;
				int moy2 = (Image2[lig][col].r +  Image2[lig][col].g + Image2[lig][col].b) / 3;
				//Binarisation	
				if(abs(moy2 - moy1) > SEUIL)
					ImageResultat[lig][col] = 255;	
				else
					ImageResultat[lig][col] = 0;
			}
		}


		/*-----------------------------
			Optimisation
		-----------------------------*/
		//fermeture
		dilatation(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		erosion(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//ouverture
		erosion(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		dilatation(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//étiquettage des régions
		//etiquettage(ImageFinale, Image2, nomImageResultat, nrh, nrl, nch, ncl);

		//Ecriture de l'image résultat (non optimisée) et de l'image finale (optimisée)
		SavePGM_bmatrix(ImageResultat,nrl,nrh,ncl,nch,nomImageResultat);
		SavePGM_bmatrix(ImageFinale,nrl,nrh,ncl,nch,nomImageFinale);
		
		//Libération des ressources
		free_rgb8matrix(Image1, nrl,nrh,ncl,nch);
		free_rgb8matrix(Image2, nrl,nrh,ncl,nch);
		free_bmatrix(ImageResultat, nrl,nrh,ncl,nch);
		free_bmatrix(ImageFinale, nrl,nrh,ncl,nch);
	}
}


void question5_fomd(){
	int cmp, lig, col;
	long nrh,nrl,nch,ncl;

	//Initialisation des tailles des images
	rgb8 **Image1;
	char nomImage1[255];
	sprintf(nomImage1, "Images/Sequences/Fomd/ppm/fomd001.ppm");
	Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);
	free_rgb8matrix(Image1, nrl,nrh,ncl,nch);

	//Allocation Matrice
	int DonneesMedian[NB_IMAGE_TOTAL];
	float DonneesMoyenne[NB_IMAGE_TOTAL];

	//Allocation des images résultats
	byte **ImageResultatMoyenne, **ImageResultatMedian;
	char nomImageResultatMedian[255], nomImageResultatMoyenne[255];
	sprintf(nomImageResultatMedian, "Images/ResultatsMedian/fomdMedian.pgm");
	sprintf(nomImageResultatMoyenne, "Images/ResultatsMoyenne/fomdMoyenne.pgm");
	ImageResultatMoyenne = bmatrix(nrl,nrh,ncl,nch);
	ImageResultatMedian = bmatrix(nrl,nrh,ncl,nch);

	for(lig=0; lig<=nrh; lig++)
	{
		for(col=0; col<=nch+1; col++)
		{
			float sommePourMoyenne;
			int *vecteurValueMasque = (int*) malloc( NB_IMAGE_TOTAL * sizeof(int)) ;
			for( cmp=1; cmp<=NB_IMAGE_TOTAL; cmp++)
			{
				sommePourMoyenne = 0;
				//Init nom fichier image source
				sprintf(nomImage1, "Images/Sequences/Fomd/ppm/fomd%03d.ppm", cmp);
				//Lecture Image source
				Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);

				//Alimentation des données
				DonneesMoyenne[cmp-1] = (Image1[lig][col].r + Image1[lig][col].g + Image1[lig][col].b) / 3;
				DonneesMedian[cmp-1] = (Image1[lig][col].r + Image1[lig][col].g + Image1[lig][col].b) / 3;
	
				//Libération des ressources
				free_rgb8matrix(Image1, nrl,nrh,ncl,nch);

				//Moyenne
				sommePourMoyenne += DonneesMoyenne[cmp-1];
				//Median
				vecteurValueMasque[cmp-1] = DonneesMedian[cmp-1];
			}
			ImageResultatMoyenne[lig][col] = (float) sommePourMoyenne / NB_IMAGE_TOTAL;
			ImageResultatMedian[lig][col] = getMedian(DonneesMedian,NB_IMAGE_TOTAL);
			free(vecteurValueMasque);		
		}
	}

	//Sauvegarde des images
	SavePGM_bmatrix(ImageResultatMoyenne, nrl,nrh,ncl,nch, nomImageResultatMoyenne);
	SavePGM_bmatrix(ImageResultatMedian, nrl,nrh,ncl,nch, nomImageResultatMedian);

	//Libération des ressources
	free_bmatrix(ImageResultatMoyenne, nrl,nrh,ncl,nch);
	free_bmatrix(ImageResultatMedian, nrl,nrh,ncl,nch);
}

void question5_lbox(){
	int cmp, lig, col;
	long nrh,nrl,nch,ncl;

	//Initialisation des tailles des images
	rgb8 **Image1;
	char nomImage1[255];
	sprintf(nomImage1, "Images/Sequences/Lbox/ppm/lbox001.ppm");
	Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);
	free_rgb8matrix(Image1, nrl,nrh,ncl,nch);

	//Allocation Matrice
	int DonneesMedian[NB_IMAGE_TOTAL];
	float DonneesMoyenne[NB_IMAGE_TOTAL];

	//Allocation des images résultats
	byte **ImageResultatMoyenne, **ImageResultatMedian;
	char nomImageResultatMedian[255], nomImageResultatMoyenne[255];
	sprintf(nomImageResultatMedian, "Images/ResultatsMedian/lboxMedian.pgm");
	sprintf(nomImageResultatMoyenne, "Images/ResultatsMoyenne/lboxMoyenne.pgm");
	ImageResultatMoyenne = bmatrix(nrl,nrh,ncl,nch);
	ImageResultatMedian = bmatrix(nrl,nrh,ncl,nch);

	for(lig=0; lig<=nrh; lig++)
	{
		for(col=0; col<=nch+1; col++)
		{
			float sommePourMoyenne;
			int *vecteurValueMasque = (int*) malloc( NB_IMAGE_TOTAL * sizeof(int)) ;
			for( cmp=1; cmp<=NB_IMAGE_TOTAL; cmp++)
			{
				sommePourMoyenne = 0;
				//Init nom fichier image source
				sprintf(nomImage1, "Images/Sequences/Lbox/ppm/lbox%03d.ppm", cmp);
				//Lecture Image source
				Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);

				//Alimentation des données
				DonneesMoyenne[cmp-1] = (Image1[lig][col].r + Image1[lig][col].g + Image1[lig][col].b) / 3;
				DonneesMedian[cmp-1] = (Image1[lig][col].r + Image1[lig][col].g + Image1[lig][col].b) / 3;
	
				//Libération des ressources
				free_rgb8matrix(Image1, nrl,nrh,ncl,nch);

				//Moyenne
				sommePourMoyenne += DonneesMoyenne[cmp-1];
				//Median
				vecteurValueMasque[cmp-1] = DonneesMedian[cmp-1];
			}
			ImageResultatMoyenne[lig][col] = (float) sommePourMoyenne / NB_IMAGE_TOTAL;
			ImageResultatMedian[lig][col] = getMedian(DonneesMedian,NB_IMAGE_TOTAL);
			free(vecteurValueMasque);		
		}
	}

	//Sauvegarde des images
	SavePGM_bmatrix(ImageResultatMoyenne, nrl,nrh,ncl,nch, nomImageResultatMoyenne);
	SavePGM_bmatrix(ImageResultatMedian, nrl,nrh,ncl,nch, nomImageResultatMedian);

	//Libération des ressources
	free_bmatrix(ImageResultatMoyenne, nrl,nrh,ncl,nch);
	free_bmatrix(ImageResultatMedian, nrl,nrh,ncl,nch);
}

void question6_fomd()
{
	int cmp;

	for( cmp=1; cmp<=(NB_IMAGE_TOTAL-1); cmp++)
	{
		//Initialisation des fichiers images à lire
		byte **Image1;
		rgb8 **Image2;
		char nomImage1[255], nomImage2[255];
		long nrh,nrl,nch,ncl;

		sprintf(nomImage1, "Images/ResultatsMedian/fomdMedian.pgm");
		sprintf(nomImage2, "Images/Sequences/Fomd/ppm/fomd%03d.ppm", cmp);

		char nomImageResultat[255];
		char nomImageFinale[255];
		sprintf(nomImageResultat, "Images/ResultatsRef/Fomd/fomd%d-%d.pgm", cmp, cmp+1);
		sprintf(nomImageFinale, "Images/ResultatsRef/Fomd/Optimisees/fomd%d-%d.pgm", cmp, cmp+1);

		//Lecture Image
		Image1 = LoadPGM_bmatrix(nomImage1, &nrl,&nrh,&ncl,&nch);
		Image2 = LoadPPM_rgb8matrix(nomImage2, &nrl,&nrh,&ncl,&nch);

		//Allocation Matrice
		byte **ImageResultat; 
		ImageResultat = bmatrix(nrl,nrh,ncl,nch);
		byte **ImageFinale;
		ImageFinale = bmatrix(nrl,nrh,ncl,nch);

		//Soustraction
		int lig, col;
		for(lig=0; lig<=nrh; lig++)
		{
			for(col=0; col<=nch+1; col++)
			{
				//Récupération des valeurs des pixels
				int moy1 = Image1[lig][col];
				int moy2 = (Image2[lig][col].r +  Image2[lig][col].g + Image2[lig][col].b) / 3;
				//Binarisation	
				if(abs(moy2 - moy1) > SEUIL)
					ImageResultat[lig][col] = 255;	
				else
					ImageResultat[lig][col] = 0;
			}
		}


		/*-----------------------------
			Optimisation
		-----------------------------*/
		//fermeture
		dilatation(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		erosion(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//ouverture
		erosion(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		dilatation(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//étiquettage des régions
		//etiquettage(ImageFinale, Image2, nomImageResultat, nrh, nrl, nch, ncl);

		//Ecriture de l'image résultat (non optimisée) et de l'image finale (optimisée)
		SavePGM_bmatrix(ImageResultat,nrl,nrh,ncl,nch,nomImageResultat);
		SavePGM_bmatrix(ImageFinale,nrl,nrh,ncl,nch,nomImageFinale);
		
		//Libération des ressources
		free_bmatrix(Image1, nrl,nrh,ncl,nch);
		free_rgb8matrix(Image2, nrl,nrh,ncl,nch);
		free_bmatrix(ImageResultat, nrl,nrh,ncl,nch);
		free_bmatrix(ImageFinale, nrl,nrh,ncl,nch);
	}
}

void question6_lbox()
{
	int cmp;

	for( cmp=1; cmp<=(NB_IMAGE_TOTAL-1); cmp++)
	{
		//Initialisation des fichiers images à lire
		byte **Image1;
		rgb8 **Image2;
		char nomImage1[255], nomImage2[255];
		long nrh,nrl,nch,ncl;

		sprintf(nomImage1, "Images/ResultatsMedian/lboxMedian.pgm");
		sprintf(nomImage2, "Images/Sequences/Lbox/ppm/lbox%03d.ppm", cmp);

		char nomImageResultat[255];
		char nomImageFinale[255];
		sprintf(nomImageResultat, "Images/ResultatsRef/Lbox/lbox%d-%d.pgm", cmp, cmp+1);
		sprintf(nomImageFinale, "Images/ResultatsRef/Lbox/Optimisees/lbox%d-%d.pgm", cmp, cmp+1);

		//Lecture Image
		Image1 = LoadPGM_bmatrix(nomImage1, &nrl,&nrh,&ncl,&nch);
		Image2 = LoadPPM_rgb8matrix(nomImage2, &nrl,&nrh,&ncl,&nch);

		//Allocation Matrice
		byte **ImageResultat; 
		ImageResultat = bmatrix(nrl,nrh,ncl,nch);
		byte **ImageFinale;
		ImageFinale = bmatrix(nrl,nrh,ncl,nch);

		//Soustraction
		int lig, col;
		for(lig=0; lig<=nrh; lig++)
		{
			for(col=0; col<=nch+1; col++)
			{
				//Récupération des valeurs des pixels
				int moy1 = Image1[lig][col];
				int moy2 = (Image2[lig][col].r +  Image2[lig][col].g + Image2[lig][col].b) / 3;
				//Binarisation	
				if(abs(moy2 - moy1) > SEUIL)
					ImageResultat[lig][col] = 255;	
				else
					ImageResultat[lig][col] = 0;
			}
		}


		/*-----------------------------
			Optimisation
		-----------------------------*/
		//fermeture
		dilatation(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		erosion(12, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//ouverture
		erosion(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);
		dilatation(2, ImageResultat, ImageFinale, nrl, nrh, ncl, nch);

		//étiquettage des régions
		//etiquettage(ImageFinale, Image2, nomImageResultat, nrh, nrl, nch, ncl);

		//Ecriture de l'image résultat (non optimisée) et de l'image finale (optimisée)
		SavePGM_bmatrix(ImageResultat,nrl,nrh,ncl,nch,nomImageResultat);
		SavePGM_bmatrix(ImageFinale,nrl,nrh,ncl,nch,nomImageFinale);
		
		//Libération des ressources
		free_bmatrix(Image1, nrl,nrh,ncl,nch);
		free_rgb8matrix(Image2, nrl,nrh,ncl,nch);
		free_bmatrix(ImageResultat, nrl,nrh,ncl,nch);
		free_bmatrix(ImageFinale, nrl,nrh,ncl,nch);
	}
}

void question7(){
	int cmp;
	for( cmp=1; cmp<=(NB_IMAGE_TOTAL-1); cmp++)
	{
		//Initialisation des fichiers images à lire
		rgb8 **Image1;
		char nomImage1[255];
		long nrh,nrl,nch,ncl;

		sprintf(nomImage1, "Images/Sequences/Fomd/ppm/fomd%03d.ppm", cmp, cmp+1);

		char nomImageResultat[255];
		sprintf(nomImageResultat, "Images/ResultatsEtiquettage/Fomd/fomd%03d.pgm", cmp);
		
		//Lecture Image
		printf("Checkpoint before chargement image source\n");
		Image1 = LoadPPM_rgb8matrix(nomImage1, &nrl,&nrh,&ncl,&nch);

		//Allocation Matrice
		byte **ImageResultat; 
		ImageResultat = bmatrix(nrl,nrh,ncl,nch);

		//étiquettage des régions
		printf("Checkpoint before etiquettage\n");
		etiquettage(ImageResultat, Image1, nomImageResultat, nrh, nrl, nch, ncl);

		//Ecriture de l'image résultat
		SavePGM_bmatrix(ImageResultat,nrl,nrh,ncl,nch,nomImageResultat);
		
		//Libération des ressources
		free_rgb8matrix(Image1, nrl,nrh,ncl,nch);
		free_bmatrix(ImageResultat, nrl,nrh,ncl,nch);
	}
}



float** getPixels(rgb8** Image, long nrh, long nch)
{
	float** tabPixels = malloc(sizeof(float*) * nrh);
	int lig, col;
	for(lig=0; lig<nrh; lig++)
	{
		tabPixels[lig] = malloc(sizeof(float) * nch);
		for(col=0; col<nch; col++)
		{
			tabPixels[lig][col] = (float) (Image[lig][col].r + Image[lig][col].g + Image[lig][col].b) /3;
		}
	}
	return tabPixels;
}

float getMoyenne(float** tabPixels, long nrh, long nch)
{
	float somme = 0.0;
	int lig, col;
	for(lig=0; lig<nrh; lig++)
	{
		for(col=0; col<nch; col++)
		{
			somme = tabPixels[lig][col];
		}
	}
	return somme / nrh*nch;
}

int getMedian(int* tabPixels, long nch)
{
	int* copieTabPixels = (int*) malloc(nch*sizeof(int));
	int col;
		for(col=0; col<nch; col++)
		{
			copieTabPixels[col] = tabPixels[col];
		}

	for(col=0; col<nch; col++)
	{
		qsort(copieTabPixels, nch, sizeof(int), compare_int);
	}

	if(nch%2 == 0)
		return copieTabPixels[nch/2];
	if(nch%2 == 1)
		return copieTabPixels[nch/2+1];
}

int compare_int(const void* a, const void* b)
{
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
 
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

void etiquettage(byte** binary, rgb8 ** image, char nomImageResultat[255], long nrh, long nrl, long nch, long ncl)
{
	int** etiquette;
	int i,j,k;
	int maxEtiquette = 0;
	int nbEtiquette = 0;
	int nbRegions = 0;
	char nameF[255];
	
	etiquette = imatrix(nrl, nrh, ncl, nch);

	i = 0;
	while (nomImageResultat[i] != '.')
	{
		nameF[i] = nomImageResultat[i];
		i++;
	}
	nameF[i] = '.'; nameF[i+1] = 't'; nameF[i+2] = 'x'; nameF[i+3] = 't';

	for (i = nrl; i < nrh; i++)
	{
		for (j = ncl; j < nch; j++)
		{
			byte attC = binary[i][j];
			byte attA, attB;
			float eA, eB;

			if (i > nrl)
			{
				attA = binary[i - 1][j];
				eA = etiquette[i - 1][j];
			}
			else
			{
				attA = 0;
				eA = 0;
			}

			if (j > ncl)
			{
				attB = binary[i][j - 1];
				eB = etiquette[i][j - 1];
			}
			else
			{
				attB = 0;
				eB = 0;
			}

			if (attC == attA)
			{
				if (attC != attB)
					etiquette[i][j] = eA;
				else
				{
					etiquette[i][j] = eB;
					if (eA != eB)
					{
						int k, l;

						for (k = nrl; k <= i; k++)
						{
							for (l = ncl; l < nch; l++)
							{
								if (etiquette[k][l] == eA)
									etiquette[k][l] = eB;
							}
						}
					}
				}
			}
			else
			{
				if (attC == attB)
					etiquette[i][j] = eB;
				else
				{
					etiquette[i][j] = maxEtiquette;
					maxEtiquette++;
				}
			}
		}
	}

	FILE* fichier = NULL;
    fichier = fopen(nameF, "w+");
	
	for (k=0; k<maxEtiquette; k++)
	{
		int nbPixel = 0;
		float cX = 0;
		float cY = 0;
		int nX = 0;
		int nY = 0;

		for (i = nrl; i < nrh; i++)
		{
			for (j = ncl; j < nch; j++)
			{
				if (etiquette[i][j] == k)
				{
					cX += j;
					cY += i;
					nX++;
					nY++;
					nbPixel++;
				}
			}
		}

		float resEcartX = 0;
		float resEcartY = 0;
		float resEcartXY = 0;
		float direction = 0;

		float moyGris = 0;
		float moyR = 0;
		float moyV = 0;
		float moyB = 0;
		
		int countGris[255];
		int gris;
		int l;

		for (l=0; l<255; l++)
			countGris[l] = 0;

		for (i = nrl; i < nrh; i++)
		{
			for (j = ncl; j < nch; j++)
			{
				if (etiquette[i][j] == k)
				{
					resEcartX += (j - (float)(cX/nX))*(j - (float)(cX/nX));
					resEcartY += (i - (float)(cY/nY))*(i - (float)(cY/nY));
					resEcartXY += (j - (float)(cX/nX))*(i - (float)(cY/nY));

					moyGris += (image[i][j].r +  image[i][j].g + image[i][j].b);
					moyR += image[i][j].r;
					moyV += image[i][j].g;
					moyB += image[i][j].b;

					gris = moyGris/(3*nX);
					countGris[gris]++;
				}
			}
		}

		direction = 0.5 * atan((2*resEcartXY/nY)/((resEcartX/nX)-(resEcartY/nY)));

		if (nbPixel != 0)
		{
			fprintf(fichier, "\nNombre de pixel dans la région[%d] : %d\n", k, nbPixel);
			fprintf(fichier, "Coordonnéé centre gravité : %f , %f\n", (float)(cX/nX), (float)(cY/nY));
			
			fprintf(fichier, "Ecart type en X : %f\n", resEcartX/nX);
			fprintf(fichier, "Ecart type en Y : %f\n", resEcartY/nY);
			fprintf(fichier, "Ecart type en XY : %f\n", resEcartXY/nY);

			fprintf(fichier, "Direction principale : %f\n", direction*180/3.14);

			fprintf(fichier, "Moyenne de gris : %f\n", moyGris/(3*nY));
			fprintf(fichier, "Moyenne de rouge : %f\n", moyR/(3*nY));
			fprintf(fichier, "Moyenne de vert : %f\n", moyV/(3*nY));
			fprintf(fichier, "Moyenne de bleu : %f\n", moyB/(3*nY));

			//Histogramme pour chaque région
			fprintf(fichier, "Histogramme : \n");

			for (l=0; l<255; l++)
				fprintf(fichier, "%d ", countGris[l]);

			fprintf(fichier, "\n\n");

			nbRegions++;
		}
	}

	fprintf(fichier, "\nAu total, il y a %d régions.\n\n", nbRegions);

	for (i = nrl; i < nrh; i++)
	{
		for (j = ncl; j < nch; j++)
		{
			if (etiquette[i][j] != 0) //background
			{
				fprintf(fichier, "E : %d\n", etiquette[i][j]);
			}
		}
	}

	fclose(fichier);
}

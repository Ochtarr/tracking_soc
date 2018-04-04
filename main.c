/*---------------------- 
	Librairies 
------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "def.h"
#include "nrio.h"
#include "nrarith.h"
#include "nralloc.h"
#include "tools.h"

/*---------------------- 
    Variables globales 
------------------------*/

#define NB_IMAGE_TOTAL 50
#define NB_IMAGE_LUES_SIMULT 3

/*---------------------- 
	Main loop 
------------------------*/
int main(){

	/* -------------------------------
	Step 0 : Unit test 
	------------------------------- */
	unitTest_colorToNdg();

	/* -------------------------------
	Step 1 : Colours images to NDG 
	------------------------------- */
		
	// Step 2 : NDG Image to binary image
	// Step 3 : Extraction of reference image
	// Step 4 : Difference from reference image
	// Step 5 : Morphologie
	// Step 6 : Etiquettage
	// Step 7 : Tracking
}

void unitTest_colorToNdg()
{
	
	long nrh,nrl,nch,ncl;

	// chargement image original
	rgb8 **ImageCol_original;
	char nom_imageCol_original[255];
	sprintf(nom_imageCol_original, "Images/UnitTest/original.ppm");

	// declaration image resultat NDG
	byte **Image_ndg;
	char nom_imageNDG[255];
	sprintf(nom_imageNDG, "Images/UnitTest/original_ndg.pgm");
	
	colorToNdg(ImageCol_original, nom_imageCol_original, Image_ndg, nom_imageNDG);

	// Liberation des ressources
	free_bmatrix(Image_ndg, nrl,nrh,ncl,nch);
	free_rgb8matrix(ImageCol_original, nrl,nrh,ncl,nch);
}

void unitTest_ndgToBinary()
{
	long nrh,nrl,nch,ncl;

	// chargement image original
	byte **ImageNDG_original;
	char nom_imageNDG_original[255];
	sprintf(nom_imageNDG_original, "Images/UnitTest/original_ndg.pgm");

	// declaration image resultat NDG
	byte **Image_binary;
	char nom_imageBinary[255];
	sprintf(nom_imageBinary, "Images/UnitTest/original_binary.pgm");
	
	ndgToBinary(ImageNDG_original, nom_imageNDG_original, Image_binary, nom_imageBinary);

	// Liberation des ressources
	free_bmatrix(ImageNDG_original, nrl,nrh,ncl,nch);
	free_bmatrix(Image_binary, nrl,nrh,ncl,nch);	
}

void unitTest_extractRefImg()
{
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
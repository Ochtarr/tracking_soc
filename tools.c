#include "tools.h"


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

	//déclaration table de correspondance
	int tableCorrespondance[maxEtiquette][2];
	int a;
	for(a=0;a<maxEtiquette;a++)
	{
		tableCorrespondance[a][0] = a+1;
		tableCorrespondance[a][1] = 0;
	}

	//preparation fichier de sortie pour l'image en cours
	i = 0;
	while (nomImageResultat[i] != '.')
	{
		nameF[i] = nomImageResultat[i];
		i++;
	}
	nameF[i] = '.'; nameF[i+1] = 't'; nameF[i+2] = 'x'; nameF[i+3] = 't';

	// 1er balayage de l'image
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

/*
	ici, on reparcourt l'ensemble des pixels précédant. Passage à supprimer
	le but ici est d'implémenter la méthode non intuitive, pour gagner en perfs
	(on s'assure qu'il n'y ait que 2 passages de boucle)
	TODO: implémenter la table de correspondance
	TODO: implémenter le deuxième parcours de boucle pour maj label region (avec table de correspondance)
*/
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



// 2e balayage
	//on parcourt tous les éléments de la matrice, représentant le numéro
	//de l'étiquette de la région du pixel dans l'image
	for (i = nrl; i < nrh; i++)
	{
		for (j = ncl; j < nch; j++)
		{
			etiquette[i][j];
		}
	}


// Calcul et sauvegarde des données caractéristiques des régions

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



void colorToNdg(rgb8** ImageColor, char* nomImageColor, byte** ImageNdg, char* nomImageNdg,
				long nrl, long nrh, long ncl, long nch)
{
	// Allocation
	ImageColor = LoadPPM_rgb8matrix(nomImageColor, &nrl,&nrh,&ncl,&nch);
	ImageNdg = bmatrix(nrl,nrh,ncl,nch);
	
	// Traitement
	int lig, col;
	for(lig=0; lig<=nrh; lig++)
	{
		for(col=0; col<=nch+1; col++)
		{
			//Récupération des valeurs des pixels
			int moy = (
				ImageColor[lig][col].r +  
				ImageColor[lig][col].g + 
				ImageColor[lig][col].b) / 3;
			ImageNdg[lig][col] = moy;
		}
	}

	// Ecriture
	SavePGM_bmatrix(ImageNdg,nrl,nrh,ncl,nch,nomImageNdg);
}

void ndgToBinary(byte** ImageNdg, char* nomImageNdg, byte** ImageBinary, char* nomImageBinary,
				long nrl, long nrh, long ncl, long nch)
{
	// Allocation
	ImageNdg = LoadPGM_bmatrix(nomImageNdg, &nrl,&nrh,&ncl,&nch);
	ImageBinary = bmatrix(nrl,nrh,ncl,nch);
	
	// Traitement
	int lig, col;
	for(lig=0; lig<=nrh; lig++)
	{
		for(col=0; col<=nch+1; col++)
		{

			//Récupération des valeurs des pixels
			if(ImageNdg[lig][col] >= SEUIL_BINARISATION)
				ImageBinary[lig][col] = 1;
			else
				ImageBinary[lig][col] = 0;
		}
	}

	// Ecriture
	SavePGM_bmatrix(ImageBinary,nrl,nrh,ncl,nch,nomImageBinary);
}

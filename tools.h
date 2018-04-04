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

float** getPixels(rgb8** Image, long nrh, long nch);
float getMoyenne(float** tabPixels, long nrh, long nch);
int getMedian(int* tabPixels, long nch);

int compare_int(const void* a, const void* b);
void etiquettage(byte** binary, rgb8 ** image, char nomImageResultat[255], long nrh, long nrl, long nch, long ncl);



void colorToNdg(rgb8** ImageColor, char* nomImageColor, byte** ImageNdg, char* nomImageNdg, long nrl, long nrh, long ncl, long nch);
void ndgToBinary(byte** ImageNdg, char* nomImageNdg, byte** ImageBinary, char* nomImageBinary, long nrl, long nrh, long ncl, long nch);



/*---------------------- 
    Variables globales 
------------------------*/

#define SEUIL 80
#define SEUIL_BINARISATION 80
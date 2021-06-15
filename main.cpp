#include <stdio.h>
#include <string.h>
#include <math.h>
#include "tensor.h"
#include "libbmp.h"
#include "DAISGram.h"


void show_help(){
    printf("*** DAISGram ***\n");
    printf("\targ 1: input file name (img1) \n");
    printf("\targ 2: input file name (img2) \n");
    printf("\targ 3: operazione da effettuare (gray, brighten, blend, sharp, edge, emboss, smooth, warhol, equalize, chromakey) \n");
    printf("\targ 4: output file name\n");
    printf("\targ 5: Diversi significati in funzione dell'operazione (default 3):\n"
           "\t\t- [smooth]: kernel size \n"
           "\t\t- [brighten]: valore bright per aumentare la luminosità \n"
           "\t\t\n");
    printf("\targ 6: Diversi significati in funzione dell'operazione (default 1.0):\n"
           "\t\t- [blend] parametro alpha per il blending di due immagini"); 
    printf("\n");
}

enum Filter { sharpen, smooth3, smooth5, smooth7, emboss, edge, grey, brighten20, brighten100};

int main (int argc, char * argv[]) 
{
/*
    DAISGram dais, moon, result, compare;
	dais.load_image("images/dais.bmp");
	moon.load_image("images/fullmoon.bmp");

	vector<string> outputs {
		"dais_brighten_20", "dais_brighten_100", "dais_edge", "dais_gray", "dais_sharp",
		"dais_smooth_3", "dais_smooth_5", "dais_smooth_7", "dais_warhol",
		"dais_equalize_int_cast", "dais_equalize", "fullmoon_equalize_int_cast", "fullmoon_equalize"
	};

	for(int i = 0; i < outputs.size(); i++)
	{
		switch (i)
		{
		case 0: 	result = dais.brighten(20);		break;
		case 1: 	result = dais.brighten(100);	break;
		case 2: 	result = dais.edge();			break;
		case 3: 	result = dais.grayscale();		break;
		case 4: 	result = dais.sharpen();		break;
		case 5: 	result = dais.smooth(3);		break;
		case 6: 	result = dais.smooth(5);		break;
		case 7: 	result = dais.smooth(7);		break;
		case 8: 	result = dais.warhol();			break;
		case 9: 	result = dais.equalize();		break;
		case 10: 	result = dais.equalize();		break;
		case 11: 	result = moon.equalize();		break;
		case 12: 	result = moon.equalize();		break;
		}

		string resultName = outputs.at(i) + ".bmp";
		string resultPath = "testing/" + resultName;

		result.save_image(resultPath);
		result.load_image(resultPath);
		compare.load_image("results/" + resultName);

		if(!(result.data == compare.data))
			cout << outputs.at(i) << endl;
	}
*/
    return 0; /* ciao a tutti!*/
}
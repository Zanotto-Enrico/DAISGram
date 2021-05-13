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

int main (int argc, char * argv[]) {

    DAISGram b, c, img;

    vector<string> imageNames {"flower_hires", "dais", "fullmoon", "seba"};
	vector<Filter> filterNames {sharpen, smooth3, smooth5, smooth7, emboss, edge, grey, brighten20, brighten100};

	for(const auto& imageName : imageNames)
	{
		b.load_image("images/" + imageName + ".bmp");
		for(const auto& filterName : filterNames)
		{
            cout << "Applying filter " << "\"" << filterName << "\"" << " to image " << imageName << endl;
            string Fname = "";
            switch (filterName)
            {
                case sharpen:       img = b.sharpen();      Fname = "sharpen";  break;
                case smooth3:       img = b.smooth(3);      Fname = "smooth3";   break;
                case smooth5:       img = b.smooth(5);      Fname = "smooth5";   break;
                case smooth7:       img = b.smooth(7);      Fname = "smooth7";   break;
                case emboss:        img = b.emboss();       Fname = "emboss";   break;
                case edge:          img = b.edge();         Fname = "edge";     break;
                case grey:          img = b.grayscale();    Fname = "grayscale";     break;
                case brighten20:    img = b.brighten(20);   Fname = "bighten20";     break;
                case brighten100:   img = b.brighten(100);  Fname = "brighten100";     break;
            }
            img.save_image("testing/basic_filters/" + imageName + "/" + Fname + ".bmp");
        }
    }

    b.load_image("images/equalize/hill.bmp");
    c.load_image("images/fullmoon.bmp");
    img = b.equalize();
    img.save_image("testing/hill_equalized.bmp");
    img = c.equalize();
    img.save_image("testing/fullmoon_equalized.bmp");

    b.load_image("images/dais.bmp");
    img = b.warhol();
    img.save_image("testing/warhol.bmp");

    b.load_image("images/blend/blend_a.bmp");
    c.load_image("images/blend/blend_b.bmp");
    float alpha = 0.0F;
    while (alpha <= 1)
    {
        img = b.blend(c,alpha);
        img.save_image("testing/blend_"+ to_string(alpha).substr(0,4) +".bmp");
        alpha += 0.25F;
    }

    b.load_image("images/greenscreen/gs_2.bmp");
    c.load_image("images/greenscreen/gs_2_bkg.bmp");
    int rgb[3]={144, 208, 49};
    float th[3]={100, 100, 50};
    img = b.greenscreen(c,rgb,th);
    img.save_image("testing/dais_matrix.bmp");

    b.load_image("images/greenscreen/gs_4.bmp");
    c.load_image("images/greenscreen/gs_4_bkg.bmp");
    int rgb1[3]={226,225,220};
    float th1[3]={50, 50, 50};
    img = b.greenscreen(c,rgb1,th1);
    img.save_image("testing/seba_flower.bmp");



    return 0; /* ciao a tutti!*/
}
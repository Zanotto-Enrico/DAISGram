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

Tensor load_image(string filename)
{
	BmpImg img = BmpImg();

	img.read(filename.c_str());

	const int h = img.get_height();
	const int w = img.get_width();

	Tensor result = Tensor(h, w, 3, 0.0);

	for(int i=0;i<h;i++){
		for(int j=0;j<w;j++){ 
			result(i,j,0) = (float) img.red_at(j,i);
			result(i,j,1) = (float) img.green_at(j,i);    
			result(i,j,2) = (float) img.blue_at(j,i);   
		}                
	}

	return result;
}

void save_image(Tensor data, string filename)
{
    data.clamp(0,255);

    BmpImg img = BmpImg(data.cols(), data.rows());

    img.init(data.cols(), data.rows());

    for(int i=0;i<data.rows();i++){
        for(int j=0;j<data.cols();j++){
            img.set_pixel(j,i,(unsigned char) data(i,j,0),(unsigned char) data(i,j,1),(unsigned char) data(i,j,2));                   
        }                
    }

    img.write(filename);
}

void PrintTensor(Tensor& t, string name)
{
	cout << name << ":" << endl << endl;

	for(int ch = 0; ch < t.depth(); ch++)
	{
		cout << "channel" << ch << ":" << endl;
		for(int row = 0; row < t.rows(); row++)
		{
			cout << "[";
			for(int col = 0; col < t.cols(); col++)
				cout << t(row, col, ch) << ", ";
			cout << "\b\b]" << endl;
		}
		cout << endl;
	}
	cout << endl << endl;
}

int main (int argc, char * argv[]) 
{
	vector<string> imageNames {"flower_hires", "dais", "fullmoon", "seba"};
	vector<string> filterNames {"sharpen", "smooth", "emboss"};

	for(const auto& imageName : imageNames)
	{
		Tensor img = load_image("images/" + imageName + ".bmp");
		for(const auto& filterName : filterNames)
		{
			cout << "Applying filter " << "\"" << filterName << "\"" << " to image " << imageName << endl;

			Tensor filter;
			if(filterName == "smooth")
				filter.init(7,7,3,1/(float)49);
			else
				filter.read_file("filters/" + filterName);

			Tensor result = img.convolve(filter);
			save_image(result, "testing/" + imageName + "/" + filterName + ".bmp");
		}
	}
}

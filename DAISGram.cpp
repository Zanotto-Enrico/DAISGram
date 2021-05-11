#include <iostream>
#include <string>

#include "dais_exc.h"
#include "tensor.h"
#include "libbmp.h"
#include "DAISGram.h"

using namespace std;

DAISGram::DAISGram()
{
	
}

DAISGram::~DAISGram()
{
	
}

/**
 * Load a bitmap from file
 *
 * @param filename String containing the path of the file
 */
void DAISGram::load_image(string filename){
    BmpImg img = BmpImg();

    img.read(filename.c_str());

    const int h = img.get_height();
    const int w = img.get_width();

    data = Tensor(h, w, 3, 0.0);

    for(int i=0;i<img.get_height();i++){
        for(int j=0;j<img.get_width();j++){ 
            data(i,j,0) = (float) img.red_at(j,i);
            data(i,j,1) = (float) img.green_at(j,i);    
            data(i,j,2) = (float) img.blue_at(j,i);   
        }                
    }
}


/**
 * Save a DAISGram object to a bitmap file.
 * 
 * Data is clamped to 0,255 before saving it.
 *
 * @param filename String containing the path where to store the image.
 */
void DAISGram::save_image(string filename){

    data.clamp(0,255);

    BmpImg img = BmpImg(getCols(), getRows());

    img.init(getCols(), getRows());

    for(int i=0;i<getRows();i++){
        for(int j=0;j<getCols();j++){
            img.set_pixel(j,i,(unsigned char) data(i,j,0),(unsigned char) data(i,j,1),(unsigned char) data(i,j,2));                   
        }                
    }

    img.write(filename);

}


/**
 * Generate Random Image
 * 
 * Generate a random image from nois
 * 
 * @param h height of the image
 * @param w width of the image
 * @param d number of channels
 * @return returns a new DAISGram containing the generated image.
 */  
void DAISGram::generate_random(int h, int w, int d){
    data = Tensor(h,w,d,0.0);
    data.init_random(128,50);
    data.rescale(255);
}

int DAISGram::getRows()
{    
    return data.rows();
}

int DAISGram::getCols()
{
    return data.cols();
}

int DAISGram::getDepth()
{
    return data.depth();
}

DAISGram DAISGram::brighten(float bright)
{
    DAISGram result;
    result.data = Tensor(data);
    for (int r = 0; r < result.data.rows(); r++)
        for (int c = 0; c < result.data.cols(); c++)
            for (int d = 0; d < result.data.depth(); d++)
                result.data(r,c,d) += bright;
    result.data.clamp(0,255);
    return result;
}

DAISGram DAISGram::grayscale()
{
    DAISGram result;
    result.data = Tensor(data);
    float mean = 0;
    for (int r = 0; r < result.data.rows(); r++)
    {
        for (int c = 0; c < result.data.cols(); c++)
        {
            mean = 0;
            for (int d = 0; d < result.data.depth(); d++)  mean += result.data(r,c,d);
            mean = mean/3;
            for (int d = 0; d < result.data.depth(); d++)  result.data(r,c,d) = mean;
        }
    }
    return result;
}

DAISGram DAISGram::warhol()
{
    Tensor normal    = Tensor(data);
    Tensor RedGreen  = Tensor(data);
    Tensor BlueGreen = Tensor(data);
    Tensor RedBlue   = Tensor(data);

    for (int r = 0; r < data.rows(); r++)
    {
        for (int c = 0; c < data.cols(); c++)
        {
            //swap red and green
            RedGreen(r,c,0)  =  data(r,c,1);
            RedGreen(r,c,1)  =  data(r,c,0);
            //swap blue and green
            BlueGreen(r,c,1) =  data(r,c,2);
            BlueGreen(r,c,2) =  data(r,c,1);
            //swap red and blue
            RedBlue(r,c,0)   =  data(r,c,2);
            RedBlue(r,c,2)   =  data(r,c,0);
        }
    }
    //concateno le 4 immagini
    Tensor top = normal.concat(RedGreen,1);
    Tensor bottom = BlueGreen.concat(RedBlue,1);
    DAISGram result;
    result.data = top.concat(bottom,0);
    
    return result;
}

DAISGram DAISGram::sharpen()
{
    Tensor filter;
    filter.read_file("filters/sharpen");
    DAISGram result;
    result.data = Tensor(data).convolve(filter);
    return result;
}

DAISGram DAISGram::emboss()
{
    Tensor filter;
    filter.read_file("filters/emboss");
    DAISGram result;
    result.data = Tensor(data).convolve(filter);
    return result;
}

DAISGram DAISGram::smooth(int h)
{
    Tensor filter = Tensor(h,h,3,h*h);
    DAISGram result;
    result.data = Tensor(data).convolve(filter);
    return result;
}

DAISGram DAISGram::edge()
{
    Tensor filter;
    filter.read_file("filters/edge");
    DAISGram result;
    result.data = Tensor(data).convolve(filter);
    return result;
}

DAISGram DAISGram::blend(const DAISGram & rhs, float alpha)
{
    if(getRows() != rhs.data.rows() || getCols() != rhs.data.cols() || getDepth() != rhs.data.depth())
        throw(dimension_mismatch());
    DAISGram result;
    result.data = Tensor(data);
    for (int r = 0; r < result.data.rows(); r++)
        for (int c = 0; c < result.data.cols(); c++)
            for (int d = 0; d < result.data.depth(); d++)
                result.data(r,c,d) = result.data(r,c,d)*alpha + (1 - alpha)*rhs.data(r,c,d);

    return result;
}

DAISGram DAISGram::greenscreen(DAISGram & bkg, int rgb[], float threshold[])
{
    DAISGram result;
    result.data = Tensor(data);
    for (int r = 0; r < result.getRows(); r++)
        for (int c = 0; c < result.getCols(); c++)
                if( (rgb[0] - threshold[0] <= result.data(r,c,0)) && (result.data(r,c,0) <= rgb[0] + threshold[0]) &&
                    (rgb[1] - threshold[1] <= result.data(r,c,1)) && (result.data(r,c,1) <= rgb[1] + threshold[1]) &&
                    (rgb[2] - threshold[2] <= result.data(r,c,2)) && (result.data(r,c,2) <= rgb[2] + threshold[2])   )
                {
                    for (int d = 0; d < getDepth(); d++)
                        result.data(r,c,d) = bkg.data(r,c,d);
                }            
    return result;
}

DAISGram DAISGram::equalize()
{
    DAISGram result;
    result.data = Tensor(data);
    float maxValue = 0, minValue = 0;
    float cdfMin = 0, cdfMax = 0;

    for (int ch = 0; ch < 3; ch++)//per ogni livello di depth del tensore
    {
        maxValue = result.data.getMax(ch);
        minValue = result.data.getMin(ch);
        int DistLenght = maxValue-minValue+1;
        int *Distribution = new int[DistLenght]();

        //calcolo dell'istogramma
        for (int row = 0; row < result.getRows(); row++)
            for (int col = 0; col < result.getCols(); col++)
                {
                    int value = result.data(row,col,ch) - minValue;
                    Distribution[value] += 1;
                }

        //calcolo della distribuzione
        int cdf = 0; 
        cdfMin = Distribution[0]; 
        for (int j = 0; j < DistLenght; j++){
            Distribution[j] += cdf;
            cdf = Distribution[j];
        }
        cdfMax = Distribution[DistLenght - 1] - 1;
        
        //sostituzione dei pixels
        for (int row = 0; row < result.getRows(); row++)
            for (int col = 0; col < result.getCols(); col++)
                {
                    int Pos = result.data(row,col,ch) - minValue;
                    result.data(row,col,ch) = round(((Distribution[Pos] - cdfMin) * 255)/cdfMax);   
                }
        delete[] Distribution;
    }
    return result;
}
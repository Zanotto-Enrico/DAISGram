#include <iostream>
#include <string>
#include <random>
#include <math.h>
#include <fstream>

#include "dais_exc.h"
#include "tensor.h"

#define PI 3.141592654
#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN 1.175494351e-38F /* min positive value */

using namespace std;


/**
 * Random Initialization
 * 
 * Perform a random initialization of the tensor
 * 
 * @param mean The mean
 * @param std  Standard deviation
 */
void Tensor::init_random(float mean, float std){
    if(data){

        std::default_random_engine generator;
        std::normal_distribution<float> distribution(mean,std);

        for(int i=0;i<r;i++){
            for(int j=0;j<c;j++){
                for(int k=0;k<d;k++){
                    this->operator()(i,j,k)= distribution(generator);
                }
            }
        }    

    }else{
        throw(tensor_not_initialized());
    }
}

Tensor::Tensor()
{

}

Tensor::Tensor(int r, int c, int d, float v)
{
	init(r, c, d, v);
}

Tensor::Tensor(const Tensor& that)
{
	Copy(that);
}

Tensor::~Tensor()
{
	WipeData();
}

float Tensor::operator()(int row, int col, int ch) const
{
	if(	row >= this->r || col >= this->c || ch >= this->d ||
		row < 0 || col < 0 || ch < 0)
		throw(index_out_of_bound());

	return data[row][col][ch];
}

float& Tensor::operator()(int row, int col, int ch)
{
	if(	row >= this->r || col >= this->c || ch >= this->d ||
		row < 0 || col < 0 || ch < 0)
		throw(index_out_of_bound());

	return data[row][col][ch];
}

Tensor Tensor::operator-(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());

	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) -= rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator+(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());

	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) += rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator*(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());
		
	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) *= rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator/(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());
		
	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) /= rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator-(const float &rhs)
{
	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) -= rhs;

	return result;
}

Tensor Tensor::operator+(const float &rhs)
{
	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) += rhs;

	return result;
}

Tensor Tensor::operator*(const float &rhs)
{
	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) *= rhs;

	return result;
}

Tensor Tensor::operator/(const float &rhs)
{
	Tensor result = Tensor(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) /= rhs;

	return result;
}

Tensor& Tensor::operator=(const Tensor &other)
{
	Copy(other);
	return *this;
}

void Tensor::init(int r, int c, int d, float v)
{
	WipeData();

	this->r = r;
	this->c = c;
	this->d = d;

	data = new float**[r];
	for(int row = 0; row < r; row++)
	{
		data[row] = new float*[c];
		for(int col = 0; col < c; col++)
		{
			data[row][col] = new float[d];
			for(int ch = 0; ch < d; ch++)
				data[row][col][ch] = v;
		}
	}
}

void Tensor::clamp(float low, float high)
{
	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				if(operator()(row, col, ch) < low)
					operator()(row, col, ch) = low;
				else if(operator()(row, col, ch) > high)
					operator()(row, col, ch) = high;
}


int Tensor::rows()
{
	return r;
}

int Tensor::cols()
{
	return c;
}

int Tensor::depth()
{
	return d;
}

float Tensor::getMin(int ch)
{
	float min {operator()(0, 0, ch)};
	for(int row = 0; row < r; row++)
		for(int col = 1; col < c; col++)
		{
			float val = operator()(row, col, ch);
			if(val < min)
				min = val;
		}

	return min;
}

float Tensor::getMax(int ch)
{
	float max {operator()(0, 0, ch)};
	for(int row = 0; row < r; row++)
		for(int col = 1; col < c; col++)
		{
			float val = operator()(row, col, ch);
			if(val > max)
				max = val;
		}

	return max;
}

void Tensor::showSize()
{
	cout << r << " x " << c << " x " << d << endl;
}

ostream& operator<< (ostream& stream, const Tensor & obj)
{
	for(int row = 0; row < obj.r; row++)
		for(int col = 0; col < obj.c; col++)
			for(int ch = 0; ch < obj.d; ch++)
				stream << "[" << row << "," << col << "," << obj(row, col, ch) << "]\n";

	return stream;
}

void Tensor::read_file(string filename)
{
	ifstream file;
	file.open (filename);

	file >> r;
	file >> c;
	file >> d;

	WipeData();
	init(r, c, d);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				file >> operator()(row, col, ch);

	file.close();
}

void Tensor::write_file(string filename)
{
	ofstream file;
	file.open (filename);

	file << r << '\n' << c << '\n' << d << '\n';

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				file << operator()(row, col, ch) << '\n';

	file.close();
}
void Tensor::Copy(const Tensor& other)
{
	WipeData();

	r = other.r;
	c = other.c;
	d = other.d;

	data = new float**[r];
	for(int row = 0; row < r; row++)
	{
		data[row] = new float*[c];
		for(int col = 0; col < c; col++)
		{
			data[row][col] = new float[d];
			for(int ch = 0; ch < d; ch++)
				data[row][col][ch] = other.data[row][col][ch];
		}
	}
}

void Tensor::WipeData()
{
	if(data)
	{
		for(int row = 0; row < r; row++)
		{
			if(data[row])
			{
				for(int col = 0; col < c; col++)
					if(data[row][col])
						delete[] data[row][col];
				delete[] data[row];
			}
		}
		delete[] data;
		data = nullptr;
	}
}

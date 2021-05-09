#include <iostream>
#include <string>
#include <random>
#include <math.h>
#include <fstream>
#include <string.h>

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

Tensor::Tensor(int r, int c, int d, float v)
{
	this->r = r;
	this->c = c;
	this->d = d;
	data = new float[r * c * d];
	if(v)
		init(r, c, d, v);
}

Tensor::Tensor(const Tensor& that)
{
	r = that.r;
	c = that.c;
	d = that.d;

	int size = r * c * d;
	data = new float[size];
	memcpy(data, that.data, size * sizeof(float));
}

Tensor::~Tensor()
{
	delete[] data;
	data = nullptr;
}

float Tensor::operator()(int i, int j, int k) const
{
	if(!data)
		throw(index_out_of_bound());

	return data[(i * c * d) + (j * d) + k];
}

float& Tensor::operator()(int i, int j, int k)
{
	if(!data)
		throw(index_out_of_bound());

	return data[(i * c * d) + (j * d) + k];
}

Tensor Tensor::operator-(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());

	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] -= rhs.data[n];

	return result;
}

Tensor Tensor::operator+(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());

	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] += rhs.data[n];

	return result;
}

Tensor Tensor::operator*(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());
		
	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] *= rhs.data[n];

	return result;
}

Tensor Tensor::operator/(const Tensor &rhs)
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());
		
	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] /= rhs.data[n];

	return result;
}

Tensor Tensor::operator-(const float &rhs)
{
	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] -= rhs;

	return result;
}

Tensor Tensor::operator+(const float &rhs)
{
	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] += rhs;

	return result;
}

Tensor Tensor::operator*(const float &rhs)
{
	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] *= rhs;

	return result;
}

Tensor Tensor::operator/(const float &rhs)
{
	Tensor result = Tensor(*this);

	int size = r * c * d;
	for(int n = 0; n < size; n++)
		result.data[n] /= rhs;

	return result;
}

Tensor& Tensor::operator=(const Tensor &other)
{
	r = other.r;
	c = other.c;
	d = other.d;

	int size = r * c * d;
	delete[] data;
	data = new float[size];
	memcpy(data, other.data, size * sizeof(float));

	return *this;
}

void Tensor::init(int r, int c, int d, float v)
{
	int size = r * c * d;
	for(int n = 0; n < size; n++)
		data[n] = v;
}

void Tensor::clamp(float low, float high)
{
	int size = r * c * d;
	for(int n = 0; n < size; n++)
		if(data[n] < low)
			data[n] = low;
		else if(data[n] > high)
			data[n] = high;
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

float Tensor::getMin(int k)
{
	float min {operator()(0, 0, k)};
	for(int i = 0; i < r; i++)
		for(int j = 1; j < c; j++)
		{
			float val = operator()(i, j, k);
			if(val < min)
				min = val;
		}

	return min;
}

float Tensor::getMax(int k)
{
	float max {operator()(0, 0, k)};
	for(int i = 0; i < r; i++)
		for(int j = 1; j < c; j++)
		{
			float val = operator()(i, j, k);
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
	for(int i = 0; i < obj.r; i++)
		for(int j = 0; j < obj.c; j++)
			for(int k = 0; k < obj.d; k++)
				stream << "[" << i << "," << j << "," << obj(i, j, k) << "]\n";

	return stream;
}

void Tensor::read_file(string filename)
{
	ifstream file;
	file.open (filename);

	file >> r;
	file >> c;
	file >> d;

	delete[] data;
	data = new float[r * c * d];
	for(int i = 0; i < r; i++)
		for(int j = 0; j < c; j++)
			for(int k = 0; k < d; k++)
				file >> operator()(i, j, k);

	file.close();
}

void Tensor::write_file(string filename)
{
	ofstream file;
	file.open (filename);

	file << r << '\n' << c << '\n' << d << '\n';

	for(int i = 0; i < r; i++)
		for(int j = 0; j < c; j++)
			for(int k = 0; k < d; k++)
				file << operator()(i, j, k) << '\n';

	file.close();
}
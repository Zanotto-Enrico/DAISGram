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
	delete[] data;
}

bool Tensor::operator==(const Tensor& rhs) const
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw dimension_mismatch();

	float maxdiff = 0;
	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				if(abs(operator()(row, col, ch) - rhs(row, col, ch)) > EPSILON)
					return false;
	return true;
}

float Tensor::operator()(int row, int col, int ch) const
{
	if(!data)
		throw tensor_not_initialized();
	
	if(	row >= this->r || col >= this->c || ch >= this->d ||
		row < 0 || col < 0 || ch < 0)
		throw(index_out_of_bound());

	return data[row * c * d + col * d + ch];
}

float& Tensor::operator()(int row, int col, int ch)
{
	if(!data)
		throw tensor_not_initialized();
	
	if(	row >= this->r || col >= this->c || ch >= this->d ||
		row < 0 || col < 0 || ch < 0)
		throw(index_out_of_bound());

	return data[row * c * d + col * d + ch];
}

Tensor Tensor::operator-(const Tensor &rhs)const
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());

	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) -= rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator+(const Tensor &rhs)const
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());

	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) += rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator*(const Tensor &rhs)const
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());
		
	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) *= rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator/(const Tensor &rhs)const
{
	if(r != rhs.r || c != rhs.c || d != rhs.d)
		throw(dimension_mismatch());
		
	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) /= rhs(row, col, ch);

	return result;
}

Tensor Tensor::operator-(const float &rhs)const
{
	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) -= rhs;

	return result;
}

Tensor Tensor::operator+(const float &rhs)const
{
	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) += rhs;

	return result;
}

Tensor Tensor::operator*(const float &rhs)const
{
	Tensor result(*this);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row, col, ch) *= rhs;

	return result;
}

Tensor Tensor::operator/(const float &rhs)const
{
	Tensor result(*this);

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
	if((r <= 0 || c <= 0 || d <= 0))
		throw unknown_exception();

	delete[] data;

	this->r = r;
	this->c = c;
	this->d = d;

	int dim = r * c * d;
	data = new float[dim];
	for(int i = 0; i < dim; i++)
		data[i] = v;
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


int Tensor::rows()const
{
	return r;
}

int Tensor::cols()const
{
	return c;
}

int Tensor::depth()const
{
	return d;
}

float Tensor::getMin(int ch)const
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

float Tensor::getMax(int ch)const
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

void Tensor::showSize()const
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

	if(file.rdstate() && file.failbit)
		throw unable_to_read_file();

	file >> r;
	file >> c;
	file >> d;

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

Tensor Tensor::padding(int pad_h, int pad_w)const
{
	Tensor result(r + 2 * pad_h, c + 2 * pad_w, d);

	for(int row = 0; row < r; row++)
		for(int col = 0; col < c; col++)
			for(int ch = 0; ch < d; ch++)
				result(row + pad_h, col + pad_w, ch) = operator()(row, col, ch);

	return result;
}

Tensor Tensor::subset(unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end, unsigned int depth_start, unsigned int depth_end)const
{
	Tensor result(row_end - row_start, col_end - col_start, depth_end - depth_start);

	for(int row = 0; row < result.rows(); row++)
		for(int col = 0; col < result.cols(); col++)
			for(int ch = 0; ch < result.depth(); ch++)
				result(row, col, ch) = operator()(row + row_start, col + col_start, ch + depth_start);
	
	return result;
}

Tensor Tensor::concat(const Tensor &rhs, int axis)const
{
	if(d != rhs.d || (r != rhs.r && axis) || (c != rhs.c && !axis))
		throw(concat_wrong_dimension());
	
	if(axis != 0 && axis != 1)
		throw unknown_operation();
	
	Tensor result;
	if(axis)
		result.init(r, c + rhs.c, d);
	else
		result.init(r + rhs.r, c, d);

	for(int row = 0; row < result.r; row++)
		for(int col = 0; col < result.c; col++)
			for(int ch = 0; ch < result.d; ch++)
			{
				if(row < r && col < c)
					result(row, col, ch) = operator()(row, col, ch);
				else 
					result(row, col, ch) = rhs(row % r, col % c, ch);
			}

	return result;
}

void Tensor::rescale(float new_max)
{	
	int ch = 0;
	while(ch < d)
	{
		float min = getMin(ch);
		float max = getMax(ch);

		if(max != min)
		{
			for(int row = 0; row < r; row++)
				for(int col = 0; col < c; col++)
					operator()(row, col, ch) = ((operator()(row, col, ch)-min)/(max-min))*new_max;
		}
		ch++;
	}
}

/*
	p_ = padded
	s_ = subsection
	r_ = result
*/
Tensor Tensor::convolve(const Tensor &f)const
{
	if(f.depth() != d)
		throw dimension_mismatch();

	if(!(f.rows() % 2) || !(f.cols() % 2))
		throw filter_odd_dimensions();
	
	int hp = (f.r - 1) / 2;
	int wp = (f.c - 1) / 2;

	Tensor padded = padding(hp, wp);
	Tensor result(r, c, d);


	int p_lastRow = r + hp;
	int p_lastCol = c + wp;

	for(int p_row = hp; p_row < p_lastRow; p_row++)
	{
		int r_row = p_row - hp; //result row == upper bound
		int lowerBound = p_row + hp + 1;

		for(int p_col = wp; p_col < p_lastCol; p_col++)
		{
			int r_col = p_col - wp; //result col == left bound

			Tensor subs = padded.subset(r_row, lowerBound, r_col, p_col + wp + 1, 0, f.d) * f;

			for(int s_row = 0; s_row < subs.r; s_row++)
				for(int s_col = 0; s_col < subs.c; s_col++)
					for(int ch = 0; ch < d; ch++)
						result(r_row, r_col, ch) += subs(s_row, s_col, ch);
		}
	}
	
	return result;
}

void Tensor::Copy(const Tensor& other)
{
	delete[] data;

	r = other.r;
	c = other.c;
	d = other.d;

	int dim = r * c * d;
	data = new float[dim];
	for(int i = 0; i < dim; i++)
		data[i] = other.data[i];
}

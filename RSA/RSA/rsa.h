#pragma once
#include <iostream>
using namespace std;
#include <time.h>
#include <fstream>
#include <math.h>
#include <boost/multiprecision/cpp_int.hpp>
typedef long DataType;
#define NUMBER 256         //Ԫ�صĸ���

struct Key
{
	// (e��n)  (d,n)
	DataType _ekey;      //���ܵ���Կ
	DataType _dkey;      //���ܵ���Կ
	DataType _pkey;		 //�����Ĳ���
};

/*
	1.��������
	2.��n
	3.��f(n)
	4.�� e   1< e < f(n) ,e �� f(n) ���ɻ��ʣ����ѡ��
	5.��d : e * d % f(n) = 1
	(e��n)  (d,n)
*/

class RSA
{
public:
	RSA()                                                  //���캯��
	{
		getKeys();
	}
	//����
	void ecrept(const char* filename, const char* fileout)
	{
		ifstream fin(filename, std::ifstream::binary);
		ofstream fout(fileout, std::ifstream::binary);

		if (!fin.is_open())
		{
			perror("Input file open failed!");
			return;
		}
		char * buffer = new char[NUMBER];     //ÿ�ζ�����ʱ������ݵĻ�����
		DataType* bufferOut = new DataType[NUMBER];
		while (!fin.eof())
		{
			fin.read(buffer, NUMBER);
			int curNum = fin.gcount();          //���һ�ζ�ȡ���ֽ���   Ҳ��ʾԪ�صĸ���
			for (int i = 0; i < curNum; ++i)
			{
				bufferOut[i] = ecrept((DataType)buffer[i], _key._ekey, _key._pkey);    //��ÿ���ֽڽ��м��ܵĲ���
			}
			fout.write((char*)bufferOut, curNum * sizeof(DataType));                   //�����ܺ����Ϣ���
		}

		delete[] bufferOut;
		delete[] buffer;
		fin.close();
		fout.close();
	}

	//����
	void dcrept(const char* filename, const char* fileout)
	{
		ifstream fin(filename, ifstream::binary); 
		ofstream fout(fileout,ifstream::binary);
		if (!fin.is_open())
		{
			perror("File open failed!");
			return;
		}
		DataType * buffer = new DataType[NUMBER];          //���뻺����
		char * bufferOut = new char[NUMBER];			   //���������

		while (!fin.eof())
		{
			fin.read((char*)buffer, NUMBER * sizeof(DataType));     //NUMBER * sizeof(DataType)  ��ȡ���ֽ���
			int num = fin.gcount();                                 //ʵ�ʶ�����ֽ���
			num /= sizeof(DataType);                                //ʵ�ʶ����Ԫ�ظ���

			for (int i = 0; i < num; ++i)
			{
				bufferOut[i] = decrept(buffer[i], _key._dkey, _key._pkey);
			}
			fout.write(bufferOut, num);
		}

		delete[] bufferOut;
		delete[] buffer;
		fout.close();
		fin.close();
	}


	void getKeys()                                             //��ʼ��
	{
		DataType prime1 = getPrime();
		DataType prime2 = getPrime();

		while (prime1 == prime2)
		{
			prime2 = getPrime();
		}

		DataType orla = getOrla(prime1, prime2);
		_key._pkey = getPkey(prime1, prime2);
		_key._ekey = getEkey(orla);
		_key._dkey = getDkey(_key._ekey, orla);
	}

	Key getAllKey()
	{
		return _key;
	}

	//���ܽ���  a^b % n
	DataType ecrept(DataType data, DataType ekey, DataType pkey)		  //����
	{
		//pow :�������
		//return (long)pow(data, ekey) % pkey;

		//i: 0,�������һλ
		DataType Ai = data;
		DataType msgE = 1;

		while (ekey) 
		{
			if (ekey & 1)            //ȡ�����ܵ���Կ�����Ƶ�ÿһλ�����λΪ1������۳�
			{
				msgE = (msgE * Ai) % pkey;
			}

			ekey >>= 1;				//�����ܵ���Կ�Ķ�����λ����
			Ai = (Ai * Ai) % pkey;     
		}
		return msgE;
	}

	DataType decrept(DataType data, DataType dkey, DataType pkey)		  //����
	{
		return ecrept(data, dkey, pkey);
	}

	DataType getPrime()                                       //��ȡ����
	{
		srand(time(NULL));
		DataType prime;
		while (1)
		{
			prime = rand() % 100 + 2;
			if (isPrime(prime))
			{
				break;
			}
		}
		return prime;
	}

	bool isPrime(DataType data)								 //�ж��Ƿ�������
	{
		if (data <= 0)
		{
			return false;
		}
		for (int i = 2; i <= sqrt(data); i++)
		{
			if (data % i == 0)
			{
				return false;
			}
		}
		return true;
	}

	DataType getPkey(DataType prime1, DataType prime2)       //��ȡ��������
	{
		return prime1 * prime2;
	}

	DataType getOrla(DataType prime1, DataType prime2)		 //��ȡŷ��������ֵ
	{
		return (prime1 - 1) * (prime2 - 1);
	}

	//e ��1< e < f(n)
	DataType getEkey(DataType orla)						    //��ȡ������Կ
	{
		srand(time(NULL));
		DataType ekey;
		while (1)
		{
			ekey = rand() % orla;
			if (ekey > 1 && getGcd(ekey, orla) == 1)
			{
				return ekey;
			}
		}
	}

	//d : e * d % f(n) = 1;
	DataType getDkey(DataType ekey, DataType orla)			 //��ȡ������Կ
	{
		DataType dkey = orla / ekey;
		while (1)
		{
			if ((ekey * dkey) % orla == 1)
			{
				return dkey;
			}
			++dkey;
		}
	}

	//շת�����
	DataType getGcd(DataType data1, DataType data2)		 //��ȡ���Լ��
	{
		DataType tmp = 0;

		while (tmp = data1 % data2)
		{
			data1 = data2;
			data2 = tmp;
		}

		return data2;
	}
private:
	Key _key;
};
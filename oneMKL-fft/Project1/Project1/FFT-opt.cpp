#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <fftw3.h>
#include<time.h>
#include <mkl.h>
#include "mkl_service.h"
#include "mkl_dfti.h"
#define n 2048//����ά��
#define m 100//���Դ���
#define BRNG VSL_BRNG_MT19937// �����������������

float Radomnumber(float* arr, int SEED)
{
	//mkl���������
	VSLStreamStatePtr stream; // �����������
	// ��ʼ�������������
	vslNewStream(&stream, BRNG, SEED);
	// ������������洢��������
	vsRngUniform(VSL_RNG_METHOD_UNIFORMBITS32_STD, stream, n * n, arr, 0.0, 1.0);
	return (*arr);
	free(arr);
	vslDeleteStream(&stream);
}

void main()
{
	float misft = 1e-5;
	int s[m];
	int S = 1;
	clock_t start1[m], end1[m], start2[m], end2[m];
	double t1[m], t2[m], temp1 = 0, temp2 = 0, T1, T2;

	for (int i = 0; i < m; i++)
	{
		float* datain = (float*)malloc(sizeof(float) * n * n);   // �����ڴ�ռ�
		Radomnumber(datain, i);
		//fftw����FFT
		fftwf_complex* out = (fftwf_complex*)fftw_malloc(sizeof(fftwf_complex) * n * (n / 2 + 1));
		fftwf_plan p = fftwf_plan_dft_r2c_2d(n, n, datain, out, FFTW_ESTIMATE);    // �����任����
		start1[i] = clock();
		fftwf_execute(p);  // ִ�б任
		end1[i] = clock();

		//mkl����FFT
		MKL_LONG N[2] = { n, n };
		MKL_LONG status = 0;
		MKL_Complex8* dataout = NULL;
		MKL_LONG rs[3] = { 0, n, 1 };
		MKL_LONG cs[3] = { 0, n / 2 + 1, 1 };
		DFTI_DESCRIPTOR_HANDLE hand = NULL;

		status = DftiCreateDescriptor(&hand, DFTI_SINGLE, DFTI_REAL, 2, N);
		status = DftiSetValue(hand, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
		status = DftiSetValue(hand, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
		status = DftiSetValue(hand, DFTI_INPUT_STRIDES, rs);
		status = DftiSetValue(hand, DFTI_OUTPUT_STRIDES, cs);
		dataout = (MKL_Complex8*)mkl_malloc(n * (n / 2 + 1) * sizeof(MKL_Complex8), 64);
		status = DftiCommitDescriptor(hand);

		start2[i] = clock();
		status = DftiComputeForward(hand, datain, dataout);
		end2[i] = clock();

		status = DftiFreeDescriptor(&hand);

		//����ʱ��
		t1[i] = ((double)(end1[i] - start1[i]));
		temp1 += t1[i];
		t2[i] = ((double)(end2[i] - start2[i]));
		temp2 += t2[i];

		//�ԱȽ��
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < n / 2 + 1; k++)
			{
				float s1, s2;
				s1 = fabs(dataout[j * (n / 2 + 1) + k].real - out[j * (n / 2 + 1) + k][0]);
				s2 = fabs(dataout[j * (n / 2 + 1) + k].imag - out[j * (n / 2 + 1) + k][1]);
				if (s1 <= misft && s2 <= misft) s[i] = 1;
				else s[i] = 0;
			}
		}
		// ���ٱ任�������ͷ��ڴ�ռ�
		fftwf_destroy_plan(p);
		fftwf_free(out);
		mkl_free(dataout);
		free(datain);
	}

	T1 = temp1 / m;
	printf("fftw3ƽ�����е�ʱ����%f ms\n", T1);
	T2 = temp2 / m;
	printf("mklƽ�����е�ʱ����%lf ms\n", T2);

	//�ԱȽ��
	for (int i = 0; i < m; i++)
	{
		if (s[i] == 0) S = 0;
	}
	if (S) printf("�ԱȽ�������%f֮�ڣ������ȷ\n", misft);
	else printf("�ԱȽ�������%f֮�⣬�������\n", misft);

	system("pause");
}

#include <iostream>
#include <mkl.h>
#include <chrono>
#include "fftw/fftw3.h"

#define EPS (1e-5)
#define FFTTEST 0

// Function to compare the results of the FFT
bool compareResults(fftwf_complex* dataRef, MKL_Complex8* dataMKL, MKL_LONG sizeN[2]) {
	auto size2 = (sizeN[1] / 2 + 1);
	auto size = sizeN[0] * size2;
	for (int i = 0; i < size; i++) {
		auto MKLindex = i / size2 * sizeN[1] + i % size2;
		float diff_re = abs(dataRef[i][0] - dataMKL[MKLindex].real);
		float diff_im = abs(dataRef[i][1] - dataMKL[MKLindex].imag);
		if (diff_re > EPS || diff_im > EPS) {
			std::cout << "Result mismatch at index " << i << std::endl;
			return false;
		}
	}
	return true;
}
void printf1D(float* pData, int nsize) {
	for (int i = 0; i < nsize; i++)
	{
		std::cout << pData[i] << " ";
	}
	std::cout << std::endl;
}
void printf1D(fftwf_complex* pData, int nsize) {
	for (int i = 0; i < nsize; i++)
	{
		std::cout << pData[i][0] << "+" << pData[i][1] << "i  ";
	}
	std::cout << std::endl;
}
void printf1D(MKL_Complex8* pData, int nsize) {
	for (int i = 0; i < nsize; i++)
	{
		std::cout << pData[i].real << "+" << pData[i].imag << "i  ";
	}
	std::cout << std::endl;
}
void printf2D(float* pData, int nsizeZ, int nsizeX) {
	for (int i = 0; i < nsizeZ; ++i)
	{
		for (int j = 0; j < nsizeX; ++j)
		{
			std::cout << pData[i * nsizeX + j] << " ";

		}
		std::cout << std::endl;
	}
}
void printf2D(fftwf_complex* pData, int nsizeZ, int nsizeX) {
	for (int i = 0; i < nsizeZ; ++i)
	{
		for (int j = 0; j < nsizeX; ++j)
		{
			std::cout << pData[i * nsizeX + j][0] << "+" << pData[i * nsizeX + j][1] << "i  ";
		}
		std::cout << std::endl;
	}
}
void printf2D(MKL_Complex8* pData, int nsizeZ, int nsizeX) {
	for (int i = 0; i < nsizeZ; ++i)
	{
		for (int j = 0; j < nsizeX; ++j)
		{
			std::cout << pData[i * nsizeX + j].real << "+" << pData[i * nsizeX + j].imag << "i  ";
		}
		std::cout << std::endl;
	}
}
bool fftTest1D() {
	int nSize1D = 5;
	float* pfInputData1D = new float[nSize1D]; float* pfFFTW3ReData1D = new float[nSize1D]; float* pfMKLReData1D = new float[nSize1D];
	fftwf_complex* pfOutputData1DFFTW3 = new fftwf_complex[nSize1D];
	MKL_Complex8* pfOutputData1DMKL = new MKL_Complex8[nSize1D];
	memset(pfInputData1D, 0, sizeof(float) * nSize1D);
	pfInputData1D[0] = 1;
	DFTI_DESCRIPTOR_HANDLE hand = NULL;
	DftiCreateDescriptor(&hand, DFTI_SINGLE, DFTI_REAL, 1, nSize1D);
	DftiSetValue(hand, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
	DftiSetValue(hand, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
	DftiCommitDescriptor(hand);

	// Compute FFT and reFFT using oneMKL
	DftiComputeForward(hand, pfInputData1D, pfOutputData1DMKL);
	DftiComputeBackward(hand, pfOutputData1DMKL, pfMKLReData1D);
	DftiFreeDescriptor(&hand);

	// Compute FFT and reFFT using fftw3
	fftwf_plan forwardPlan = fftwf_plan_dft_r2c_1d(nSize1D, pfInputData1D, pfOutputData1DFFTW3, FFTW_ESTIMATE);
	fftwf_plan backwardPlan = fftwf_plan_dft_c2r_1d(nSize1D, pfOutputData1DFFTW3, pfFFTW3ReData1D, FFTW_ESTIMATE);
	fftwf_execute(forwardPlan);
	fftwf_execute(backwardPlan);
	fftwf_destroy_plan(forwardPlan);
	fftwf_destroy_plan(backwardPlan);

	//pfFFTW3ReData1D=pfFFTW3ReData1D/nSize1D; //伪代码反变换尺度回复需要处于采样点数
	//pfMKLReData1D=pfMKLReData1D/nSize1D;

	//printf
	std::cout << "Input data:" << std::endl;
	printf1D(pfInputData1D, nSize1D);
	std::cout << "FFTW3	FFT result:" << std::endl;
	printf1D(pfOutputData1DFFTW3, nSize1D);
	std::cout << "MKL FFT result:" << std::endl;
	printf1D(pfOutputData1DMKL, nSize1D);
	std::cout << "FFTW3	reFFT result:" << std::endl;
	printf1D(pfFFTW3ReData1D, nSize1D);
	std::cout << "MKL	reFFT result:" << std::endl;
	printf1D(pfMKLReData1D, nSize1D);

	// Cleanup
	delete[] pfInputData1D; delete[] pfFFTW3ReData1D; delete[] pfMKLReData1D;
	delete[] pfOutputData1DFFTW3;
	delete[] pfOutputData1DMKL;
	return true;
}
bool fftTest2D() {
	//----------------1.2D Test fft computer -----------//
	// Set input size
	int size = 5;
	MKL_LONG sizeN[2] = { size, size };
	const int dataSize = size * size;
	// Allocate memory for input data
	float* pfInputData2D = new float[dataSize]; float* pfFFTW3ReData2D = new float[dataSize]; float* pfMKLReData2D = new float[dataSize];
	fftwf_complex* pfOutputData1DFFTW3 = new fftwf_complex[dataSize];
	MKL_Complex8* pfOutputData1DMKL = new MKL_Complex8[size * size];
	memset(pfInputData2D, 0, sizeof(float) * dataSize);
	pfInputData2D[0] = 1; pfInputData2D[2] = 2;

	DFTI_DESCRIPTOR_HANDLE handleMKL = NULL;
	DftiCreateDescriptor(&handleMKL, DFTI_SINGLE, DFTI_REAL, 2, sizeN);
	auto status = DftiSetValue(handleMKL, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
	status = DftiSetValue(handleMKL, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
	status = DftiCommitDescriptor(handleMKL);

	// Compute FFT and reFFT using oneMKL
	DftiComputeForward(handleMKL, pfInputData2D, pfOutputData1DMKL);
	DftiComputeBackward(handleMKL, pfOutputData1DMKL, pfMKLReData2D);
	DftiFreeDescriptor(&handleMKL);

	// Compute FFT and reFFT using fftw3
	fftwf_plan forwardPlan = fftwf_plan_dft_r2c_2d(size, size, pfInputData2D, pfOutputData1DFFTW3, FFTW_ESTIMATE);
	fftwf_plan backwardPlan = fftwf_plan_dft_c2r_2d(size, size, pfOutputData1DFFTW3, pfFFTW3ReData2D, FFTW_ESTIMATE);
	fftwf_execute(forwardPlan);
	fftwf_execute(backwardPlan);
	fftwf_destroy_plan(forwardPlan);
	fftwf_destroy_plan(backwardPlan);

	//pfFFTW3ReData1D=pfFFTW3ReData2D/dataSize; //伪代码反变换尺度回复需要处于采样点数
	//pfMKLReData1D=pfMKLReData2D/dataSize;.
	// fftw3和MKL fft 结果位置不同。fftw3 连续存储；MKL 非连续存储；

	//printf
	std::cout << "Input data:" << std::endl;
	printf2D(pfInputData2D, size, size);
	std::cout << "FFTW3	FFT result:" << std::endl;
	printf2D(pfOutputData1DFFTW3, size, size);
	std::cout << "MKL FFT result:" << std::endl;
	printf2D(pfOutputData1DMKL, size, size);
	std::cout << "FFTW3	reFFT result:" << std::endl;
	printf2D(pfFFTW3ReData2D, size, size);
	std::cout << "MKL	reFFT result:" << std::endl;
	printf2D(pfMKLReData2D, size, size);

	// Cleanup
	delete[] pfInputData2D; delete[] pfFFTW3ReData2D; delete[] pfMKLReData2D;
	delete[] pfOutputData1DFFTW3;
	delete[] pfOutputData1DMKL;
	return true;
}

int main() {
	if (FFTTEST)
	{
		//----------------1.1D Test fft computer is correct-----------//
		fftTest1D();
		//----------------1.2D Test fft computer -----------//
		fftTest2D();
	}
	//----------------2.Initialize input data -----------//
	// Set input size
	int size = 2048;
	MKL_LONG sizeN[2] = { size, size };
	const int dataSize = size * size;
	// Allocate memory for input data
	float* pfInputData = new float[size * size];
	fftwf_complex* pFftFFTW3 = new fftwf_complex[dataSize];
	MKL_Complex8* pFftMKL = new MKL_Complex8[size * size];

	//-----------2. Initialize input data with random values---------------//
	VSLStreamStatePtr stream;
	vslNewStream(&stream, VSL_BRNG_MT19937, 1);
	vsRngUniform(VSL_RNG_METHOD_UNIFORM_STD, stream, size * size, pfInputData, 0, 1);
	vslDeleteStream(&stream);
	std::cout << "Initialize input data with random values.\nFirt value : " << pfInputData[0] << " and end value: " << pfInputData[dataSize - 1] << std::endl;

	//-----------3. Use fftw3 to compute real2complex fft---------------//
	auto start = std::chrono::high_resolution_clock::now();
	// Create plans for forward and backward FFT
	fftwf_plan forwardPlan = fftwf_plan_dft_r2c_2d(size, size, pfInputData, pFftFFTW3, FFTW_ESTIMATE);
	fftwf_execute(forwardPlan);
	auto end = std::chrono::high_resolution_clock::now();
	double totalTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	double referenceTime = totalTime;

	//-----------4. Use oneMKL API to compute real2complex fft---------------//
	start = std::chrono::high_resolution_clock::now();
	DFTI_DESCRIPTOR_HANDLE handleMKL = NULL;
	DftiCreateDescriptor(&handleMKL, DFTI_SINGLE, DFTI_REAL, 2, sizeN);
	auto status = DftiSetValue(handleMKL, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
	status = DftiSetValue(handleMKL, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
	status = DftiCommitDescriptor(handleMKL);
	// Compute FFT using oneMKL
	DftiComputeForward(handleMKL, pfInputData, pFftMKL);
	end = std::chrono::high_resolution_clock::now();
	totalTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	double averageTime = totalTime;
	std::cout << "fftw3 FFT one-time execution time: " << referenceTime << " microseconds." << std::endl;
	std::cout << "oneMKL FFT one-time execution time: " << averageTime << " microseconds." << std::endl;

	//-----------5. Compare fftw3 and oneMKL API FFT results---------------//
	bool resultsMatch = compareResults(pFftFFTW3, pFftMKL, sizeN);
	if (resultsMatch) {
		std::cout << "Results are correct." << std::endl;
	}
	else {
		std::cout << "Results are incorrect." << std::endl;
	}

	//-----------6. Compare fftw3 and oneMKL API FFT average performance---------------//
	int numRuns = 1000;
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numRuns; i++) {
		fftwf_execute(forwardPlan);
	}
	end = std::chrono::high_resolution_clock::now();
	totalTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	referenceTime = totalTime / numRuns;

	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numRuns; i++) {
		DftiComputeForward(handleMKL, pfInputData, pFftMKL);
	}
	end = std::chrono::high_resolution_clock::now();
	totalTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	averageTime = totalTime / numRuns;

	// Print performance results
	std::cout << "fftw3 FFT " << numRuns << " times average execution time: " << referenceTime << " microseconds." << std::endl;
	std::cout << "oneMKL FFT" << numRuns << " times average execution time: " << averageTime << " microseconds." << std::endl;

	// Cleanup
	DftiFreeDescriptor(&handleMKL);
	fftwf_destroy_plan(forwardPlan);
	delete[] pfInputData;
	delete[] pFftFFTW3;
	delete[] pFftMKL;
	system("pause");
	return 0;
}
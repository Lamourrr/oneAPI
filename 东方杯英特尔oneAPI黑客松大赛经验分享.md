

@[TOC](文章目录)

---

#  前言
**oneMKL**是英特尔推出的一款数学核心库，旨在提供高性能的数学函数和操作，以加速数据科学、科学计算、机器学习等领域的应用程序。它是英特尔oneAPI跨架构开发工具套件的一部分，支持在不同硬件加速器（如CPU、GPU、FPGA等）上实现高性能的数学计算。
本次“**东方杯英特尔oneAPI黑客松大赛**”赛题为使用oneMKL工具，对FFT算法进行加速与优化，下面是对于本次赛题以及oneMKL的使用经验分享。



# 一、题目要求
 1. 配置oneMKL环境
 2. 调用oneMKL 相应 API函数，产生 **2048 * 2048** 个 随机**单精度实数**
 3.  根据2产生的随机数据作为输入，实现两维Real to complex FFT参考代码
 4. 根据2产生的随机数据作为输入，调用 oneMKL API 计算两维Real to complex FFT
 5. 结果正确性验证，对3和4计算的两维FFT输出数据进行全数据比对（允许适当精度误差）， 输出 “结果正确”或“结果不正确”信息
 6. 平均性能数据比对（比如运行1000次），输出FFT参考代码平均运行时间和oneMKL FFT平均运行时间

# 二、配置环境
## 1.下载安装VS2022
在[VS官网](https://visualstudio.microsoft.com/zh-hans/downloads/)下载安装VS2022
## 2.下载安装intel oneMKL

进入[intel oneMKL官网](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html#gs.4gen78)下载安装oneMKL
![在这里插入图片描述](https://img-blog.csdnimg.cn/e8d396dc21dd44a1854f3cc4b0cbbbb1.png#pic_center)

选择操作系统和发行版本（我们使用的是Windows系统）
![在这里插入图片描述](https://img-blog.csdnimg.cn/f17e64e3308b42bfb9cea657b0e57060.png#pic_center)

不想注册可选以下选项直接下载
![在这里插入图片描述](https://img-blog.csdnimg.cn/055ed4f017d748e48bdd714b1bc5b516.png#pic_center)

下载完打开安装包，点击Extrat安装product package
![在这里插入图片描述](https://img-blog.csdnimg.cn/e3f72fe01ed6465bb762db4992ba5482.png#pic_center)

选Continue进行oneMKL安装
![在这里插入图片描述](https://img-blog.csdnimg.cn/fcfb49c635de45b59994dce229c431cc.png#pic_center)

点击Customize以定义安装位置
![在这里插入图片描述](https://img-blog.csdnimg.cn/a068a3e438a1408e9c54decb1946f78e.png#pic_center)

左下角选择安装位置（**一定要记住你的安装位置，复制下来后面配置要用**）
![在这里插入图片描述](https://img-blog.csdnimg.cn/afb14aa2188842ae928e0d3be5b7f49d.png#pic_center)

选择MKL库和VS版本安装
![在这里插入图片描述](https://img-blog.csdnimg.cn/79d042efd64f4252967dfbc33ea9593d.png#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/aacaf96d5df44bdc97aac449ed21c941.png#pic_center)
![在这里插入图片描述](https://img-blog.csdnimg.cn/7d8f23b780fd4cfb861b42effaa9ef30.png#pic_center)



点击Finish安装完成
![在这里插入图片描述](https://img-blog.csdnimg.cn/3c43731e63d741e7a807eace1b47c16e.png#pic_center)

## 3. 配置VS2022的环境
打开 Visual Studio2022，打开属性管理器。（属性管理器在视图-其他窗口）。打开后右上角出现下图
![在这里插入图片描述](https://img-blog.csdnimg.cn/7fc7fe10044040a0877388c0a85387f9.png#pic_center)

鼠标右键Debug | x64添加新项目属性表
![在这里插入图片描述](https://img-blog.csdnimg.cn/c88a52b899d04d78960f7649a547a7e5.png#pic_center)

双击新建的MKL属性表，打开属性
![在这里插入图片描述](https://img-blog.csdnimg.cn/fa3bbf3954894a6b811ab6ef9f82a69a.png#pic_center)

在 Intel Libraries for oneAPI 中把 Use oneMKL 设置成 Parallel
![在这里插入图片描述](https://img-blog.csdnimg.cn/f3cde71a91324aab9b6e079a48ec6fa7.png#pic_center)

在 VC++目录中进行三项设置（**具体根据你自己安装oneMKL的位置来设置，上面复制有可直接粘贴**）

 1. 可执行文件目录：(安装位置)\mkl\2023.1.0\bin\intel64 
 2. 包含目录：(安装位置)\mkl\2023.1.0\include
 3. 库目录(两个)：(安装位置)\mkl\2023.1.0\lib\intel64
(安装位置)\compiler\2023.1.0\windows\compiler\lib\intel64_win
![在这里插入图片描述](https://img-blog.csdnimg.cn/967556e1db36452799871c52724f046b.png#pic_center)


在**链接器-输入中添加附加依赖项**，根据电脑不同配置，选择不同依赖项。（我们用的是64位，Link： mkl_intel_ilp64.lib mkl_intel_thread.lib mkl_core.lib libiomp5md.lib）
选择地址：[https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-link-line-advisor.html#gs.4geua1](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-link-line-advisor.html#gs.4geua1)
![在这里插入图片描述](https://img-blog.csdnimg.cn/aa968cf2167447b49300494417a97b2c.png#pic_center)

在Windows系统搜索框中搜索 “编辑系统环境变量”，打开，弹出系统属性
![在这里插入图片描述](https://img-blog.csdnimg.cn/1885098f5c1c4c00a81ca3b62d617370.png#pic_center)

点击环境变量，在系统变量中，选择 Path 变量，双击进入编辑环境变量，点击新建，将路径：(安装oneMKL位置)\mkl\2023.1.0\redist\intel64 添加入系统变量
![在这里插入图片描述](https://img-blog.csdnimg.cn/04e603dc22224136befc93150c1a2e06.png#pic_center)

**到此，环境配置完成。**
参考文章：[https://blog.csdn.net/m0_63111108/article/details/124734432](https://blog.csdn.net/m0_63111108/article/details/124734432)

# 三、程序实现
## 1.随机数生成
随机数生成器类型：**VSL_BRNG_MT19937**
**VSL_BRNG_MT19937** 用于生成基于 **Mersenne Twister** 算法的随机数序列。通过选择不同的种子值，可以生成不同的随机数序列。
```c
float Radomnumber(float* arr, int SEED)
{
	//mkl生成随机数
	VSLStreamStatePtr stream; // 随机数流对象
	// 初始化随机数流对象
	vslNewStream(&stream, BRNG, SEED);
	// 生成随机数并存储到矩阵中
	vsRngUniform(VSL_RNG_METHOD_UNIFORMBITS32_STD, stream, n * n, arr, 0.0, 1.0);
	return (*arr);
	free(arr);
	vslDeleteStream(&stream);
}
```

## 2.fftw3计算两维Real to complex

```c
//fftw进行FFT
fftwf_complex* out = (fftwf_complex*)fftw_malloc(sizeof(fftwf_complex) * n * (n / 2 + 1));
fftwf_plan p = fftwf_plan_dft_r2c_2d(n, n, datain, out, FFTW_ESTIMATE);    // 创建变换方案
//计算运行时间
start1[i] = clock();
fftwf_execute(p);  // 执行变换
end1[i] = clock();
```

## 3.oneMKL FFT计算Real to complex

```c
//mkl进行FFT
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
//计算运行时间
start2[i] = clock();
status = DftiComputeForward(hand, datain, dataout);
end2[i] = clock();

status = DftiFreeDescriptor(&hand);
```
## 4.结果正确性验证

对fftw3与oneMKL FFT计算结果进行实部与虚部对比，计算两种方法做差所得到的绝对值是否满足所给misfit :1e-5
```c
//结果正确性验证
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
```
## 5.计算平均运行时间
**注意事项：以上两种方法均只对执行变换过程统计时间**
```c
T1 = temp1 / m;
printf("fftw3平均运行的时间是%f ms\n", T1);
T2 = temp2 / m;
printf("mkl平均运行的时间是%lf ms\n", T2);
```
## 6.销毁变换方案并释放内存
每次循环结束前及时销毁变换方案并释放内存
```c
// 销毁变换方案并释放内存空间
fftwf_destroy_plan(p);
fftwf_free(out);
mkl_free(dataout);
free(datain);
```

# 四、运行结果
**该运行结果为循环100次计算得到的平均运行时间**
可见对比误差在1e-5之内，oneMKL FFT算法运算结果是正确的，并且运行速度约为fftw3算法的3倍
![在这里插入图片描述](https://img-blog.csdnimg.cn/9c9ce9ffc69c45d8b0c100477f6ab05c.png#pic_center)

# 总结
**使用oneMKL FFT计算傅里叶变换的优点:**
 1. 高性能优化： oneMKL 中的 FFT 函数经过了高度优化，可以充分利用底层硬件资源，例如多核 CPU、GPU 等，以实现最佳性能。
 2. 支持多维和多通道数据： oneMKL 的 FFT 函数支持多维和多通道的数据变换。这对于处理图像、视频和其他多通道信号非常有用。
 3. 支持不同的数据精度： oneMKL 提供了多种数据精度的 FFT 函数
 4. 支持多种变换类型： oneMKL 可实现正向和反向变换，以及各种维度的变换。
**oneMKL工具对FFT算法的加速与优化效果明显，后续我也将运用oneMKL工具对实际数学工程问题进行加速与优化。**

#include <stdlib.h>

#include <math.h>


class Neuro_Net{
	public :
	int input;					//输入单元数
	int middle;					//中间单元数
	int output;					//输出单元数
	float *input_layer;			//输入层
	float *output_layer;		//输出层
	float *middle_layer;		//中间层
	float **middle_weigth;		//输入到中间层的连接权矩阵，是一个二维数组
	float **output_weigth;		//中间层到输出层的连接权矩阵，是一个二维数组
//	float (*tfun)(int n,float t);		//转换函数
	//bool (*ctrlIO)(Neuro_Net *n,bool IO,float * pIO);		//控制函数

	Neuro_Net(int iinput,int imiddle,int ioutput){
		//,bool (*ctrlIO)(Neuro_Net *n,bool IO,float * pIO)
	
		//input 输入单元数；middle 中间单元数；output 输出单元数；tfunction 转换函数
		this->input=iinput;
		this->middle=imiddle;
		this->output=ioutput;
		this->input_layer=new float [input];
		this->output_layer=new float [output];
		this->middle_layer=new float [middle];
		int i;
		this->middle_weigth=new float *[middle];
		for(i=0;i<middle;i++)
			this->middle_weigth[i]=new float[input];

		this->output_weigth=new float *[output];
		for(i=0;i<output;i++)
			this->output_weigth[i]=new float[middle];
		//this->tfun=tfunction;
		//this->ctrlIO=ctrlIO;
		initnet();
		
	}
	float tsfun(int n,float t){
		return tanh(2*t/n);
	};
	void initnet()
	//初设化，把所有权限设为在[-1,1]的随机值
	{
		int i,j;
		
		for ( i = 0; i < this->middle; i++ )
			for(j=0;j<this->input;j++)
				this->middle_weigth[i][j]=-1+2*(float)(rand() / (RAND_MAX + 1));//u [-1,1);
		for ( i = 0; i < this->output; i++ )
			for(j=0;j<this->middle;j++)
				this->output_weigth[i][j]=-1+2*(float)(rand() / (RAND_MAX + 1));//u [-1,1);

	}

	void cal()
	//计算
	{
		calmiddle();
		caloutput();

	}
private:
	void calmiddle()
	{
		int i,j;
		float t;
		for(i=0;i<this->middle;i++)
		{
			t=0;
			for(j=0;j<this->input;j++)
			{
				t+=this->middle_weigth[i][j]*this->input_layer[j];
			}
			this->middle_layer[i]=tsfun(this->input,t);
		}
	}
	void caloutput()
	{
		int i,j;
		float t;
		for(i=0;i<this->output;i++)
		{
			t=0;
			for(j=0;j<this->middle;j++)
			{
				t+=this->output_weigth[i][j]*this->middle_layer[j];
			}
			this->output_layer[i]=tsfun(this->middle,t);
		}
	}
};

class GA{
	public:
	Neuro_Net * nn;		//进行遗传算法的神经网络数组指针，一维
	int nnum;			//
	int g;
	int times;
	int time;//单位为毫秒
	float ch_rate;
	float slct_rate;
	float cb_rate;
	float *fit;
	//void (*ffun)(GA *ga,int i);
	//bool (*controlfun)(GA *ga,int i);
	int gen;

	class Nfit{
	public:
		int i;//按照fit排序，i为nn中的序号
		float fit;
	};

	Nfit * nnfit;


	GA( Neuro_Net *inn,int innum,int ig,
		//int itimes
		//,int time
		float islct_rate,
		float ich_rate,float icb_rate
		//,void (*ffun)(GA *ga,int i),bool (*callback)(GA *ga,int j)
		)
		//nn Neuro_Net的一维指针，nnum Neuro_Net的数量，g GA次数，itimes控制次数（10000），time 每次控制时间间隔，slct_rate 选择率（不变率0.05）cb_rate杂交率（0.90） ch_rate 重新生成率（0.05），  
		//ffun 适应值函数；callback 输入输出函数
	{
		this->nn=inn;
		this->nnum=innum;
		this->g=ig;
		//this->times=itimes;
		this->ch_rate=ich_rate;
		this->slct_rate=islct_rate;
		this->cb_rate=icb_rate;
		//this->time=time;
		//this->ffun=ffun;
		//this->controlfun=callback;
		this->fit=new float[nnum];
		this->nnfit=new Nfit[nnum];	
		this->gen=0;
	}

	void generation()
	{
		
		if(this->gen<this->g)
		{

			//control();
			sort();
			gafun();
			this->gen++;
			//return false;	//
		}else{
			//return true;	//
		}
	}

	private:	
	void sort(){
		for(int i=0;i<this->nnum;i++){
			this->nnfit[i].i=i;
			this->nnfit[i].fit=this->fit[i];
		}
		QuickSort(this->nnfit,0,this->nnum);
	}
	void gafun(){
		select();
		combin();
		reborn();

	}

	void select(){
	}
	void combin(){
		int x,y,w;
		float z;

		for(x=this->nnum*this->slct_rate;x<this->nnum*this->cb_rate;x++)
		{
			y=rand()%(int)(this->slct_rate*this->nnum);
			z=rand() / (RAND_MAX + 1);
			w=this->nn[x].middle*z;
			if(z>0.5)
				copyw(this->nn[y].middle_weigth,this->nn[x].middle_weigth,w,this->nn[y].input);
			else
				copyw(this->nn[y].middle_weigth+w,this->nn[x].middle_weigth+w,this->nn[y].middle-w,this->nn[y].input);
			
			z=rand() / (RAND_MAX + 1);
			w=this->nn[x].output*z;
			if(z>0.5)
				copyw(this->nn[y].output_weigth,this->nn[x].output_weigth,w,this->nn[y].middle);
			else
				copyw(this->nn[y].output_weigth+w,this->nn[x].output_weigth+w,this->nn[y].output-w,this->nn[y].middle);

		}
	}
	void copyw(float ** wsrc,float ** wtrg,int i,int j ){
		int x,y;
		for(x=0;x<i;x++)
			for(y=0;y<j;y++)
				wtrg[x][y]=wsrc[x][y];

	}
	void reborn(){
		int x;
		
		for(x=this->nnum*this->ch_rate;x<this->nnum;x++){
			this->nn[x].initnet();
		}
	}

	int Partition(Nfit array[], int low, int high)
	{
		float pivot = array[low].fit;
		while (low < high)
		{
			while (low < high && array[high].fit <= pivot)
			{
				--high;
			}
			Swap(&array[low], &array[high]);
			while (low < high && array[low].fit >= pivot)
			{
				++low;
			}
			Swap(&array[low], &array[high]);
		}
		return low;
	}
	void QuickSort(Nfit array[], int low, int high)
	{
		if (low < high)
		{
			int n = Partition(array, low, high);
			QuickSort(array, low, n);
			QuickSort(array, n + 1, high);
		}
	}
	void Swap(Nfit * a, Nfit *b ){
		Nfit * c= new Nfit();
		c->fit=a->fit;
		c->i=a->i;
		a->fit=b->fit;
		a->i=b->i;
		b->fit=c->fit;
		b->i=c->i;
		delete c;
	}

};
#include <stdio.h>
#include <iostream>  
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include <cstdio> 

using namespace std;
static int map_rate;

class Map
{
	private:
		int lines;
		int originalX;	//机器人初始x
		int originalY;	//机器人初始y
		int originalD;	//机器人初始方向
		int lightNum;
		vector<int> lightX;
		vector<int> lightY;
		
	public:
		int rX;	//机器人x
		int rY;	//机器人y
		int rD;	//机器人方向
		vector<int> isOn;
		string mapName;
		inline int getLines(){return this->lines;}
		inline int getLightX(int index){return this->lightX[index];}
		inline int getLightY(int index){return this->lightY[index];}
		inline int getIsOn(int index){return this->isOn[index];}
		inline int getOriginalX(){return this->originalX;}
		inline int getOriginalY(){return this->originalY;}
		inline int getOriginalD(){return this->originalD;}
		inline int getLightNum(){return this->lightNum;}
		inline void set_rX(int x){rX=x;}
		inline void set_rY(int y){rY=y;}
		inline void set_rD(int d){rD=d;}
		inline void set_lightOn(int index){isOn[index]=1;}

		int **readMap(string mapName){
			this->mapName=mapName+".txt";
			const char *name=this->mapName.c_str();
			ifstream in;
			in.open(name);
			//cout<<"name is"<<name;
			if(!in){  
				cout<<"地图不存在"<<endl;
				return NULL;
			}
			string firstLine;
			getline(in,firstLine);
			
			sscanf(firstLine.c_str(),"%d",&lines);	//读入地图行数
			//cout<<lines<<endl;
			int map[lines][lines];
			string buf;
			int m=0;
			int a;
			while(getline(in,buf)){		
				if(m<lines){				//读入地图内容
					istringstream is(buf);
					int n=0;
					while(is>>a){
						map[m][n]=a;
						n++;
					}
				}
				else if(m==lines)			//读入机器人初始坐标
					sscanf(buf.c_str(),"%d %d %d",&originalX,&originalY,&originalD);
				else if(m==lines+1)			//读入灯数
					sscanf(buf.c_str(),"%d",&lightNum);
				else if(m>=lines+2){		//读入灯坐标
					int x,y;
					sscanf(buf.c_str(),"%d %d",&x,&y);
					lightX.push_back(x);
					lightY.push_back(y);
					isOn.push_back(0);
				}
				m++;
			}
			robotInit();
			int **mapPtr=new int*[lines];  
			int n=0;  
			for (int i=0;i<lines;i++)  
			{  
				mapPtr[i] = new int[lines];  
				for(int j=0;j<lines;j++)  
					mapPtr[i][j]=map[i][j];  
			}  
			return mapPtr;
		}
		
		inline void robotInit(){
			this->rX = this->originalX;
			this->rY = this->originalY;
			this->rD = this->originalD;
		}
};

class Play
{
	Map map;
	int **p;
	int mark;	//合法操作：1   非法操作：0
	inline int checkLight(int x,int y){
		for(int i=0;i<map.getLightNum();i++)
			if(x==map.getLightX(i)&&y==map.getLightY(i))
				return i;
		return -1;	//(x,y) is not a light
	}
	public:
	Play(Map map,int **p){
		this->map=map;
		this->p=p;
	}

	inline string robotDirection(){
		switch (map.rD)
		{
			case 0:return "↑";
			case 1:return "←";
			case 2:return "↓";
			case 3:return "→";
			default:return NULL;
		}
	}
	inline int robotTurnLeft(){return (map.rD+1)%4;}
	inline int robotTurnRight(){return (map.rD+3)%4;}
	inline void turnLeft(){map.set_rD(robotTurnLeft());}
	inline void turnRight(){map.set_rD(robotTurnRight());}
	inline int checkGo(int x,int y){
		if(x<0||y<0||x>=map.getLines()||y>=map.getLines()){
			cout<<"超出地图范围"<<endl;
			return 0;
		}
		else if(p[map.rX][map.rY]>=p[x][y]){
			mark=1;
			return 1;	//可以走
		}
		else{
			cout<<"只能走到高度不大于当前位置的格子"<<endl;
			return 0;	//不可以走
		}
	}
	inline void goStraight(){
		switch (map.rD)
		{
			case 1:
				if(checkGo(map.rX,map.rY-1)==1)
					map.set_rY(map.rY-1);
				break;
			case 0:
				if(checkGo(map.rX-1,map.rY)==1)
					map.set_rX(map.rX-1);
				break;
			case 3:
				if(checkGo(map.rX,map.rY+1)==1)
					map.set_rY(map.rY+1);
				break;
			case 2:
				if(checkGo(map.rX+1,map.rY)==1)
					map.set_rX(map.rX+1);
				break;
			default:
				cout<<"方向错误"<<endl;
		}
	}
	inline int checkJump(int x,int y){
		if(x<0||y<0||x>=map.getLines()||y>=map.getLines()){
			cout<<"超出地图范围"<<endl;
			return 0;
		}
		else if(p[map.rX][map.rY]+1>=p[x][y]){
			mark=1;
			return 1;	//可以跳
		}
		else{
			cout<<"至多能跳到比当前高1格的位置"<<endl;
			return 0;	//不可以跳
		}
	}
	inline void jump(){
		switch (map.rD)
		{
			case 1:
				if(checkJump(map.rX,map.rY-1)==1)
					map.set_rY(map.rY-1);
				break;
			case 0:
				if(checkJump(map.rX-1,map.rY)==1)
					map.set_rX(map.rX-1);
				break;
			case 3:
				if(checkJump(map.rX,map.rY+1)==1)
					map.set_rY(map.rY+1);
				break;
			case 2:
				if(checkJump(map.rX+1,map.rY)==1)
					map.set_rX(map.rX+1);
				break;
			default:
				cout<<"方向错误"<<endl;
		}
	}
	inline void lightOn(){
		int isLight=checkLight(map.rX,map.rY);
		if(isLight!=-1){
			map.set_lightOn(isLight);
		}
		else{
			cout<<"当前位置没有灯"<<endl;
			mark=-1;
		}		
	}

	inline int checkLight(){
		for(int i=0;i<map.getLightNum();i++){
			if(map.isOn[i]==0){
				return 0;
			}
		}
		cout<<"游戏胜利！"<<endl;
		return 1;
	}

	void file(time_t start,int c){		//存档
		time_t stop;
		stop=time(NULL);
		long t=stop-start;
		cout<<"请输入文件名"<<endl;
		string fileName;
		cin>>fileName;
		string fName;
		fName=fileName+".file";
		const char *name=fName.c_str();
		ofstream out;
		out.open(name);
		//记录难度等级
		if(c==0)	//已有地图
			out<<0<<endl;
		else		//随机地图
			out<<c<<endl;
		out<<map.mapName.substr(0,map.mapName.length()-4)<<endl;
		out<<map.rX<<endl<<map.rY<<endl<<map.rD<<endl;
		out<<t<<endl;
		for(int i=0;i<map.getLightNum();i++){
			if(map.isOn[i]==1)
				out<<i<<endl;
		}
		
	} 
	
	void handle(time_t start,int rate){
		mark=0;
		cout<<"请输入操作：l-左转，r-右转，s-直走，j-跳跃，o-点灯，f：存档，q：退出(支持连续输入，命令间不需要加空格)"<<endl;
		string inStr;
		cin>>inStr;
		int len=inStr.length();
		char charArr[len];
		strcpy(charArr,inStr.c_str());
		for(int i=0;i<len;i++)
		{
			switch (charArr[i])
			{
			case 'l':
				turnLeft();
				mark=1;
				break;
			case 'r':
				turnRight();
				mark=1;
				break;
			case 's':
				goStraight();
				break;
			case 'j':
				jump();
				mark=1;
				break;
			case 'o':
				lightOn();
				mark=1;
				break;
			case 'f':
				file(start,rate);
				break;
			case 'q':
				exit(0);
			default:
				cout<<charArr[i]<<"不是合法输入"<<endl;
				break;
			}
		}
		if(mark!=0)
			printMap();
	}
	void printMap(){
		//this->map=map;
		//cout<<map.getLines()<<endl;
		for(int j=0;j<map.getLines();j++)
			cout<<"______";
		cout<<endl;
		for(int i=0;i<map.getLines();i++){
			for(int j=0;j<map.getLines();j++)
				cout<<"|"<<"     ";
			cout<<"|"<<endl;
			for(int j=0;j<map.getLines();j++){
				if(checkLight(i,j)==-1){	//(x,y) is not a light
					if((i!=map.rX)||(j!=map.rY)){
						if(p[i][j]!=0)
							cout<<"|"<<"  "<<p[i][j]<<"  ";
						else
							cout<<"|"<<"     ";
					}
					else{
						if(p[i][j]!=0)
							cout<<"|"<<"  "<<p[i][j]<<robotDirection();
						else
							cout<<"|"<<"   "<<robotDirection();
					}
				}
				else{			//(x,y) is a light
					string lightSymbol;
					if(map.getIsOn(checkLight(i,j))==1)
						lightSymbol="O";
					if(map.getIsOn(checkLight(i,j))==0)
						lightSymbol="_";
					if((i!=map.rX)||(j!=map.rY)){
						if(p[i][j]!=0)
							cout<<"|"<<" "<<lightSymbol<<p[i][j]<<"  ";
						else
							cout<<"|"<<" "<<lightSymbol<<"   ";
					}
					else{
						if(p[i][j]!=0)
							cout<<"|"<<" "<<lightSymbol<<p[i][j]<<robotDirection();
						else
							cout<<"|"<<" "<<lightSymbol<<" "<<robotDirection();
					}
				}
			}
			cout<<"|"<<endl;
			for(int j=0;j<map.getLines();j++)
				cout<<"|"<<"_____";
			cout<<"|"<<endl;
		}
	}
};

class RandomMap
{
	private:
	int lightNum;
	vector<int> lightX;
	vector<int> lightY;
	vector<int> lightH;
	int lines;
	string name;
	int rX,rY,rD;
	int rate;
	public:
	RandomMap(string rName){
		srand((int)time(0));
		lines=rand()%7+8;
		//cout<<lines<<endl;
		name=rName;
	}
	inline void setRate(int num){rate=num;}
	inline void setLightNum(int num){lightNum=num;}
	void setLight(int h){
		int x;
		int y;
		for(int i=0;i<lightNum;i++){
			x=rand()%(lines-2)+1;
			y=rand()%(lines-2)+1;
			for(int j=0;j<i;j++){
				while((lightX[j]==x)&&(lightY[j]==y)){
					x=rand()%(lines-2)+1;
					y=rand()%(lines-2)+1;
				}
			}
			lightX.push_back(x);
			lightY.push_back(y);
			lightH.push_back(h);
		}
	}
	inline void setRobot(){
		rX=rand()%lines;
		rY=rand()%lines;
		rD=rand()%4;
	}
	
	void init(int num){
		setRate(num);
		setLightNum(num);
		setRobot();
		setLight(rate);
	}
	inline int isLight(int x,int y){
		for(int i=0;i<lightNum;i++){
			if(x==lightX[i]&&y==lightY[i])
				return 1;
		}
		return 0;
	}
	void writeFile(){		//写随机地图
		int map[lines][lines];
		for(int i=0;i<lines;i++){
			for(int j=0;j<lines;j++){
				map[i][j]=0;
			}
		}
		for(int i=0;i<lightNum;i++){
			int x=lightX[i];
			int y=lightY[i];
			map[x][y]=lightH[i];
			if(rate==1)
				continue;
			else if(rate==2){
				int r1=rand()%4;
				int r2=rand()%4;
				while(r2==r1)
					r2=rand()%4;
				switch (r1)
				{
				case 0:
					map[x-1][y]=lightH[i]-1;
					break;
				case 1:
					map[x+1][y]=lightH[i]-1;
					break;
				case 2:
					map[x][y-1]=lightH[i]-1;
					break;
				case 3:
					map[x][y+1]=lightH[i]-1;
					break;
				default:
					break;
				}				
				switch (r2)
				{
				case 0:
					map[x-1][y]=lightH[i]-1;
					if(isLight(x-1,y)==0)
						break;
				case 1:
					map[x+1][y]=lightH[i]-1;
					if(isLight(x+1,y)==0)
						break;
				case 2:
					map[x][y-1]=lightH[i]-1;
					if(isLight(x,y-1)==0)
						break;
				case 3:
					map[x][y+1]=lightH[i]-1;
					if(isLight(x,y+1)==0)
						break;
				default:
					map[x-1][y]=lightH[i]-1;
					break;
				}
			}
			else if(rate==3){
				int r=rand()%4;
				switch (r)
				{
				case 0:
					map[x-1][y]=lightH[i]-1;
					map[x-1][y-1]=lightH[i]-2;
					if(isLight(x-1,y-1)==0&&isLight(x-1,y)==0)
						break;
				case 1:
					map[x+1][y]=lightH[i]-1;
					map[x+1][y+1]=lightH[i]-2;
					if(isLight(x+1,y)==0&&isLight(x+1,y+1)==0)
						break;
				case 2:
					map[x][y-1]=lightH[i]-1;
					map[x-1][y-1]=lightH[i]-2;
					if(isLight(x,y-1)==0&&isLight(x-1,y-1)==0)
						break;
				case 3:
					map[x][y+1]=lightH[i]-1;
					map[x+1][y+1]=lightH[i]-2;
					if(isLight(x,y+1)==0&&isLight(x+1,y+1)==0)
						break;
				default:
					map[x-1][y]=lightH[i]-1;
					map[x-1][y-1]=lightH[i]-2;
					break;
				}
			}
		}
		/*for(int i=0;i<lines;i++){
			for(int j=0;j<lines;j++){
	cout<<map[i][j];
			}
			cout<<endl;
		}*/
		string fName=name+".txt";
		const char *n=fName.c_str();
		ofstream out;
		out.open(n);
		out<<lines<<endl;
		for(int i=0;i<lines;i++){
			for(int j=0;j<lines;j++){
				out<<map[i][j]<<" ";
			}
			out<<endl;
		}
		out<<rX<<" "<<rY<<" "<<rD<<endl;
		out<<lightNum<<endl;
		for(int i=0;i<lightNum;i++)
			out<<lightX[i]<<" "<<lightY[i]<<endl;
		
	}
};

int **loadFile(Map* map,long long* ori_time){
	
	cout<<"请输入要读取的文件名"<<endl;
	string fileName;
	cin>>fileName;
	string fName;
	fName=fileName+".file";
	const char *name=fName.c_str();
	ifstream in;
	in.open(name);
	if(!in){  
		cout<<"存档不存在"<<endl;
		return NULL;
	}
	string mapName;
	int **p;
	
	
	//const char *oriName=firstLine.c_str();
	int robotX,robotY,robotD,rate;
	string buf;
	char *r,*x,*y,*d,*t,*temp;
	vector<int> light;
	getline(in,buf);
	r = new char[buf.length()+1];
	strcpy(r,buf.c_str());
	rate=atoi(r);
	map_rate=rate;
	getline(in,mapName);
	getline(in,buf);
	x = new char[buf.length()+1];
	strcpy(x,buf.c_str());
	robotX=atoi(x);
	getline(in,buf);
	y = new char[buf.length()+1];
	strcpy(y,buf.c_str());
	robotY=atoi(y);
	getline(in,buf);
	d = new char[buf.length()+1];
	strcpy(d,buf.c_str());
	robotD=atoi(d);
	getline(in,buf);
	t = new char[buf.length()+1];
	strcpy(t,buf.c_str());
	*ori_time=atoi(t);
	while(getline(in,buf)){
		temp = new char[buf.length()+1];
		strcpy(temp,buf.c_str());
		int index=atoi(temp);
		light.push_back(index);
	}
	in.close();
	p=map->readMap(mapName);
	map->set_rX(robotX);
	map->set_rY(robotY);
	map->set_rD(robotD);
	for(int i=0;i<light.size();i++){
		map->isOn[light[i]]=1;
	}
	return p;
}

void record(int rate,int time){
	if(rate==0)
		return;
	int len;
	len=5;
	ifstream in;
	ofstream out;
	stringstream stream;  
    stream<<rate;  
    string s=stream.str();
	string name=s+".r";
	const char* fName=name.c_str();
	in.open(fName);
	string nickname;
	if(!in){
		cout<<"请输入昵称"<<endl;
		cin>>nickname;
		out.open(fName);
		out<<time<<" "<<nickname<<endl;
		cout<<endl;
		cout<<"排行榜"<<endl;
		cout<<1<<" "<<time<<"秒 "<<nickname<<endl;		//显示排行榜
		for(int i=0;i<len-1;i++){
			out<<99999<<" "<<"---"<<endl;
			cout<<i+2<<" "<<99999<<"秒 "<<"---"<<endl;		//显示排行榜
		}
		out.close();
		return;
	}
	string buf;
	int time_arr[len];
	string nickname_arr[len];
	int count=0;
	for(int i=0;i<len;i++){
		time_arr[i]=99999;
		nickname_arr[i]="---";
	}
	while(getline(in,buf)){
		int t;
		string n;
		istringstream is(buf);
		is>>t;
		is>>n;
		time_arr[count]=t;
		nickname_arr[count]=n;
		count++;
	}
	if(time>=time_arr[len-1])	//没有产生记录
		return;
	cout<<"请输入昵称"<<endl;
	cin>>nickname;
	out.open(fName);
	for(int i=len-2;i>=0;i--){
		if(time>=time_arr[i]){
			if(i<len-2){
				for(int j=len-1;j>=i+2;j--){
					time_arr[j]=time_arr[j-1];
					nickname_arr[j]=nickname_arr[j-1];
				}
			}
			time_arr[i+1]=time;
			nickname_arr[i+1]=nickname;
			break;
		}
	}
	if(time<time_arr[0]){
		for(int j=len-1;j>=1;j--){
			time_arr[j]=time_arr[j-1];
			nickname_arr[j]=nickname_arr[j-1];
		}
		time_arr[0]=time;
		nickname_arr[0]=nickname;
	}
	cout<<endl;
	cout<<"排行榜（难度：）"<<rate<<endl;
	for(int i=0;i<len;i++){
		out<<time_arr[i]<<" "<<nickname_arr[i]<<endl;		//写入文件
		cout<<i+1<<" "<<time_arr[i]<<"秒 "<<nickname_arr[i]<<endl;		//显示排行榜
	}

}

int main(int argc, char *argv[])
{
	string choice;
	while(choice!="n"&&choice!="l"&&choice!="v"){
		cout<<"请选择新游戏（n）或读档（l）或查看排行榜（v）"<<endl;
		cin>>choice;
	}
	Map map;
	int **p;
	time_t start,stop,t;
	long long ori_time;
	string c;
	int d;
	int loop=0;
	while(choice=="v"&&loop==0){
		int rate;
		while(loop==0){
			cout<<"请选择排行榜对应的难度,如要退出查看排行榜，请输入-1"<<endl;
			cin>>rate;
			if(rate==-1){
				loop=1;
				continue;
			}
			ifstream in;
			stringstream stream;  
			stream<<rate;  
			string s=stream.str();
			string name=s+".r";
			const char* fName=name.c_str();
			in.open(fName);
			string nickname;
			if(!in){
				cout<<"不存在该难度的排行榜，请重新输入"<<endl;
				continue;
			}
			string buf;
			cout<<"排行榜（难度：）"<<rate<<endl;
			while(getline(in,buf)){
				cout<<buf<<endl;
			}
			in.close();
			int rem;
			cout<<"如果要清除该排行榜，请输入1，否则输入0"<<endl;
			cin>>rem;
			if(rem==1){
				int i=remove(fName);
			}
			break;
		}
		if(loop==0){
			cout<<"请选择继续查看其他难度的排行榜（v）或开始游戏（任意其他输入）"<<endl;
			cin>>choice;
		}
	}
	while(choice!="n"&&choice!="l"){
		cout<<"请选择新游戏（n）或读档（l）"<<endl;
		cin>>choice;
	}
	if (choice=="n")
	{
		while(c!="e"&&c!="r"){
			cout<<"请选择已有地图(e)/随机地图(r)(随机地图可选择难度)"<<endl;
			cin>>c;
		}
		if(c=="e"){
			cout<<"请输入地图名"<<endl;
			string mapName;
			cin>>mapName;
			p=map.readMap(mapName);
			while(p==NULL){
				cout<<"请输入地图名"<<endl;
				cin>>mapName;
				p=map.readMap(mapName);
			}
			if(p==NULL)
				return -1;
		}
		else if(c=="r"){
			while(d!=1&&d!=2&&d!=3){
				if(cin.good()==1)
					cout<<"请选择难度(简单-1，普通-2，困难-3)"<<endl;
				cin>>d;
				map_rate=d;
			}
			t=time(NULL);
			ostringstream os;  
			os<<t;  
			string rName;
			istringstream is(os.str());  
			is>>rName; 
			RandomMap randomMap(rName);		//随机地图名为当前时间
			randomMap.init(d);
			randomMap.writeFile();
			p=map.readMap(rName);
		}
	}
	else if (choice=="l")
	{
		p=loadFile(&map,&ori_time);
		while(p==NULL)
			p=loadFile(&map,&ori_time);
	}
	start=time(NULL);
	Play play(map,p);
	play.printMap();
	while (play.checkLight()==0)	//灯没有被全部点亮，游戏继续
	{
		play.handle(start,d);
	}
	stop=time(NULL);
	int time;
	if(choice=="n")
		time=stop-start;
	else if(choice=="l")
		time=stop-start+ori_time;
	cout<<"您共计用时"<<time<<"秒"<<endl;
	if(c=="r"||map_rate!=0)
		record(map_rate,time);
	return 0;
}

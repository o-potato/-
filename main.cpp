#include<cmath>
#include<time.h>
#include<random>
#include"guests.h"
#include"restaurant.h"

using namespace std;

Time* startTime = new Time("11:00");	//开始营业的时间
Time* endTime = new Time("2:00");	//结束营业的时间

////帮助对顾客进行排序
//void swap(Customers* &c1, Customers* &c2) {
//	Customers* t;
//	t = c1;
//	c1 = c2;
//	c2 = t;
//}

int main() {
	srand(time(NULL));

	//随机生成顾客组数CusNum
	int CusNum = rand() % 200 + 101;	//保证顾客组数大于100，不超过300
	//int CusNum = 50;	//测试用

	//随机生成桌子情况，并写入input_table.txt
	ofstream genInputTable("input_table.txt");
	int flag[MaxCapacity + 1] = { 0 };		//flag[i] = 0表示没有i人桌，flag[i] = 1表示已有i人桌
	for (int i = 0; i < MaxCapacity; i++) {
		int a = rand() % MaxTableNum + 1;	//保证每种桌子数目不多于MaxTableNum，也大于0
		int b = rand() % MaxCapacity + 1;	//保证桌子容量大于0，且不多于MaxCapacity
		if (flag[b] == 0) {
			genInputTable << a << ' ' << b << endl;
			flag[b] = 1;
		}
	}
	if (flag[MaxCapacity] == 0)
		genInputTable << 1 << ' ' << MaxCapacity << endl;	//保证所有组都能有可以容纳的桌子
	genInputTable.close();

	//随机生成顾客情况，并写入input_customer.txt
	ofstream genInputCustomer("input_customer.txt");
	for (int i = 0; i < CusNum; i++) {
		int n = rand() % MaxCapacity + 1;		//保证就餐人数不超过最大桌子容量，且大于0
		int h = (rand() % 13 + 11) % 24;		//保证小时数在11--24之间，这里为了防止最后一组离开时刻超过第二天2:00
										//故将到达时间的下限设置的比较靠前
		int m = rand() % 60;
		int eatT = rand() % 21 + 10;	//保证用餐时间在10--30min
		string s1 = to_string(h) + ":" + to_string(m);
		string s2 = "0:" + to_string(eatT);
		genInputCustomer << n << ' ' << s1 << ' ' << s2 << endl;
	}
	genInputCustomer.close();

	//根据input_table.txt构造旦苑大饭店
	ifstream inputTable("input_table.txt");
	//inputTable.open("input_table.txt");
	vector<Table*> t;
	int a, b;
	while (inputTable >> a >> b) {	//a张b人桌
		while (a) {
			Table* tab = new Table(b);
			t.push_back(tab);
			a--;
		}
	}
	Restaurant* danyuanRes = new Restaurant(t);		//旦苑大饭店

	//根据input_customer.txt构造顾客对象
	//此时仅设置了用餐人数，用餐时间，到达时间
	ifstream inputCustomer("input_customer.txt");
	//inputCustomer.open("input_customer.txt");
	vector<Customers*> cus;
	int n;
	string s1, s2;
	while (inputCustomer >> n >> s1 >> s2) {	//就餐人数为n，到达时刻为s1(xx:xx)，就餐时间为s2(xx:xx)
		Time* t1, * t2;
		t1 = new Time(s1);
		t2 = new Time(s2);
		Customers* c = new Customers(n, t1, t2);
		cus.push_back(c);
	}
	
	//对顾客数组cus按时间先后排序，采用插入排序法
	for (int i = 0; i < cus.size(); i++) {
		int j = i;
		Customers* tmp = cus[i];
		Time* t_i = tmp->getArriveTime();
		while (j > 0) {
			Time* t_j;
			t_j = cus[j -1]->getArriveTime();
			int compT = cus[i]->getArriveTime()->comp(t_i, t_j);
			if (compT == Early) {
				cus[j] = cus[j - 1];
			}
			if (compT == Late) {
				cus[j] = tmp;
				break;
			}
			if (compT == Same) {
				if (tmp->getPersonNum() < cus[j - 1]->getPersonNum()) {
					cus[j] = cus[j - 1];
				}
				else {
						cus[j] = tmp;
						break;
				}
			}
			j--;
		}
		if (j == 0)
			cus[j] = tmp;
	}
	
	//对排好序的顾客数组cus按顺序设置ID
	//顾客ID从1开始
	for (int i = 0; i < cus.size(); i++) {
		cus[i]->setID(i + 1);
	}
	
	//安排每组顾客的就餐，在此过程中可获得每组顾客
	//的就坐时间，等待时间，离开时间
	Time* currentTime = startTime;	//用currentTime来记录当前时间点
	//Time* nextTime;		//用nextTime来标记下一个关键时间点：释放桌子，安排顾客
	int cus_num = 0;		//cus_num记录当前被安排的顾客序号
	Table* toFree;		//记录即将释放的桌子
	while (cus_num < cus.size()) {
		//首先根据时间判断当前是否有已到达顾客，若有已到达顾客，对当前顾客，
		//判断是否有可容纳的空闲桌子，若有桌子，则给当前顾客安排入座；否则，
		//按时间顺序释放桌子；若没有已到达顾客，按时间顺序释放桌子
		int cusWait = currentTime->comp(currentTime, cus[cus_num]->getArriveTime());	//cusWait记录当前顾客是否已经到达
		int capa = danyuanRes->nPersonTable(cus[cus_num]->getPersonNum());		//capa记录当前顾客能入座要求的桌子容量
		if (cusWait > Early) {	//当前时刻有已到达顾客
			if (capa > 0) {		//当前有能容纳这组顾客的桌子
				cus[cus_num]->setSettleDown(currentTime);	//cus[cus_num]在当前时刻就坐
				cus[cus_num]->setLeaveTime();
				cus[cus_num]->setWaitTime();
				danyuanRes->useTable(capa, cus[cus_num]);	//cus[cus_num]占用一张capa人桌
				cus_num++;
			}
			else {		//当前没有能容纳这组顾客的桌子，按时间顺序释放桌子
				toFree = danyuanRes->latestTable();
				currentTime = toFree->getFreeTime();
				danyuanRes->freeTable(toFree->getCapacity(), toFree);
			}
		}
		else {	//当前没有已经到达的顾客，检查触发下一关键时间点的事件是顾客到达还是顾客离开，
			//若为顾客到达，更新时间点为顾客到达时间；否则，按时间顺序释放桌子即可
			//释放桌子首先要检查是否有可释放桌子，即busyTables[]不为空；
			//若没有可释放桌子，说明当前时刻饭店处于空闲状态，应更新
			//currentTime为下一组顾客到来时间(不考虑因顾客人数超过桌子
			//最大容量而造成死锁）；若有可释放桌子，则利用latestTable()
			//获取最先被释放的桌子，并释放该桌子，同时要将currentTime更
			//新为释放该桌子的时间，并进入下一轮循环
			Time* nextArr = cus[cus_num]->getArriveTime();
			toFree = danyuanRes->latestTable();		//toFree == NULL说明没有可释放桌子
			if (toFree == NULL) {
				currentTime = cus[cus_num]->getArriveTime();
			}
			else {
				int k = nextArr->comp(nextArr, toFree->getFreeTime());
				if (k == Early)
					currentTime = cus[cus_num]->getArriveTime();
				else {
					currentTime = toFree->getFreeTime();
					danyuanRes->freeTable(toFree->getCapacity(), toFree);
				}
			}
		}
	}

	//获得最后一组顾客离开时间
	//若桌子全都空闲，说明所有顾客均已离开，否则，按时间顺序
	//释放所有桌子，最终得到的currentTime即为最后一组离开时间
	toFree = danyuanRes->latestTable();
	while (toFree != NULL) {
		currentTime = toFree->getFreeTime();
		danyuanRes->freeTable(toFree->getCapacity(), toFree);
		toFree = danyuanRes->latestTable();
	}

	int sumStayTime = 0;	//所有顾客总停留时间
	int sumWaitTime = 0;	//所有顾客总等候时间
	double aveStayTime;	//平均停留时间，精确到分钟
	double aveWaitTime;	//平均等候时间，精确到分钟
	for (int i = 0; i < cus.size(); i++) {
		if (cus[i]->getLeaveTime()->getNextDay())	//超过00:00不被计入
			continue;
		sumStayTime = sumStayTime + cus[i]->getStayTime();
		sumWaitTime = sumWaitTime + cus[i]->getWaitTime();
	}
	aveStayTime = 1.0 * sumStayTime / cus.size();
	aveWaitTime = 1.0 * sumWaitTime / cus.size();

	ofstream outputData("output_data.txt");
	ofstream outputCustomer("output_customer.txt");

	//写入output_data.txt
	outputData << "平均停留时间：" << round(aveStayTime) << "min" << endl;
	outputData << "平均等候时间：" << round(aveWaitTime) << "min" << endl;
	outputData << "最后一组顾客离开时间：" << currentTime->Time2str() << endl;

	//写入output_customer.txt
	outputCustomer << "编号" << '\t'
		<< "顾客人数" << '\t'
		<< "到来时刻" << '\t'
		<< "等候用时" << '\t'
		<< "就餐时刻" << '\t'
		<< "就餐用时" << '\t'
		<< "离开时刻" << '\t' << endl;
	for (int i = 0; i < cus.size(); i++) {
		cus[i]->customerInfo(outputCustomer);
	}

	//关闭所有文件
	inputTable.close();
	inputCustomer.close();
	outputCustomer.close();
	outputData.close();

	return 0;
}
#pragma once
#include<fstream>
#include<iomanip>
#include<string>

using namespace std;

//用来表示时间早晚
enum TimeSeq
{
	Early, Same, Late
};

class Customers;

//Time类用来表示时间
class Time {
	friend class Customers;
	friend class Table;
private:
	int hour;	//小时
	int minute;		//分钟
	bool nextDay;	//是否到了第二天，这里以11:00为一天开始，
					//24:00（以00:00表示）为结束，超过则被判定为第二天
public:
	Time() {};
	//根据string类型变量(xx:xx)构造Time
	Time(string s) {
		string t = "";
		int i = 0;
		while (s[i] != ':') {
			t = t + s[i];
			i++;
		}
		hour = stoi(t);
		i++;
		t = "";
		while (i < s.length()) {
			t = t + s[i];
			i++;
		}
		minute = stoi(t);
		if (hour >= 11)
			nextDay = false;
		else
			nextDay = true;
	}
	//afterT()函数计算经过t分钟后的时间
	Time* afterT(int t) {
		Time* res = new Time();
		if (minute + t >= 60) {
			if ((hour + (minute + t) / 60) > 23) {
				res->hour = (hour + (minute + t) / 60) % 24;
				res->minute = (minute + t) % 60;
				res->nextDay = true;
			}
			else {
				res->hour = hour + (minute + t) / 60;
				res->minute = (minute + t) % 60;
				res->nextDay = false;
			}
		}
		else {
			res->hour = hour;
			res->minute = minute + t;
			res->nextDay = false;
		}
		return res;
	}
	//comp()函数比较两个时间的早晚
	int comp(Time* t1, Time* t2) {
		if (t1->nextDay == false && t2->nextDay == true)
			return Early;
		if (t1->nextDay == true && t2->nextDay == false)
			return Late;
		if (t1->nextDay == t2->nextDay) {
			if (t1->hour > t2->hour)
				return Late;
			if (t1->hour < t2->hour)
				return Early;
			if (t1->hour == t2->hour) {
				if (t1->minute < t2->minute)
					return Early;
				if (t1->minute > t2->minute)
					return Late;
				if (t1->minute == t2->minute)
					return Same;
			}
		}
	}
	//length()函数计算从t1到t2经过的分钟数，这里保证t1不晚于t2
	int length(Time* t1, Time* t2) {
		int res;
		if (t1->nextDay == t2->nextDay) {
			res = (t2->hour - t1->hour) * 60 + (t2->minute - t1->minute);
		}
		else {
			res = (24 - t1->hour) * 60 - t1->minute + t2->hour * 60 + t2->minute;
		}
		return res;
	}
	//Time2int()函数将输入的就餐用时转化成int型分钟数
	//该函数仅对就餐时间用！！！
	int Time2int() {
		return minute;
	}
	//Time2str()函数将时间转换为string以便输出
	string Time2str() {	
		string res = "";
		if (hour == 0)
			res = res + "00:";
		else if (hour < 10)
			res = res + "0" + to_string(hour) + ":";
		else
			res = res + to_string(hour) + ":";
		if (minute == 0)
			res = res + "00";
		else if (minute < 10)
			res = res + "0" + to_string(minute);
		else
			res = res + to_string(minute);
		return res;
	}
	//获取nextDay
	bool getNextDay() {
		return nextDay;
	}
};

class Customers {
	friend class Table;
private:
	int ID;
	int personNum;
	int eatTime;
	int waitTime;
	Time* arriveTime;
	Time* settleDown;
	Time* leaveTime;
public:
	//通过输入初始化顾客对象，此时还没有得到所有的变量值
	Customers(int n, Time* arrT, Time* eatT) {
		ID = -1;
		personNum = n;
		eatTime = eatT->Time2int();
		waitTime = -1;
		arriveTime = arrT;
		settleDown = nullptr;
		leaveTime = nullptr;
	}
	//设置ID
	void setID(int i) {
		ID = i;
	}
	//设置waitTime
	void setWaitTime() {
		waitTime = settleDown->length(arriveTime, settleDown);
	}
	//设置settleDown
	void setSettleDown(Time* curTime) {
		settleDown = curTime;
	}
	//设置leaveTime
	void setLeaveTime() {
		leaveTime = settleDown->afterT(eatTime);
	}

	//获得顾客到达时间
	Time* getArriveTime() {
		return arriveTime;
	}
	//获得顾客ID
	/*int getID() {
		return ID;
	}*/
	//获得顾客人数
	int getPersonNum() {
		return personNum;
	}
	//获得顾客等候时间
	int getWaitTime() {
		return waitTime;
	}
	//获得顾客停留时间
	int getStayTime() {
		return waitTime + eatTime;
	}
	//获得顾客离开时间
	Time* getLeaveTime() {
		return leaveTime;
	}

	//输出顾客信息
	void customerInfo(ofstream &outputFile) {
		outputFile << setw(3) << setfill('0') << ID << '\t';
		outputFile << personNum << '\t';
		outputFile << arriveTime->Time2str() << '\t';
		outputFile << waitTime / 60 << ":" << setw(2) << setfill('0') << waitTime % 60 << '\t';
		outputFile << settleDown->Time2str() << '\t';
		outputFile << "0:" << setw(2) << setfill('0') << eatTime << '\t';
		outputFile << leaveTime->Time2str() << endl;
	}
	

};
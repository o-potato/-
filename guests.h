#pragma once
#include<fstream>
#include<iomanip>
#include<string>

using namespace std;

//������ʾʱ������
enum TimeSeq
{
	Early, Same, Late
};

class Customers;

//Time��������ʾʱ��
class Time {
	friend class Customers;
	friend class Table;
private:
	int hour;	//Сʱ
	int minute;		//����
	bool nextDay;	//�Ƿ��˵ڶ��죬������11:00Ϊһ�쿪ʼ��
					//24:00����00:00��ʾ��Ϊ�������������ж�Ϊ�ڶ���
public:
	Time() {};
	//����string���ͱ���(xx:xx)����Time
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
	//afterT()�������㾭��t���Ӻ��ʱ��
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
	//comp()�����Ƚ�����ʱ�������
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
	//length()���������t1��t2�����ķ����������ﱣ֤t1������t2
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
	//Time2int()����������ľͲ���ʱת����int�ͷ�����
	//�ú������ԾͲ�ʱ���ã�����
	int Time2int() {
		return minute;
	}
	//Time2str()������ʱ��ת��Ϊstring�Ա����
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
	//��ȡnextDay
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
	//ͨ�������ʼ���˿Ͷ��󣬴�ʱ��û�еõ����еı���ֵ
	Customers(int n, Time* arrT, Time* eatT) {
		ID = -1;
		personNum = n;
		eatTime = eatT->Time2int();
		waitTime = -1;
		arriveTime = arrT;
		settleDown = nullptr;
		leaveTime = nullptr;
	}
	//����ID
	void setID(int i) {
		ID = i;
	}
	//����waitTime
	void setWaitTime() {
		waitTime = settleDown->length(arriveTime, settleDown);
	}
	//����settleDown
	void setSettleDown(Time* curTime) {
		settleDown = curTime;
	}
	//����leaveTime
	void setLeaveTime() {
		leaveTime = settleDown->afterT(eatTime);
	}

	//��ù˿͵���ʱ��
	Time* getArriveTime() {
		return arriveTime;
	}
	//��ù˿�ID
	/*int getID() {
		return ID;
	}*/
	//��ù˿�����
	int getPersonNum() {
		return personNum;
	}
	//��ù˿͵Ⱥ�ʱ��
	int getWaitTime() {
		return waitTime;
	}
	//��ù˿�ͣ��ʱ��
	int getStayTime() {
		return waitTime + eatTime;
	}
	//��ù˿��뿪ʱ��
	Time* getLeaveTime() {
		return leaveTime;
	}

	//����˿���Ϣ
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
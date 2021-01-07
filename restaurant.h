#pragma once
#include<vector>
#include"guests.h"
#define MaxCapacity 20		//ÿ��������������ɵ�����
using namespace std;

//enum Status {
//	Available, NoTable
//};

class Restaurant;

class Table {
	friend class Restaurant;
private:
	int capacity;	//ÿ����������
	bool busy;		//����Ƿ�ʹ��
	Time* freeTime;		//����ʹ�ã��ͷ�ʱ��
public:
	Table(int c) {	//��ʼ״̬���ӿ���
		capacity = c;
		busy = false;
		freeTime = nullptr;
	}
	//���ӱ�customerռ��
	void beUsed(Customers* c) {
		busy = true;
		freeTime = c->leaveTime;
	}
	//���ӱ��ͷ�
	void beFreed() {
		busy = false;
		freeTime = nullptr;
	}
	//��ȡ�����ͷ�ʱ��
	Time* getFreeTime() {
		return freeTime;
	}
	//��ȡ��������
	int getCapacity() {
		return capacity;
	}
};

class Restaurant {
private:
	vector<Table*> allTables[MaxCapacity + 1];	//�ò͹�ӵ�е��������ӣ��������Ӱ������γ�vector���飬���γ�������
	vector<Table*> freeTables[MaxCapacity + 1];	//�ò͹ݿ��е�����
	vector<Table*> busyTables[MaxCapacity + 1];	//�ò͹ݱ�ռ�õ�����
public:
	Restaurant(vector<Table*> a) {	//��ʼ״̬�������Ӷ��ǿ��е�
		for (int i = 0; i < a.size(); i++) {
			allTables[a[i]->capacity].push_back(a[i]);
			freeTables[a[i]->capacity].push_back(a[i]);
		}
	}
	//�ж�n���ܷ�����������ԣ�������������
	int nPersonTable(int n) {
		for (int i = n; i <= MaxCapacity; i++) {
			if (freeTables[i].size() > 0)
				return i;
		}
		return -1;
	}
	//ռ��һ��n����
	void useTable(int n, Customers* c) {
		Table* p = freeTables[n].back();
		freeTables[n].pop_back();
		p->beUsed(c);
		busyTables[n].push_back(p);
	}
	//�ͷ�һ��n����
	void freeTable(int n, Table* T) {
		for (int i = 0; i < busyTables[n].size(); i++) {
			if (busyTables[n][i] == T) {
				for (int j = i; j < busyTables[n].size() - 1; j++) {
					busyTables[n][j] = busyTables[n][j + 1];
				}
				busyTables[n].pop_back();
				break;
			}
		}
		Table* p = new Table(n);
		freeTables[n].push_back(p);
	}
	//�ҵ���ռ�����������ȱ��ͷŵ�����
	Table* latestTable() {
		Table* latest = NULL;
		for (int i = 1; i < MaxCapacity; i++) {
			if (busyTables[i].size()) {
				if (latest == NULL)
					latest = busyTables[i][0];
				Table* tmp = busyTables[i][0];
				int j = 0;
				while (j < busyTables[i].size()) {
					Time* t = busyTables[i][j]->getFreeTime();
					int res = t->comp(t, latest->getFreeTime());
					if (res == Early)
						latest = busyTables[i][j];
					j++;
				}
			}
		}
		return latest;
	}
};
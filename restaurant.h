#pragma once
#include<vector>
#include"guests.h"
#define MaxCapacity 20		//每张桌子最多能容纳的人数
using namespace std;

//enum Status {
//	Available, NoTable
//};

class Restaurant;

class Table {
	friend class Restaurant;
private:
	int capacity;	//每张桌子容量
	bool busy;		//标记是否被使用
	Time* freeTime;		//若被使用，释放时间
public:
	Table(int c) {	//初始状态桌子空闲
		capacity = c;
		busy = false;
		freeTime = nullptr;
	}
	//桌子被customer占用
	void beUsed(Customers* c) {
		busy = true;
		freeTime = c->leaveTime;
	}
	//桌子被释放
	void beFreed() {
		busy = false;
		freeTime = nullptr;
	}
	//获取桌子释放时间
	Time* getFreeTime() {
		return freeTime;
	}
	//获取桌子容量
	int getCapacity() {
		return capacity;
	}
};

class Restaurant {
private:
	vector<Table*> allTables[MaxCapacity + 1];	//该餐馆拥有的所有桌子，所有桌子按容量形成vector数组，再形成总数组
	vector<Table*> freeTables[MaxCapacity + 1];	//该餐馆空闲的桌子
	vector<Table*> busyTables[MaxCapacity + 1];	//该餐馆被占用的桌子
public:
	Restaurant(vector<Table*> a) {	//初始状态所有桌子都是空闲的
		for (int i = 0; i < a.size(); i++) {
			allTables[a[i]->capacity].push_back(a[i]);
			freeTables[a[i]->capacity].push_back(a[i]);
		}
	}
	//判断n人能否就坐，若可以，返回桌子容量
	int nPersonTable(int n) {
		for (int i = n; i <= MaxCapacity; i++) {
			if (freeTables[i].size() > 0)
				return i;
		}
		return -1;
	}
	//占用一张n人桌
	void useTable(int n, Customers* c) {
		Table* p = freeTables[n].back();
		freeTables[n].pop_back();
		p->beUsed(c);
		busyTables[n].push_back(p);
	}
	//释放一张n人桌
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
	//找到被占用桌子中最先被释放的桌子
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
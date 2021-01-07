#include<cmath>
#include"guests.h"
#include"restaurant.h"

using namespace std;

Time* startTime = new Time("11:00");	//��ʼӪҵ��ʱ��
Time* endTime = new Time("2:00");	//����Ӫҵ��ʱ��

////�����Թ˿ͽ�������
//void swap(Customers* &c1, Customers* &c2) {
//	Customers* t;
//	t = c1;
//	c1 = c2;
//	c2 = t;
//}

int main() {
	//����input_table.txt���쵩Է�󷹵�
	ifstream inputTable("input_table.txt");
	vector<Table*> t;
	int a, b;
	while (inputTable >> a >> b) {	//a��b����
		while (a) {
			Table* tab = new Table(b);
			t.push_back(tab);
			a--;
		}
	}
	Restaurant* danyuanRes = new Restaurant(t);		//��Է�󷹵�

	//����input_customer.txt����˿Ͷ���
	//��ʱ���������ò��������ò�ʱ�䣬����ʱ��
	ifstream inputCustomer("input_customer.txt");
	vector<Customers*> cus;
	int n;
	string s1, s2;
	while (inputCustomer >> n >> s1 >> s2) {	//�Ͳ�����Ϊn������ʱ��Ϊs1(xx:xx)���Ͳ�ʱ��Ϊs2(xx:xx)
		Time* t1, * t2;
		t1 = new Time(s1);
		t2 = new Time(s2);
		Customers* c = new Customers(n, t1, t2);
		cus.push_back(c);
	}
	
	//�Թ˿�����cus��ʱ���Ⱥ����򣬲��ò�������
	for (int i = 1; i < cus.size(); i++) {
		int j = i - 1;
		Customers* tmp = cus[i];
		while (j >= 0) {
			Time* t1, * t2;
			t1 = cus[i]->getArriveTime();
			t2 = cus[j]->getArriveTime();
			int compT = cus[i]->getArriveTime()->comp(t1, t2);
			if (compT < Late) {
				cus[j + 1] = cus[j];
			}
			else {
				cus[j + 1] = tmp;
				break;
			}
			j--;
		}
	}
	
	//���ź���Ĺ˿�����cus��˳������ID
	//�˿�ID��1��ʼ
	for (int i = 0; i < cus.size(); i++) {
		cus[i]->setID(i + 1);
	}
	
	//����ÿ��˿͵ľͲͣ��ڴ˹����пɻ��ÿ��˿�
	//�ľ���ʱ�䣬�ȴ�ʱ�䣬�뿪ʱ��
	Time* currentTime = startTime;	//��currentTime����¼��ǰʱ���
	//Time* nextTime;		//��nextTime�������һ���ؼ�ʱ��㣺�ͷ����ӣ����Ź˿�
	int cus_num = 0;		//cus_num��¼��ǰ�����ŵĹ˿����
	Table* toFree;		//��¼�����ͷŵ�����
	while (cus_num < cus.size()) {
		//���ȸ���ʱ���жϵ�ǰ�Ƿ����ѵ���˿ͣ������ѵ���˿ͣ��Ե�ǰ�˿ͣ�
		//�ж��Ƿ��п����ɵĿ������ӣ��������ӣ������ǰ�˿Ͱ�������������
		//��ʱ��˳���ͷ����ӣ���û���ѵ���˿ͣ���ʱ��˳���ͷ�����
		int cusWait = currentTime->comp(currentTime, cus[cus_num]->getArriveTime());	//cusWait��¼��ǰ�˿��Ƿ��Ѿ�����
		int capa = danyuanRes->nPersonTable(cus[cus_num]->getPersonNum());		//capa��¼��ǰ�˿�������Ҫ�����������
		if (cusWait > Early && capa > 0) {	//��ǰʱ�����ѵ���˿��ҵ�ǰ������������˿͵�����
			cus[cus_num]->setSettleDown(currentTime);	//cus[cus_num]�ڵ�ǰʱ�̾���
			cus[cus_num]->setLeaveTime();
			cus[cus_num]->setWaitTime();
			danyuanRes->useTable(capa, cus[cus_num]);	//cus[cus_num]ռ��һ��capa����
			cus_num++;
		}
		else {	//��ʱ��˳���ͷ�����
			//�ͷ���������Ҫ����Ƿ��п��ͷ����ӣ���busyTables[]��Ϊ�գ�
			//��û�п��ͷ����ӣ�˵����ǰʱ�̷��괦�ڿ���״̬��Ӧ����
			//currentTimeΪ��һ��˿͵���ʱ��(��������˿�������������
			//�����������������������п��ͷ����ӣ�������latestTable()
			//��ȡ���ȱ��ͷŵ����ӣ����ͷŸ����ӣ�ͬʱҪ��currentTime��
			//��Ϊ�ͷŸ����ӵ�ʱ�䣬��������һ��ѭ��
			//Time* nextArr = cus[cus_num]->getArriveTime();
			toFree = danyuanRes->latestTable();		//toFree == NULL˵��û�п��ͷ�����
			if (toFree == NULL) {
				currentTime = cus[cus_num]->getArriveTime();
			}
			else {
				currentTime = toFree->getFreeTime();
				danyuanRes->freeTable(toFree->getCapacity(), toFree);
			}
		}
	}

	//������һ��˿��뿪ʱ��
	//������ȫ�����У�˵�����й˿;����뿪�����򣬰�ʱ��˳��
	//�ͷ��������ӣ����յõ���currentTime��Ϊ���һ���뿪ʱ��
	toFree = danyuanRes->latestTable();
	while (toFree != NULL) {
		currentTime = toFree->getFreeTime();
		danyuanRes->freeTable(toFree->getCapacity(), toFree);
		toFree = danyuanRes->latestTable();
	}

	int sumStayTime = 0;	//���й˿���ͣ��ʱ��
	int sumWaitTime = 0;	//���й˿��ܵȺ�ʱ��
	double aveStayTime;	//ƽ��ͣ��ʱ�䣬��ȷ������
	double aveWaitTime;	//ƽ���Ⱥ�ʱ�䣬��ȷ������
	for (int i = 0; i < cus.size(); i++) {
		if (cus[i]->getLeaveTime()->getNextDay())
			continue;
		sumStayTime = sumStayTime + cus[i]->getStayTime();
		sumWaitTime = sumWaitTime + cus[i]->getWaitTime();
	}
	aveStayTime = 1.0 * sumStayTime / cus.size();
	aveWaitTime = 1.0 * sumWaitTime / cus.size();

	ofstream outputData("output_data.txt");
	ofstream outputCustomer("output_customer.txt");

	outputData << "ƽ��ͣ��ʱ�䣺" << round(aveStayTime) << "min" << endl;
	outputData << "ƽ���Ⱥ�ʱ�䣺" << round(aveWaitTime) << "min" << endl;
	outputData << "���һ��˿��뿪ʱ�䣺" << currentTime->Time2str() << endl;

	outputCustomer << "���" << '\t'
		<< "�˿�����" << '\t'
		<< "����ʱ��" << '\t'
		<< "�Ⱥ���ʱ" << '\t'
		<< "�Ͳ�ʱ��" << '\t'
		<< "�Ͳ���ʱ" << '\t'
		<< "�뿪ʱ��" << '\t' << endl;
	for (int i = 0; i < cus.size(); i++) {
		cus[i]->customerInfo(outputCustomer);
	}

	return 0;
}
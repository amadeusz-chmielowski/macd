// g++ main.cpp -o main; ./main

#include<iostream>
#include<fstream>
#include<string>
#include<math.h>

using namespace std;

struct dane {
	int index;
	string name;
};


void loadWIG20(double * out, int size) {

	ifstream input("WIG20_1000.txt");

	int k = 0;
	if (input.is_open()) {
		//while(!input.eof() && k < size ){
		while (k < size) {
			string number;
			int data;
			getline(input, number); //read number
			out[k] = (double)atof(number.c_str()); //convert 
			k++;
		}

	}
}


double alfa(int n) {
	if (n == 26) {
		return 0.08;
	}
	if (n == 12) {

		return 0.1818181818181818181818181818181818181818;
	}
	if (n == 9) {
		return 0.25;
	}

}

double OneMinusALfa(int n) {
	return (1 - alfa(n));
}

double powAlfa(int n, int alfa) {
	return pow(alfa, n);
}

void ema26(double ** in, double * out, int size) {
	double * temp = *in;
	double g = 0;
	double x = OneMinusALfa(26);
	for (int i = 0; i < size; i++) {
		out[i] = (double)0;
	}
	for (int i = 0; i < size; i++) {
		if (i < 26) {
			out[i] = (double)0;
		}
		else {
			for (int j = 0; j < 26; j++) {
				double z = pow(x, j);

				out[i] += temp[i - j] * z;
				g = g + z;
			}
			out[i] = out[i] / g;
			g = 0;
		}

	}
}

void ema12(double ** in, double * out, int size) {
	double * temp = *in;
	double g = 0;
	double x = OneMinusALfa(12);
	for (int i = 0; i < size; i++) {
		out[i] = (double)0;
	}
	for (int i = 0; i < size; i++) {
		if (i < 26) {
			out[i] = (double)0;
		}
		else {
			for (int j = 0; j < 12; j++) {
				double z = pow(x, j);
				out[i] += temp[i - j] * z;
				g = g + z;
			}
			out[i] = out[i] / g;
			g = 0;
		}

	}
}

void signal(double ** in, double * out, int size) {
	double * temp = *in;
	double x = OneMinusALfa(9);
	double g = 0;
	for (int i = 0; i < size; i++) {
		out[i] = (double)0;
	}
	for (int i = 0; i < size; i++) {
		if (i < 26) {
			out[i] = (double)0;
		}
		else {
			for (int j = 0; j < 9; j++) {
				double z = pow(x, j);
				out[i] += temp[i - j] * z;
				g = g + z;
			}
			out[i] = out[i] / g;
			g = 0;
		}

	}
}

double * MACD(double** in12, double** in26, int size) {
	double * macd = new double[size];
	double * in11 = *in12;
	double * in25 = *in26;
	for (int i = 0; i < size; i++) {
		macd[i] = (double)0;
	}
	for (int i = 0; i < size; i++) {
		macd[i] = in11[i] - in25[i];
	}
	return macd;
}

void buy_sell(double** in, double** in2, int size, struct dane* out, int &z) {
	double * signal = *in;
	double * macd = *in2;
	//char = "B";
	int j = 0;
	for (int i = 0; i < size; i++) {
		if ((macd[i - 1] < signal[i] < macd[i + 1])&& (macd[i]+1>=signal[i]>=macd[i]-1)) {
			out[j].name = "BUY";
			out[j].index = i;
			j++;
		}
		if ((macd[i - 1] > signal[i] > macd[i + 1]) && (macd[i] + 1 <= signal[i] <= macd[i] - 1)) {
			out[j].name = "SELL";
			out[j].index = i;
			j++;
		}
	}
	z = j;
}

void auto_buy_sell(struct dane** in, double ** wig, int size,int z, double * out) {
	struct dane* buy_sell = *in;
	double * WIG20 = *wig;
	bool buy = false;
	bool sell = true;
	int j = 0;
	string * temp = new string[z];
	for (int i = 0; i < z; i++) {
		if (sell && buy_sell[i].name == "SELL") {
			sell = false;
			buy = true;
			temp[j] = buy_sell[i].name;
			j++;
		}
		if (buy && buy_sell[i].name == "BUY") {
			sell = true;
			buy = false;
			temp[j] = buy_sell[i].name;
			j++;
		}
	}

}

int main() {
	std::cout << "Load WIG20" << std::endl;

	int size = 1000;
	double * WIG20 = new double[size];
	double * ema25 = new double[size];
	double * ema11 = new double[size];
	double * signal1 = new double[size];
	struct  dane* oo = new dane[size];
	double * indexes = new double[size];
	double * macd;
	int z=0;
	loadWIG20(WIG20, size);
	ema26(&WIG20, ema25, size);
	ema12(&WIG20, ema11, size);
	macd = MACD(&ema11, &ema25, size);
	signal(&macd, signal1, size);
	buy_sell(&signal1, &signal1, size, oo,z);
	auto_buy_sell(&oo, &WIG20, size, z, indexes);


	ofstream plik("nowy.txt");
	ofstream macdf("macd.txt");
	ofstream signalf("signal.txt");
	plik << "WIG20" << "\t\t" << "MACD" << "\t\t" << "EMA11" << "\t\t" << "EMA25" << "\t\t" << "SIGNAL" << endl;
	for (int k = 0; k < size; k++) {
		//cout <<k<<"\t"<<WIG20[k] << "\t\t" << macd[k] << "\t\t"<<ema11[k]<<"\t\t"<<ema25[k]<<"\t\t"<<signal1[k]<<endl;
				plik << WIG20[k] << "\t\t" << macd[k] << "\t\t" << ema11[k] << "\t\t" << ema25[k] << "\t\t" << signal1[k] << endl;
		macdf << macd[k] << endl;
		signalf << signal1[k] << endl;
	}
	for (int i = 0; i < z; i++) {
		//cout << oo[i].name << " at: " << oo[i].index << endl;
	//	cout << indexes[i] << endl;
	}



	return 0;
}
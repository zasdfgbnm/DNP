#include "spin.hpp"
#include <iostream>
#include <fstream>

double fi = -14.5_MHz;
double Azx[7] = { 1.0625_MHz,-2.5745_MHz,0.1657_MHz,1.2042_MHz,0.0968_MHz,0.7024_MHz,-2.6575_MHz };
double Azy[7] = { 0.4234_MHz,-0.5842_MHz,-1.8497_MHz,-1.6795_MHz,-1.9991_MHz,-0.6635_MHz,-1.9556_MHz };
double Azz[7] = { -3.9900_MHz,-1.3463_MHz,-0.5889_MHz,-0.5885_MHz,1.3987_MHz,-1.2610_MHz,7.4265_MHz };

int main() {
	int n = 2000;
	double T = 2000_ns;
	double delta_t = T/n;
	vector<double> fe_list = { -30_MHz,-25_MHz,-20_MHz,-14.5_MHz,-10_MHz,-5_MHz,0_MHz,5_MHz,10_MHz,14.5_MHz,20_MHz,25_MHz,30_MHz };
	for(auto k=fe_list.begin();k!=fe_list.end();k++) {
		/* Hamiltonian */
		double fe = *k;
		Operator H = 2*pi*fe*Sx(0);
		for(int i=0;i<7;i++)
			H += 2*pi*(fi*Sz(i+1)+Sz(0)*(Azx[i]*Sx(i+1)+Azy[i]*Sy(i+1)+Azz[i]*Sz(i+1)));
		auto U = H.U();
		/* initial state */
		double alpha_e = 7.6216e-4;
		double alpha_i = -1.1579e-6;
		Operator rho0 = Op<2>(0,0.5*(1-alpha_e),0,0,0.5*(1+alpha_e));
		//Operator rho0 = Op<2>(0,0.5,0.5,0.5,0.5);
		for(int i=1;i<=7;i++)
			rho0 *= Op<2>(i,0.5*(1-alpha_i),0,0,0.5*(1+alpha_i));
		/* output stream */
		stringstream fnstream;
		fnstream << "malonic-no-main-H_" << fe/1_MHz << "MHz.txt";
		string fn = fnstream.str();
		ofstream out(fn);
		/* calculate and output */
		for(int i=0;i<=n;i++) {
			double t = delta_t*i;
			cout << fn << ":\t" << t/T*100 << "%" << endl;
			out << t/1_ns << '\t';
			Operator rho = U(t)*rho0*U(-t);
			Operator rhoe = rho.tr(1,2,3,4,5,6,7);
			out << real(tr(rhoe*Sx(0))) << '\t';
			out << real(tr(rhoe*Sy(0))) << '\t';
			out << real(tr(rhoe*Sz(0))) << '\t';
			for(int j=1;j<8;j++) {
				Operator rhoj = rho;
				for(int k=0;k<8;k++) {
					if(k==j)
						continue;
					rhoj = rhoj.tr(k);
				}
				out << real(tr(rhoj*Sz(j))) << '\t';
			}
			out << endl;
		}
		out.close();
	}
}
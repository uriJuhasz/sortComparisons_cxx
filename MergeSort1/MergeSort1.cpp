#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <array>
#include <cassert>
#include <functional>
#include <chrono>
#include <iomanip>
#include <string>

using namespace std;

template<typename T>void inplaceMergeSort(vector<T>& in);
template<typename T>void inplaceQuickSort(vector<T>& in);
template<typename T>void checkSorted(const vector<T>& in);
template<typename T>void checkPermutationOf(const vector<T>& a, const vector<T>& b);

template<class F> long measure(F f)
{
	auto startT = chrono::system_clock::now();
	f();
	auto endT = chrono::system_clock::now();

	chrono::duration<double> elapsed_seconds = endT - startT;

	const long elapsedNS = static_cast<long>(1000000 * elapsed_seconds.count());

	return elapsedNS;
}

template<class F>static void checkSort(const string& name, F f, const vector<int>& in)
{
	auto v = in;

	cout << " " << name;
	const auto elapsedNS = measure([&v,&f]() {f(v); return 0; });
	cout << " - " << elapsedNS << "ns";

	cout << "  Checking";
	checkSorted(v);
	checkPermutationOf(v, in);
	cout << " - OK";
}
int main()
{
    cout << "Start" << endl;
	
	constexpr int numRuns = 10;
	cout << "Running " << numRuns << endl;
	
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

	constexpr int maxSize = 10000000;
	for (int i = 0; i < numRuns;++i)
	{
		const int numValues = uniform_int_distribution<int>(0, maxSize)(gen);

		cout << " Run " << i << " size: " << numValues << endl;
		cout << "  Generating input" << endl;
		uniform_int_distribution<int> dis(0, numeric_limits<int>::max());
		vector<int> v; v.reserve(numValues);
		for (int i = 0; i < numValues; ++i)
		{
			v.push_back(dis(gen));
		}

		checkSort("MergeSort", inplaceMergeSort<int>, v);
		checkSort("QuickSort", inplaceQuickSort<int>, v);
		cout << " Done" << endl;
	}
	cout << "End" << endl;
}

template<typename T>void checkSortedRange(const vector<T>& in, const int s, const int e)
{
	for (int i = s; i < min(e,(int)in.size())-1; ++i)
		if (!(in[i] <= in[i + 1]))
		{
			cerr << "Sorting failure at " << i << endl;
			throw new exception();
		}
}

template<typename T>void checkSorted(const vector<T>& in)
{
	checkSortedRange(in, 0, in.size());
}

//Assume a sorted
template<typename T>void checkPermutationOf(const vector<T>& a, const vector<T>& b)
{
	unordered_map<T, int> countsA,countsB;
	for (auto v : a)
		countsA[v]++;
	for (auto v : b)
		countsB[v]++;
	for (auto kv : countsA)
		if (countsB[kv.first] != kv.second)
		{
			cerr << "Permutation failure at " << kv.first << ": " << kv.second << " vs " << countsB[kv.first] << endl;
			throw new exception();
		}
}

constexpr bool enableAssertions = false;

template<class T>int qsSplit(vector<T>& in, const int s, const int e)
{
	assert(s >= 0 && s < in.size());
	assert(e >= 0 && e < in.size());
	assert(s <= e);
	if (s == e)
		return s;
	const auto pivot = in[e];
	int l = s;
	int h = e;
	while (l < h)
	{
		while (l < h && in[l] < pivot)
			l++;
		while (l < h && in[h] > pivot)
			h++;
		if (l < h)
			swap(in[l], in[h]);
	}
	return l;
}
template<class T>void inplaceQuickSort1(vector<T>& in, const int s, const int e)
{
	assert(s >= 0 && s < in.size());
	assert(e >= 0 && e < in.size());
	assert(s <= e);
	if (s == e)
		return;
	const int m = qsSplit(in,s, e);
	inplaceQuickSort1(in, s,   m);
	inplaceQuickSort1(in, m+1, e);
}
template<class T>void inplaceQuickSort(vector<T>& in)
{
	if (in.size() == 0)
		return;
	inplaceQuickSort1(in, 0, in.size() - 1);
}

template<class T>void inplaceMergeSort(vector<T>& in)
{
	vector<T> buffer(in.size());
	const int num = (int)in.size();
	for (auto s = 1; s < num; s *= 2)
	{
//		cout << " sorting block size " << s << " / " << num << endl;
		for (int bs = 0; bs < num; bs+=2*s)
		{
			const auto be = bs + s;
//			cout << "  sorting block " << bs << " - " << be << endl;
			if (be < num)
			{
				      array<int, 2> p = { bs,         bs + s };
				const array<int, 2> e = { p[1], min(p[1] + s, num) };
//				cout << "   merging " << p[0] << "-" << e[0] << "  and " << p[1] << "-" << e[1] << endl;
				if (enableAssertions)
				{
					if (s > 1)
					{
						checkSortedRange(in, p[0], e[0]);
						checkSortedRange(in, p[1], e[1]);
					}
				}
				int op = 0;
				while (p[0] < e[0] || p[1] < e[1])
				{
					const int w = 
						(p[1] >= e[1]) 
							? 0
							: (p[0] >= e[0])
								? 1
								: in[p[0]]<=in[p[1]] ? 0 : 1;
					{
						auto& r = p[w];
						buffer[op] = in[r]; op++;
						r++;
					}
				}
				for (int i = 0; i < op; ++i)
					in[bs + i] = buffer[i];
				if (enableAssertions)
				{
					checkSortedRange(in, bs, e[1]);
				}
			}
		}
	}
}

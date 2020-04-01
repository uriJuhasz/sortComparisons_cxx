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

constexpr bool debugBuild =
#ifdef _DEBUG
true
#else
false
#endif
;

template<typename T>void inplaceMergeSort(vector<T>& in);
template<typename T>void inplaceQuickSort1(vector<T>& in);
template<typename T>void inplaceQuickSort2(vector<T>& in);
template<>void inplaceQuickSort2<int>(vector<int>& in);
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
	vector<int> v = in;

	cout << " " << name;
	const auto elapsedNS = measure([&v,&f]() {f(v); return 0; });
	cout << " - " << setw(10) << elapsedNS << "ns" << endl;

	cout << "  Checking";
	checkPermutationOf(v, in);
	checkSorted(v);
	cout << " - OK" << endl;
}
int main()
{
    cout << "Start" << endl;
	
	constexpr int numRuns = 100;
	cout << "Running " << numRuns << endl;
	
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

	constexpr int maxSize = 100000;
	for (int i = 0; i < numRuns;++i)
	{
		const int numValues = uniform_int_distribution<int>(0, maxSize)(gen);
		constexpr int maxValue = numeric_limits<int>::max();
		cout << " Run " << i << " size: " << numValues << endl;
		cout << "  Generating input" << endl;
		uniform_int_distribution<int> dis(0, maxValue); 
		vector<int> v; v.reserve(numValues);
		for (int i = 0; i < numValues; ++i)
		{
			v.push_back(dis(gen));
		}

		checkSort("MergeSort ", inplaceMergeSort<int>, v);
		checkSort("QuickSort1", inplaceQuickSort1<int>, v);
		checkSort("QuickSort2", inplaceQuickSort2<int>, v);
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
	checkSortedRange(in, 0, (int)in.size());
}

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

/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
template<class T>int qsSplit1(vector<T>& in, const int s, const int e);
/*
template<>int qsSplit<int>(vector<int>& in, const int s, const int e)
{
	assert(s >= 0 && s < in.size());
	assert(e >= 0 && e < in.size());
	assert(s+1 < e); //At least 3 elements
	if (s+1 >= e)
		return s;
	const auto pivot = in[e];
	int l = s;
	int h = e;
	while (true)
	{
		while (in[l] < pivot)
			l++;
		while (in[h] > pivot)
			h--;
		if (l >= h)
			break;
		if (in[l] > in[h])
			swap(in[l], in[h]);
		else //in[l]==in[h]==pivot
			h--;
	}
	assert(l >  s);
	assert(l <= e);
	for (int i = s; i <= e; ++i)
		if (i < l)
			assert(in[i] <= pivot);
		else if (i > l)
			assert(in[i] >= pivot);
//		else
//			assert(in[i] == pivot);
	return l;
}
*/
template<>int qsSplit1<int>(vector<int>& in, const int s, const int e)
{
	assert(s >= 0 && s < in.size());
	assert(e >= 0 && e < in.size());
	assert(s + 1 < e); //At least 3 elements
	if (s + 1 >= e)
		return s;
	const int pivot = in[e];
	int l = s;
	for (int j = s; j <= e; ++j)
		if (in[j] < pivot)
		{
			if (l < j)
			{
				assert(!(in[l]<pivot));
				swap(in[l], in[j]);
			}
			l++;
		}
	if (l < e)
		swap(in[l], in[e]);
	if (debugBuild)
	{
		for (int i = s; i < l; ++i)
			assert(in[i] < pivot);
		assert(in[l] == pivot);
		for (int i = l + 1; i < e; ++i)
			assert(in[i] >= pivot);
	}
	return l;
}
template<class T>void inplaceQuickSort1R(vector<T>& in, const int s, const int e)
{
	if (s >= e)
		return; //length 1
	assert(s >= 0 && s < in.size());
	assert(e >= 0 && e < in.size());
//	assert(s <= e);
	if (s + 1 == e)
	{
		if (in[s] > in[e])
			swap(in[s], in[e]);
		return;
	}
	const int m = qsSplit1(in,s, e);
	inplaceQuickSort1R(in, s, m-1);
	inplaceQuickSort1R(in, m+1, e);
	if (debugBuild) checkSortedRange(in, s, e);
}
template<class T>void inplaceQuickSort1(vector<T>& in)
{
	if (in.size() < 2)
		return;
	inplaceQuickSort1R(in, 0, (int)in.size() - 1);
}

/////////////////////////////////////////////////////////////////////////
//template<class T>void inplaceQuickSort2(vector<T>& in);
void inplaceQuickSort2R(vector<int>& in, const int s, const int e);
template<>void inplaceQuickSort2<int>(vector<int>& in)
{
	if (in.size() < 2)
		return;
	inplaceQuickSort2R(in, 0, (int)in.size() - 1);
}
template<class T> inline void swap3(T& a, T& b, T& c)
{
	const T t = a;
	a = b;
	b = c;
	c = t;
}
template<class T> inline void swap4(T& a, T& b, T& c, T& d)
{
	const T t = a;
	a = b;
	b = c;
	c = d;
	d = t;
}
void inplaceQuickSort2R(vector<int>& in, const int s, const int e)
{
	if (s >= e)
		return;
	int m1 = s; int n1 = m1;
	int m2 = e; int n2 = m2;
	const auto pivot = in[e];
	while (true)
	{
		if (m1 == n1)
		{
			while (in[m1] < pivot && m1<n2)
				m1++;
			n1 = m1;
			assert(n1 <= n2);
		}
		while (n1 < n2)
		{
			if (in[n1] < pivot)
			{
				swap(in[m1], in[n1]);
				n1++;
				m1++;
			}
			else if (in[n1] == pivot)
			{
				n1++;
			}
			else //(in[n1] > pivot)
				break;
		}
		assert(n1 <= n2);
		//Here n1==n2 || in[n1]>pivot

		if (m2 == n2)
		{
			while (in[m2] > pivot && m2>n1)
				m2--;
			n2 = m2;
			assert(n1 <= n2);
		}
		while (n1 < n2)
		{
			if (in[n2] > pivot)
			{
				swap(in[m2], in[n2]);
				n2--;
				m2--;
			}
			else if (in[n2] == pivot)
			{
				n2--;
			}
			else //(in[n1] < pivot)
				break;
		}
		assert(n1 <= n2);
		//Here n1==n2 || (in[n1]>pivot && in[n2]<pivot)
		if (n1 < n2)
		{
			const int x1 = n1 > m1 ? n1 : m1;
			if (n1 == m1 && n2 == m2)
				swap(in[n1], in[n2]);
			else if (n1 == m1)
				swap3(in[n1], in[n2], in[m2]);
			else if (n2 == m2)
				swap3(in[m1], in[n1], in[n2]);
			else
				swap4(in[m1], in[n2], in[m2], in[n1]);
			m1++;
			n1++;
			if (n1 < n2)
			{
				n2--;
			}
			if (n2 < m2)
				m2--;
			assert(n1 <= n2);
		}
		else
			break;
	}
	assert(n1 == n2);
	if (in[n1] < pivot)
	{
		swap(in[n1], in[m1]);
		n1++;
		m1++;
	}
	else if (in[n2] > pivot)
	{
		swap(in[n2], in[m2]);
		m2--;
	}
	if (debugBuild)
	{
		assert(m1 >= s);
		assert(m2 <= e);
		assert(m1 <= m2);
		for (int i = s; i < m1; ++i)
			assert(in[i] < pivot);
		for (int i = m1; i <= m2; ++i)
			assert(in[i] == pivot);
		for (int i = m2+1; i <= e; ++i)
			assert(in[i] > pivot);

	}
	assert(m1-1 < m2+1);
	inplaceQuickSort2R(in, s, m1-1);
	inplaceQuickSort2R(in, m2+1, e);
}
/////////////////////////////////////////////////////////////////////////
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

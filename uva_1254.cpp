//
// Created by shai.zeevi on 28/05/2019.
//

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <string.h>

using namespace std;
typedef vector<int> vi;

string SA_str; // the input string, up to 100K characters
int n; // the length of input string
vi RA, tempRA; // rank array and temporary rank array
vi SA, tempSA; // suffix array and temporary suffix array
vi c; // for counting/radix sort

void countingSort(int k) { // O(n)
    int n = SA_str.length() - 1;
    int i, sum, maxi = max(300, n); // up to 255 ASCII chars or length of n
    c.assign(maxi, 0);
    for (i = 0; i < n; i++) // count the frequency of each integer rank
        c[i + k < n ? RA[i + k] : 0]++;
    for (i = sum = 0; i < maxi; i++) {
        int t = c[i];
        c[i] = sum;
        sum += t;
    }
    for (i = 0; i < n; i++) // shuffle the suffix array if necessary
        tempSA[c[SA[i] + k < n ? RA[SA[i] + k] : 0]++] = SA[i];
    for (i = 0; i < n; i++) // update the suffix array SA
        SA[i] = tempSA[i];
}

void constructSA() { // this version can go up to 100000 characters
    int n = SA_str.length() - 1;
    RA.assign(n, 0);
    tempRA.assign(n, 0);
    SA.assign(n, 0);
    tempSA.assign(n, 0);
    int i, k, r;
    for (i = 0; i < n; i++) RA[i] = SA_str[i]; // initial rankings
    for (i = 0; i < n; i++) SA[i] = i; // initial SA: {0, 1, 2, ..., n-1}
    for (k = 1; k < n; k <<= 1) { // repeat sorting process log n times
        countingSort(k); // actually radix sort: sort based on the second item
        countingSort(0); // then (stable) sort based on the first item
        tempRA[SA[0]] = r = 0; // re-ranking; start from rank r = 0
        for (i = 1; i < n; i++) // compare adjacent suffixes
            tempRA[SA[i]] = // if same pair => same rank r; otherwise, increase r
                    (RA[SA[i]] == RA[SA[i - 1]] && RA[SA[i] + k] == RA[SA[i - 1] + k]) ? r : ++r;
        for (i = 0; i < n; i++) // update the rank array RA
            RA[i] = tempRA[i];
        if (RA[SA[n - 1]] == n - 1) break; // nice optimization trick
    }
}

struct SA_less {
    bool operator()(int a, const string& s) {
        return SA_str.substr(SA[a]) < s;
    }
};

struct SA_equals {
    bool operator()(int a, const string& s) {
        return SA_str.substr(SA[a], s.size()) == s;
    }
};

// A suffix array based search function to search a given pattern
// 'pat' in given text 'txt' using suffix array suffArr[]
//int search(string pat)
//{
//    // Do simple binary search for the pat in txt using the
//    // built suffix array
//    int l = 0, r = SA.size()-1;  // Initilize left and right indexes
//    while (l <= r)
//    {
//        // See if 'pat' is prefix of middle suffix in suffix array
//        int mid = l + (r - l)/2;
//        int res = strncmp(pat.c_str(), SA_str.substr(SA[mid]).c_str(), pat.length());
//
//        // If match found at the middle, print it and return
//        if (res == 0)
//        {
//            return SA[mid];
//        }
//
//        // Move to left half if pattern is alphabetically less than
//        // the mid suffix
//        if (res < 0) r = mid - 1;
//
//            // Otherwise move to right half
//        else l = mid + 1;
//    }
//
//    // We reach here if return statement in loop is not executed
//    return -1;
//}

int main() {
    int N;
    cin >> N;

    unordered_map<string, int> labels;
    vector<string> dict(N);
    stringstream s;
    vector<int> lenPrefixSum(N + 1);

    lenPrefixSum[0] = 0;
    for (int i = 0; i < N; ++i) {
        cin >> dict[i];
        labels[dict[i]] = i + 1;
    }

    sort(dict.begin(), dict.end());
    for (int k = 0; k < N; ++k) {
        lenPrefixSum[k + 1] = lenPrefixSum[k] + dict[k].length() + 1;
        s << dict[k] + "$";
    }

    SA_str = s.str();
    n = SA_str.length();
    constructSA();

    vector<string> suffixes;
    for (int x : SA) {
        suffixes.push_back(SA_str.substr(x));
    }

    int Q;
    cin >> Q;

    string q;
    for (int j = 0; j < Q; ++j) {
        cin >> q;
        auto it = lower_bound(SA.begin(), SA.end(), q, SA_less());

        int res = 0;
        //cout << SA_str.substr(SA[it]) << endl;
        //cout << "q = " << q << endl;

        while(SA_equals()(*it, q) && res < 10) {
            cout << *it << endl;
            cout << SA_str.substr(*it) << endl;
            int stringIndex = upper_bound(lenPrefixSum.begin(), lenPrefixSum.end(), *it) - lenPrefixSum.begin() - 1;
            cout << (res > 0 ? " " : "") << dict[stringIndex] << endl;

            it++;
            res++;
        }

        if(!res) cout << "-1";
        cout << endl;
    }


    sort(dict.begin(), dict.end());

    return 0;
}
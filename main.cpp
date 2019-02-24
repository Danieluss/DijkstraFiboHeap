#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <map>
#include <ctime>
#define forr(a,b,c) for(int c = a; c < b; ++c)

using namespace std;

template<class K, class T>
class FiboPriQueNode{

public:

    FiboPriQueNode(){}

    FiboPriQueNode(K key, T item){
        this->key = key;
        this->item = item;
        deg = 0;
    }

    K key;
    T item;

    int deg;

    FiboPriQueNode* next;
    FiboPriQueNode* prev;

    FiboPriQueNode* child;

    void merge(FiboPriQueNode* nx){
        if(nx != nullptr) {
            this->next->prev = nx->prev;
            nx->prev->next = this->next;
            this->next = nx;
            nx->prev = this;
        }
    }

    void cut(FiboPriQueNode* nx){
        if(nx != nullptr){
            nx->next->prev = nx->prev;
            nx->prev->next = nx->next;
            nx->next = nx->prev = nx;
            merge(nx);
        }
    }

    static FiboPriQueNode* newTree(K key, T item){

        auto ret = new FiboPriQueNode(key, item);
        ret->deg = 0;
        ret->child = nullptr;
        ret->next = ret;
        ret->prev = ret;
        return ret;
    }

};

//Fibonacci heap
template<class K, class T>
class FiboPriQue{
public:
    FiboPriQueNode<K, T>* mn;
    FiboPriQueNode<K, T>* trees;

public:

    FiboPriQue(){

        mn = nullptr;
        trees = nullptr;
    }

    T top(){
        if(mn != nullptr)
            return mn->item;
    }

    void pop(){

        if(trees == mn) {
            if (mn->next != trees){
                trees = mn->next;
            } else if(mn->child != nullptr) {
                trees = mn->child;
            } else {
                delete mn;
                trees = nullptr;
                mn = nullptr;
                return;
            }
        }
        if(trees != mn->child){
            mn->prev->next = mn->next;
            mn->next->prev = mn->prev;
            mn->prev->merge(mn->child);
        }
        map<int, vector<FiboPriQueNode<K, T>*>> degMV;

        auto tmp = trees;
        if(tmp != nullptr)
            do{
                degMV[tmp->deg].push_back(tmp);
                tmp = tmp->next;
            }while(tmp != trees);

        delete mn;

        for(auto &[key, val] : degMV){

            while(val.size() > 1){

                auto a = val.back();
                val.pop_back();
                auto b = val.back();
                val.pop_back();
                if(a->key > b->key){
                    auto tmp = a;
                    a = b;
                    b = tmp;
                }

                if(a->child == nullptr){
                    b->next->prev = b->prev;
                    b->prev->next = b->next;
                    b->next = b;
                    b->prev = b;
                    a->child = b;
                } else {
                    a->child->cut(b);
                }

                if(trees == b)
                    trees = a;

                degMV[key + 1].push_back(a);
                a->deg += 1;

            }
        }

        mn = trees;
        tmp = trees->next;

        while(tmp != trees){
            if(tmp->key < mn->key)
                mn = tmp;
            tmp = tmp->next;
        }
    }

    void push(K key, T item){

        if(trees == nullptr){
            trees = FiboPriQueNode<K, T>::newTree(key, item);
            mn = trees;
        } else {
            auto ins = FiboPriQueNode<K, T>::newTree(key, item);
            trees->merge(ins);
            if(key < mn->key)
                mn = ins;
        }
    }

    bool isEmpty(){

        if(trees == nullptr)
            return true;
        return false;
    }

};

class ExData {

public:

    string dt;

    ExData(){};

    ExData(string s){

        dt = s;
    }

    void printRep(){

        cout<<dt;
    }
};

void operator >>(ifstream& in, ExData& data){

    in>>data.dt;
}

template<class T>
class Node{

public:

    T* data;
};

template<class T>
class Graph{

    vector<Node<T>> nodesV;
    vector<vector<pair<int, double>>> edgesV;

public:

    void addNode(Node<T> nd){

        nodesV.push_back(nd);
    }

    void addEdge(int fr, int to, double w){

        edgesV[fr].push_back(pair<int, double>(to, w));
    }

    void loadFromFile(string fn){

        ifstream in;
        in.open(fn);
        int n;
        in>>n;

        forr(nodesV.size(), n, i){
            Node<T> newNd;
            newNd.data = new T;
            in>>*(newNd.data);
            addNode(newNd);
            edgesV.push_back({});
        }
        in>>n;
        int fr, to;
        double w;
        forr(0, n, i){
            in>>fr>>to>>w;
            addEdge(fr, to, w);
        }
        in.close();
    }

    void findShortestPath(int fr, int to){
        //int distMn;
        int cur;
        bool* vis = new bool[nodesV.size()];
        int* prev = new int[nodesV.size()];
        double* dist = new double[nodesV.size()];
        forr(0, nodesV.size(), i){
            vis[i] = false;
            dist[i] = numeric_limits<double>::max();
            prev[i] = -1;
        }

        dist[fr] = 0;
        FiboPriQue<double, int> q;
        q.push(0, fr);
        while(!q.isEmpty()){
            cur = q.top();
            q.pop();
            vis[cur] = true;
            int next;
            forr(0, edgesV[cur].size(), i){
                next = edgesV[cur][i].first;
                if(dist[next] > dist[cur] + edgesV[cur][i].second){
                    dist[next] = dist[cur] + edgesV[cur][i].second;
                    prev[next] = cur;
                    if(!vis[next])
                        q.push(dist[next], next);
                }
            }
            /*distMn = numeric_limits<int>::max();
            cur = fr;
            forr(0, nodesV.size(), i){
                if(!vis[i] && dist[i] < distMn){
                    distMn = dist[i];
                    cur = i;
                }
            }*/
        }

        if(dist[to] == INT_MAX){
            cout<<"Disconnected"<<endl;
        } else {
            cout<<"Cost: ";
            cout<<dist[to]<<endl;
            cout<<"Path:"<<endl;
            cur = to;
            while(cur != fr) {
                cout<<cur<<" <- ";
                cur = prev[cur];
            }
            cout<<fr<<endl;
        }

        delete[] dist;
        delete[] prev;
        delete[] vis;
    }

    void printNodes(){

        forr(0, nodesV.size(), i){
            cout<<i<<":\n";
            nodesV[i].data->printRep();
            cout<<endl;
        }
        cout<<endl;
    }

    void printEdges(){

        cout<<"[cost/suc]"<<endl;
        forr(0, edgesV.size(), i){
            //nodesV[i].data->printRep();
            cout<<i<<" ->\n";

            forr(0, edgesV[i].size(), j) {
                cout<<" "<<edgesV[i][j].second<<" ";
                //nodesV[edgesV[i][j].first].data->printRep();
                cout<<edgesV[i][j].first<<endl;
            }
            cout<<endl;
        }
        cout<<endl;
    }

};


int main() {
    Graph<ExData> g;
    g.loadFromFile("test.txt");
    g.printNodes();
    g.printEdges();
    g.findShortestPath(0, 7);
    return 0;
}
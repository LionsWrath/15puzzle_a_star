#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <string.h>
#include <iterator>
#include <vector>
#include <utility> 
#include <set>
#include <map>
#include <stdlib.h>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;

//setnode
typedef struct {
    std::string key;
    int f;
    int h;
} SetNode;

//node
typedef struct {
    int table[4][4];
    //std::string father;
    std::multiset<SetNode>::iterator set_it;
    int g;
} Node;

//Comparador set
struct nodecmp {
    bool operator() (const SetNode& lhs, const SetNode& rhs) const {
        if (lhs.f == rhs.f) return lhs.h < rhs.h;
        else return lhs.f < rhs.f;    
    }
};

typedef std::unordered_map<std::string, Node> umap;
typedef std::multiset<SetNode,nodecmp> mset;
typedef umap::iterator node_iterator;
typedef mset::iterator set_iterator;

typedef struct {
    mset open_set;
    umap open_nodes;

    void insert(SetNode * setnode, Node * node) {
        node->set_it = open_set.insert(*setnode);
        open_nodes[setnode->key] = *node; 
    }

    void erase(node_iterator it) {
        open_set.erase(it->second.set_it);
        open_nodes.erase(it->first);
    }

    node_iterator find(std::string key) {
        return open_nodes.find(key);
    }

    std::pair<Node,SetNode> get_min(){ 
        set_iterator set_it = open_set.begin();
        std::pair<Node,SetNode> minimo = std::make_pair(open_nodes[set_it->key], *set_it);
        open_nodes.erase(set_it->key);
        open_set.erase(set_it);
        return minimo;
    }

    bool empty() {
        return open_nodes.empty();
    }

    node_iterator end() { 
        return open_nodes.end();
    }
} OpenSet;

//Estado final T
int final_table[4][4] = { { 1,  2, 3,  4}, 
                          {12, 13, 14, 5}, 
                          {11,  0, 15, 6}, 
                          {10,  9,  8, 7} };


std::map<int,std::pair<int,int> > place_map = { { 1, {0,0}}, { 2, {0,1}}, { 3, {0,2}}, { 4, {0,3}},
                                      { 5, {1,3}}, { 6, {2,3}}, { 7, {3,3}}, { 8, {3,2}},
                                      { 9, {3,1}}, {10, {3,0}}, {11, {2,0}}, {12, {1,0}}, 
                                      {13, {1,1}}, {14, {1,2}}, {15, {2,2}}, { 0, {2,1}} };  

//hash function
std::string generate_key(int table[][4]) {
    std::string key = "";
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(table[i][j] < 10) key += "0";
            key += std::to_string(table[i][j]);
        }
    }
    return key;
}


int calc_dist(int table[][4], int i, int j) {
    int dist;

    std::pair<int,int> correct_place = place_map[table[i][j]];

    dist = abs(j - correct_place.second) + abs(correct_place.first - i);

    return dist;

}
bool isFinal(int table[][4]) {
    if (!memcmp(table, final_table, 4*4*sizeof(int))) return true;
    return false;
}

int heuristic_one(int table[][4]) {
    int fora = 0;
    for (int i=0; i<4; i++) 
        for (int j=0; j<4; j++)
            if (table[i][j] != final_table[i][j])
                fora++;
    return fora;
}

int heuristic_two(int table[][4]) {
    int last;
    int out = 0;
    std::vector<std::pair<int,int> > indexes = { {2,1}, {2,2}, {1,2}, {1,1}, 
                                       {1,0}, {2,0}, {3,0}, {3,1}, 
                                       {3,2}, {3,3}, {2,3}, {1,3}, 
                                       {0,3}, {0,2}, {0,1}, {0,0} };
    std::pair<int,int> index = indexes[indexes.size() - 1];
    indexes.pop_back();
    last = table[index.first][index.second];

    while (!indexes.empty()) {
        index = indexes[indexes.size() - 1];
        indexes.pop_back();

        if (0 != last) {
            if (last != 15 && ((last + 1) != table[index.first][index.second]))  
                out++;
            else if (last == 15 && (0 != table[index.first][index.second])) 
                out++;
        }

        last = table[index.first][index.second];
    }

    return out;
}

int heuristic_three(int table[][4]) {
    int total_dist = 0;
    for (int i=0; i<4; i++) 
        for (int j=0; j<4; j++)
            if (table[i][j] != final_table[i][j])
                total_dist += calc_dist(table, i ,j);
    return total_dist;
}

int heuristic_four(int table[][4]) {
    int h1 = heuristic_one(table);
    int h2 = heuristic_two(table);
    int h3 = heuristic_three(table);

    return 0.08*h1 + 0.02*h2 + 0.9*h3;
}

int heuristic_five(int table[][4]) {
    int h1 = heuristic_one(table);
    int h2 = heuristic_two(table);
    int h3 = heuristic_three(table);

    return std::max(h1,std::max(h2,h3));
}

void print_table(int table[][4]) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++)
            cout << table[i][j] << " ";
        cout << endl;
    }       
    cout << endl;
}

/*int backtrack(Node node, umap closed_nodes) {
    //Provavel causa do uso de memória excessiva é copia do conjunto na passagem 
    int aux = 0;
    if (!node.father.empty()) 
        aux = backtrack(closed_nodes.find(node.father)->second, closed_nodes);
    else return aux;
    return aux + 1;
}*/

Node create_sucessor(int father_table[][4], int i, int j, int i_s, int j_s) {
    Node suc;
    memcpy(suc.table, father_table, 4*4*sizeof(int));

    suc.table[i][j] = suc.table[i_s][j_s];
    suc.table[i_s][j_s] = 0;

    return suc;
}

umap generate_sucessors(Node *father) {
    umap sucessors;

    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            if (0 == father->table[i][j]) {
                if (i-1 >= 0) {
                    Node suc = create_sucessor(father->table, i, j, i-1, j);
                    sucessors[generate_key(suc.table)] = suc;
                }
                if (i+1 <= 3) {
                    Node suc = create_sucessor(father->table, i, j, i+1, j);
                    sucessors[generate_key(suc.table)] = suc;
                }
                if (j-1 >= 0) {
                    Node suc = create_sucessor(father->table, i, j, i, j-1);
                    sucessors[generate_key(suc.table)] = suc;
                }
                if (j+1 <= 3) {
                    Node suc = create_sucessor(father->table, i, j, i, j+1);
                    sucessors[generate_key(suc.table)] = suc;
                }
            }
    return sucessors;
}

void read_table(int table[][4]) {
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++) 
            cin >> table[i][j];
}

int a_star(int (*heuristic)(int[][4])) { 
    OpenSet open_nodes;
    umap closed_nodes;

    Node initial;
    SetNode set_initial;
    read_table(initial.table);
    //initial.father = "";
    initial.g = 0;

    set_initial.h = heuristic(initial.table);
    set_initial.f = set_initial.h;
    set_initial.key = generate_key(initial.table);

    open_nodes.insert(&set_initial, &initial);

    while (!open_nodes.empty()) {
        std::pair<Node,SetNode> minimo = open_nodes.get_min();
        closed_nodes[minimo.second.key] = minimo.first;
        if (isFinal(minimo.first.table)) return  minimo.first.g;
        umap sucessors = generate_sucessors(&(minimo.first));

        for (node_iterator it = sucessors.begin(); it != sucessors.end(); ++it) {  
            it->second.g = minimo.first.g + 1;
            //it->second.father = minimo.second.key;

            SetNode setnode;
            setnode.key = it->first;
            setnode.h = heuristic(it->second.table);
            setnode.f = setnode.h + it->second.g;

            node_iterator open_it = open_nodes.find(it->first);
            node_iterator closed_it = closed_nodes.find(it->first);
            if (open_it != open_nodes.end()) {
                if (it->second.g < open_it->second.g) { 
                    open_nodes.erase(open_it);
                    open_nodes.insert(&setnode,&(it->second));
                }
            } else if (closed_it != closed_nodes.end()) {
                if (it->second.g < closed_it->second.g) {
                    closed_nodes.erase(closed_it->first);
                    open_nodes.insert(&setnode,&(it->second));
                }
            } else {
                open_nodes.insert(&setnode,&(it->second));
            }
        }

    }
}

int main() {
    cout << a_star(heuristic_four) << endl;
    return 0;
}

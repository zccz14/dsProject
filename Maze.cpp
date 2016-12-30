#include <bits/stdc++.h>
using namespace std;

class Node {
  Node* parent;
  Node* findparent () {
    if(this == parent) {
      return this;
    } else {
      return this->parent = this->parent->findparent();
    }
  }
public:
  Node () {
    parent = this;
  }
  void merge (Node& that) {
    findparent()->parent = that.findparent();
  }
  bool operator== (Node& that) {
    return findparent() == that.findparent();
  }
  bool operator!= (Node& that) {
    return !(*this == that);
  }
};

class Edge {
  Node* s;
  Node* e;
public:
  int x;
  int y;
  Edge (Node* s, Node *e, int x, int y):
  s(s), e(e), x(x), y(y){}
  bool destroy () {
    if(*s != *e) {
      s->merge(*e);
      return true;
    }
    return false;
  }
};

template < class T >
class Array2d {
  T* data;
public:
  int width;
  int height;
  Array2d (int width, int height) : width(width), height(height) {
    data = new T[width * height];
  }
  ~Array2d () {
    delete[] data;
  }
  T* get (int x, int y) {
    return &data[y * width + x];
  }
  bool valid(int x, int y) {
    return 0 <= x && x < width && 0 <= y && y < height;
  }
};

void readInput (int& width, int& height) {
  cerr << "Enter width and height" << endl;
  cerr << "Width: ";
  cin >> width;
  cerr << "Height: ";
  cin >> height;
}

int main () {
  int width, height;
  readInput(width, height);
  assert(width > 0 && height > 0);

  Array2d<Node> nodes(width, height);
  Array2d<int> disp((width * 2) + 1, (height * 2) + 1);
  vector<Edge> edges;

  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      *disp.get(j * 2 + 1, i * 2 + 1) = 1;
    }
  }
  *disp.get(0, 1) = *disp.get(disp.width - 1, disp.height - 2) = 2;

  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width - 1; j++) {
      edges.emplace_back(nodes.get(j, i), nodes.get(j + 1, i), j * 2 + 2, i * 2 + 1);
    }
  }
  for(int i = 0; i < height - 1; i++) {
    for(int j = 0; j < width; j++) {
      edges.emplace_back(nodes.get(j, i), nodes.get(j, i + 1), j * 2 + 1, i * 2 + 2);
    }
  }

  random_shuffle(edges.begin(), edges.end());
  for(auto& e: edges) {
    if(e.destroy()) {
      *disp.get(e.x, e.y) = 1;
    }
  }

  Array2d< pair<int, int> > parent(disp.width, disp.height);
  Array2d<bool> visited(disp.width, disp.height);
  queue<pair<int, int>> bfsq;
  bfsq.emplace(0, 1);
  while(!bfsq.empty()) {
    pair<int, int> p = bfsq.front();
    bfsq.pop();
    if(p == make_pair(disp.width - 1, disp.height - 2)) break;
    *visited.get(p.first, p.second) = 1;
    for(int i = -1; i <= 1; i++) {
      if( parent.valid(p.first + i, p.second) && !*visited.get(p.first + i, p.second) && *disp.get(p.first + i, p.second) ) {
        *parent.get(p.first + i, p.second) = make_pair(p.first, p.second);
        bfsq.emplace(p.first + i, p.second);
      }
    }
    for(int i = -1; i <= 1; i++) {
      if( parent.valid(p.first, p.second + i) && !*visited.get(p.first, p.second + i) && *disp.get(p.first, p.second + i) ) {
        *parent.get(p.first, p.second + i) = make_pair(p.first, p.second);
        bfsq.emplace(p.first, p.second + i);
      }
    }
  }
  int curx = disp.width - 1, cury = disp.height - 2;
  while(make_pair(curx, cury) != make_pair(0, 1)){
    *disp.get(curx, cury) = 2;
    tie(curx, cury) = *parent.get(curx, cury);
  }

  for(int i = 0; i < disp.height; i++) {
    for(int j = 0; j < disp.width; j++) {
      cout << (*disp.get(j, i) == 1 ? " " : (*disp.get(j, i) == 2) ? "\e[0;31m█\e[0;0m" : "█");
    }
    cout << endl;
  }
}

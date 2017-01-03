#include <bits/stdc++.h>
using namespace std;

// Union-Find 节点
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

// 连接迷宫节点的边
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

// 泛化二维数组
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
  T& operator () (int x, int y) {
    return data[y * width + x];
  }
  bool valid(int x, int y) {
    return 0 <= x && x < width && 0 <= y && y < height;
  }
  void clear(const T& value) {
    for(int i = 0; i < width * height; i++) {
      data[i] = value;
    }
  }
};

enum dispType {
  WALL,
  PASSABLE,
  WAY,
};

// 读取输入
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
  // 检查输入合法性
  try {
    if(!(width > 0 && height > 0)) throw("Width and height must be greater than 0");
  } catch(const char* s) {
    printf("Error: %s\n", s);
    return 0;
  }

  // 分配空间
  // nodes: 迷宫中的节点
  // disp:  最终显示出的地图
  // edges: 连接节点的边
  Array2d<Node> nodes(width, height);
  Array2d<int> disp((width * 2) + 1, (height * 2) + 1);
  disp.clear(dispType::WALL);
  vector<Edge> edges;

  // 初始化地图
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      disp(j * 2 + 1, i * 2 + 1) = dispType::PASSABLE;
    }
  }
  disp(0, 1) = disp(disp.width - 1, disp.height - 2) = dispType::WAY;

  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width - 1; j++) {
      edges.emplace_back(&nodes(j, i), &nodes(j + 1, i), j * 2 + 2, i * 2 + 1);
    }
  }
  for(int i = 0; i < height - 1; i++) {
    for(int j = 0; j < width; j++) {
      edges.emplace_back(&nodes(j, i), &nodes(j, i + 1), j * 2 + 1, i * 2 + 2);
    }
  }

  // 随机连边使图最终成为树
  random_shuffle(edges.begin(), edges.end());
  for(auto& e: edges) {
    if(e.destroy()) {
      disp(e.x, e.y) = dispType::PASSABLE;
    }
  }

  // 通过BFS查找最短路径
  Array2d< pair<int, int> > parent(disp.width, disp.height);
  Array2d<bool> visited(disp.width, disp.height);
  visited.clear(0);
  queue<pair<int, int>> bfsq;
  bfsq.emplace(disp.width - 1, disp.height - 2);
  while(!bfsq.empty()) {
    pair<int, int> p = bfsq.front();
    bfsq.pop();
    if(p == make_pair(0, 1)) break;
    visited(p.first, p.second) = 1;
    for(int i = -1; i <= 1; i++) {
      if( parent.valid(p.first + i, p.second) && !visited(p.first + i, p.second) && disp(p.first + i, p.second) ) {
        parent(p.first + i, p.second) = make_pair(p.first, p.second);
        bfsq.emplace(p.first + i, p.second);
      }
    }
    for(int i = -1; i <= 1; i++) {
      if( parent.valid(p.first, p.second + i) && !visited(p.first, p.second + i) && disp(p.first, p.second + i) ) {
        parent(p.first, p.second + i) = make_pair(p.first, p.second);
        bfsq.emplace(p.first, p.second + i);
      }
    }
  }
  // 标记最短路径
  int curx = 0, cury = 1;
  while(make_pair(curx, cury) != make_pair(disp.width - 1, disp.height - 2)){
    disp(curx, cury) = dispType::WAY;
    tie(curx, cury) = parent(curx, cury);
  }

  // 打印迷宫
  for(int i = 0; i < disp.height; i++) {
    for(int j = 0; j < disp.width; j++) {
      cout << (disp(j, i) ? "  " : "██");
    }
    cout << endl;
  }
  // 等待用户确认后打印最短路
  getchar();
  getchar();
  for(int i = 0; i < disp.height; i++) {
    for(int j = 0; j < disp.width; j++) {
      cout << (disp(j, i) == dispType::PASSABLE ? "  " : (disp(j, i) == dispType::WAY) ? "\e[0;31m██\e[0;0m" : "██");
    }
    cout << endl;
  }
}

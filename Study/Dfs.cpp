#include <vector>
using namespace std;

/*
	adjacent[from, to]
	2차원 배열 행렬을 이용한 그래프 표현
	메모리 소모가 크지만, 빠른 접근이 가능
	정점은 적고, 간선이 많은 경우 이점이 있다.
*/
vector<vector<int>> adjacent = vector<vector<int>>({
	{0, 1, 0, 1, 0, 0},
	{1, 0, 1, 1, 0, 0},
	{0, 1, 0, 0, 0, 0},
	{0, 1, 0, 0, 1, 0},
	{0, 1, 0, 0, 0, 0},
	{0, 1, 0, 0, 1, 0},
});

const int VERTEX_COUNT = 6;
vector<bool> visited = vector<bool>(VERTEX_COUNT, false);

void Dfs(int here){
	visited[here] = true;

	for(int i=0; i<adjacent[here].size(); i++){
		int there = adjacent[here][i];
		if(visited[there] == false)
			Dfs(there);
	}
}

void DfsAll(){
	for(int i=0; i<adjacent.size(); i++){
		if(visited[i] == false)
			Dfs(i);
	}
}
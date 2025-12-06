#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <iomanip>
#include <set>
#include <map>

using namespace std;

const int INF = 1e9;

// --- Data Structures ---

struct Edge {
    int to;
    int weight;
};

class Graph {
    int V;
    vector<vector<Edge>> adj;
    vector<string> nodeLabels;
    map<string, int> labelToIndex;

public:
    Graph(int v, vector<string> labels) : V(v), nodeLabels(labels) {
        adj.resize(V);
        for(int i=0; i<V; ++i) labelToIndex[labels[i]] = i;
    }

    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w}); // Undirected
    }

    void addEdge(string u, string v, int w) {
        addEdge(labelToIndex[u], labelToIndex[v], w);
    }

    // Task 1: Adjacency Matrix
    void printAdjacencyMatrix() {
        cout << "--- Adjacency Matrix ---" << endl;
        cout << "   ";
        for (const string& l : nodeLabels) cout << setw(5) << l;
        cout << endl;

        for (int i = 0; i < V; ++i) {
            cout << setw(2) << nodeLabels[i] << " ";
            for (int j = 0; j < V; ++j) {
                int w = INF;
                if (i == j) w = 0;
                else {
                    for (auto& e : adj[i]) {
                        if (e.to == j) { w = e.weight; break; }
                    }
                }
                
                if (w == INF) cout << setw(5) << "INF";
                else cout << setw(5) << w;
            }
            cout << endl;
        }
        cout << endl;
    }

    // Task 2: BFS
    void BFS(string startLabel) {
        cout << "--- BFS from " << startLabel << " ---" << endl;
        int start = labelToIndex[startLabel];
        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << nodeLabels[u] << " ";

            // Sort neighbors for deterministic output (alphabetical preference)
            vector<int> neighbors;
            for (auto& e : adj[u]) neighbors.push_back(e.to);
            sort(neighbors.begin(), neighbors.end());

            for (int v : neighbors) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        cout << endl << endl;
    }

    // Task 2: DFS
    void DFS(string startLabel) {
        cout << "--- DFS from " << startLabel << " ---" << endl;
        int start = labelToIndex[startLabel];
        vector<bool> visited(V, false);
        stack<int> s;

        s.push(start);

        while (!s.empty()) {
            int u = s.top();
            s.pop();

            if (!visited[u]) {
                cout << nodeLabels[u] << " ";
                visited[u] = true;
            }

            // Collect neighbors
            vector<int> neighbors;
            for (auto& e : adj[u]) neighbors.push_back(e.to);
            // Sort reversed so smallest pops first
            sort(neighbors.rbegin(), neighbors.rend());

            for (int v : neighbors) {
                if (!visited[v]) {
                    s.push(v);
                }
            }
        }
        cout << endl << endl;
    }

    // Task 3: Dijkstra (Shortest Path)
    void Dijkstra(string startLabel) {
        cout << "--- Dijkstra Shortest Path from " << startLabel << " ---" << endl;
        int start = labelToIndex[startLabel];
        vector<int> dist(V, INF);
        vector<int> parent(V, -1);
        dist[start] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, start});

        while (!pq.empty()) {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue;

            for (auto& e : adj[u]) {
                if (dist[u] + e.weight < dist[e.to]) {
                    dist[e.to] = dist[u] + e.weight;
                    parent[e.to] = u;
                    pq.push({dist[e.to], e.to});
                }
            }
        }

        // Print results
        for (int i = 0; i < V; ++i) {
            cout << "To " << nodeLabels[i] << ": Dist = " << setw(2) << dist[i] << ", Path = ";
            if (dist[i] == INF) cout << "No Path";
            else printPath(i, parent);
            cout << endl;
        }
        cout << endl;
    }

    void printPath(int u, const vector<int>& parent) {
        if (u == -1) return;
        printPath(parent[u], parent);
        cout << nodeLabels[u] << " ";
    }

    // Task 3: Prim (MST)
    void Prim(string startLabel) {
        cout << "--- Prim MST from " << startLabel << " ---" << endl;
        int start = labelToIndex[startLabel];
        vector<int> key(V, INF);
        vector<int> parent(V, -1);
        vector<bool> inMST(V, false);
        int totalWeight = 0;

        key[start] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, start});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (inMST[u]) continue;
            inMST[u] = true;
            if (parent[u] != -1) {
                cout << "Edge: " << nodeLabels[parent[u]] << " - " << nodeLabels[u] << " Weight: " << key[u] << endl;
                totalWeight += key[u];
            }

            for (auto& e : adj[u]) {
                if (!inMST[e.to] && e.weight < key[e.to]) {
                    key[e.to] = e.weight;
                    parent[e.to] = u;
                    pq.push({key[e.to], e.to});
                }
            }
        }
        cout << "Total MST Weight: " << totalWeight << endl << endl;
    }

    // Task 4: Articulation Points Helper
    void APUtil(int u, vector<bool>& visited, vector<int>& disc, vector<int>& low, int& time, int parent, set<int>& ap) {
        int children = 0;
        visited[u] = true;
        disc[u] = low[u] = ++time;

        for (auto& e : adj[u]) {
            int v = e.to;
            if (v == parent) continue;

            if (visited[v]) {
                low[u] = min(low[u], disc[v]);
            } else {
                children++;
                APUtil(v, visited, disc, low, time, u, ap);
                low[u] = min(low[u], low[v]);
                if (parent != -1 && low[v] >= disc[u]) {
                    ap.insert(u);
                }
            }
        }
        if (parent == -1 && children > 1) {
            ap.insert(u);
        }
    }

    // Task 4: Find Articulation Points (Consistency Check)
    void findArticulationPoints(string startLabel) {
        cout << "--- Articulation Points (Starting DFS from " << startLabel << ") ---" << endl;
        // Even though start node changes DFS tree structure, AP set should remain same for connected component
        int start = labelToIndex[startLabel];
        vector<bool> visited(V, false);
        vector<int> disc(V, 0), low(V, 0);
        int time = 0;
        set<int> ap;

        // Ensure we cover the component containing 'start'
        if (!visited[start]) {
             APUtil(start, visited, disc, low, time, -1, ap);
        }

        // Output
        if (ap.empty()) cout << "None";
        else {
            for (int i : ap) cout << nodeLabels[i] << " ";
        }
        cout << endl << endl;
    }
};

int main() {
    // --- Graph 1 Initialization ---
    vector<string> labels1 = {"A", "B", "C", "D", "E", "F", "G", "H"};
    Graph g1(8, labels1);
    
    // Edges from Diagram 1
    g1.addEdge("A", "B", 4); g1.addEdge("A", "D", 6); g1.addEdge("A", "G", 7);
    g1.addEdge("B", "C", 12); g1.addEdge("B", "E", 9);
    g1.addEdge("C", "E", 1); g1.addEdge("C", "F", 2); g1.addEdge("C", "H", 10);
    g1.addEdge("D", "E", 13); g1.addEdge("D", "G", 2);
    g1.addEdge("E", "F", 5); g1.addEdge("E", "G", 14); g1.addEdge("E", "H", 8);
    g1.addEdge("F", "H", 3);

    cout << "=== EXPERIMENT PART 1, 2, 3 (Graph 1) ===" << endl;
    g1.printAdjacencyMatrix();
    g1.BFS("A");
    g1.DFS("A");
    g1.Dijkstra("A");
    g1.Prim("A");

    // --- Graph 2 Initialization ---
    cout << "=== EXPERIMENT PART 4 (Graph 2) ===" << endl;
    vector<string> labels2 = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};
    Graph g2(12, labels2);
    
    // Horizontal
    g2.addEdge("A", "B", 1); g2.addEdge("B", "C", 1); g2.addEdge("C", "D", 1);
    g2.addEdge("E", "F", 1); g2.addEdge("F", "G", 1); g2.addEdge("G", "H", 1);
    g2.addEdge("I", "J", 1); g2.addEdge("J", "K", 1); g2.addEdge("K", "L", 1);
    // Vertical
    g2.addEdge("A", "E", 1); g2.addEdge("B", "F", 1); g2.addEdge("C", "G", 1); g2.addEdge("D", "H", 1);
    g2.addEdge("E", "I", 1); g2.addEdge("F", "J", 1); g2.addEdge("G", "K", 1); g2.addEdge("H", "L", 1);
    // Diagonal
    g2.addEdge("E", "J", 1); g2.addEdge("F", "I", 1);
    g2.addEdge("F", "K", 1); g2.addEdge("G", "J", 1);
    g2.addEdge("G", "L", 1); g2.addEdge("H", "K", 1);

    // Test different start points
    g2.findArticulationPoints("A");
    g2.findArticulationPoints("F");
    g2.findArticulationPoints("L");

    return 0;
}

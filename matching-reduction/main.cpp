#include <bits/stdc++.h>

using namespace std;

const int MAX = 200005;
int n1, n2, m, q;

using vertex_t      = int;
using edge_number_t = int;

vector<pair<vertex_t, edge_number_t>> adjListGraph[MAX];   // adjListGraph[u] = {v, edgeIndex}

vector<vertex_t> matchL(MAX); // matches of Left  nodes
vector<vertex_t> matchR(MAX); // matches of Right nodes

bool dfs(vertex_t u, vector<bool>& visited) {
    if (visited[u]) return false;
    visited[u] = true;
    for (auto& [v, idx] : adjListGraph[u]) {
        if (matchR[v] == 0 || dfs(matchR[v], visited)) {
            matchL[u] = v;
            matchR[v] = u;
            return true;
        }
    }
    return false;
}

// Build min vertex cover
vector<bool> visL(MAX), visR(MAX);

void dfs_cover(vertex_t u) {

    visL[u] = true;
    for (auto& [v, idx] : adjListGraph[u]) {
        if (matchL[u] == v) 
            continue;

        if (!visR[v]) {
            visR[v] = true;
            if (matchR[v] && !visL[matchR[v]])
                dfs_cover(matchR[v]);
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n1 >> n2 >> m >> q;
    for (int i = 1; i <= m; ++i) {
        vertex_t u, v;
        cin >> u >> v;
        adjListGraph[u].emplace_back(v, i);
    }

    // Ford-Fulkerson
    int matching = 0;
    for (int u = 1; u <= n1; ++u) {
        vector<bool> visited(n1 + 1, false);
        if (dfs(u, visited)) {
            matching++;
        }
    }

    // Min vertex cover
    for (vertex_t u = 1; u <= n1; ++u) {
        if (matchL[u] == 0)
            dfs_cover(u);
    }

    vector<vertex_t> remVertex;
    vector<edge_number_t> remEdge;

    size_t sum = 0;

    for (vertex_t u = 1; u <= n1; ++u) {
        if (!visL[u]) {
            remVertex.push_back(u);

            // find corresponding edge
            for (auto& [v, idx] : adjListGraph[u]) {
                if (matchL[u] == v) {
                    remEdge.push_back(idx);
                    sum += idx;
                    break;
                }
            }
        }
    }

    for (vertex_t v = 1; v <= n2; ++v) {
        if (visR[v]) {
            remVertex.push_back(-v); // negative becaus vertex from right side

            // find corresponding edge
            auto u = matchR[v];
            for (auto& [vv, idx] : adjListGraph[u]) {
                if (vv == v) {
                    remEdge.push_back(idx);
                    sum += idx;
                    break;
                }
            }
        }
    }

    int removed = 0;

    while (q--) {
        int t;
        cin >> t;
        if (t == 1) {
            cout << 1 << '\n';
            cout << remVertex[removed] << '\n';
            sum -= remEdge[removed];
            ++removed;
            cout << sum << '\n';
            cout.flush();
        } else {
            cout << remEdge.size() - removed << '\n';
            for (int i = removed; i < (int)remEdge.size(); ++i)
                cout << remEdge[i] << " \n"[i+1 == remEdge.size()];
            cout.flush();
        }
    }
    
    return 0;
}

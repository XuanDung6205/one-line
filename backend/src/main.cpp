#include <bits/stdc++.h>
using namespace std;
/* gái gú phù du
waifu bất tử */

// ===== Shortcut Macros =====
#define ll long long
#define eb emplace_back
#define pb push_back
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define sz(x) ((int)(x).size())
#define repeat(i, n) for (int i = 0; i < n; i++)
#define F first
#define S second
#define pii pair<int, int>
#define vi vector<int>
#define vpi vector<pii>
#define yes cout << "YES\n"
#define no cout << "NO\n"
#define endl '\n'
#define debug(x) cerr << #x << " = " << x << endl;
#define debugFlag(x) cerr << x << ": checked !" << endl;
#define debug2(x, y) cerr << #x << " = " << x << ", " << #y << " = " << y << endl;
#define debug3(x, y, z) cerr << #x << " = " << x << ", " << #y << " = " << y << ", " << #z << " = " << z << endl;
template <typename T>
void print_All_Single_Value(T &loop)
{
    for (const auto &x : loop)
    {
        cerr << x << " ";
    }
    cerr << endl;
}
template <typename T>
void print_All_Nest_Single_Value(T &loop)
{
    for (const auto &x : loop)
    {
        for (const auto &y : x)
        {
            cerr << y << " ";
        }
        cerr << endl;
    }
    cerr << endl;
}
template <typename T>
void print_All_likePair_value(T &loop)
{
    for (const auto &x : loop)
    {
        cerr << "(" << x.first << " , " << x.second << ")" << endl;
    }
    cerr << endl;
}
// ======================================================================

bool check_Bac(int x, int y, vector<vector<bool>> &board)
{
    int cnt = 0, deg;
    bool isOk = true;
    board[x][y] = false; // vẫn tính cell hiện tại
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            if (board[i][j])
                continue; // bỏ qua đỉnh đã thăm/block
            deg = 0;

            // đi lên được
            if (i - 1 >= 0 && !board[i - 1][j])
                deg++;

            // đi xuống được
            if (i + 1 < board.size() && !board[i + 1][j])
                deg++;

            // đi sang trái được
            if (j - 1 >= 0 && !board[i][j - 1])
                deg++;

            // đi sang phải được
            if (j + 1 < board[i].size() && !board[i][j + 1])
                deg++;

            // đỉnh bị cô lập
            if (deg == 0)
            {
                isOk = false;
                break;
            }
            if (deg == 1)
            {
                cnt++;
                if (cnt > 2)
                {
                    isOk = false; // có nhiều hơn 2 đỉnh bậc 1
                    break;
                }
            }
        }
        if (!isOk)
            break;
    }

    board[x][y] = true; // trả về giá trị cũ
    return isOk;
}
void check_LienThong() {}

// check xem nếu đi vào ô (i, j) hiện tại có hợp lệ parity hay không
bool checkToMau(int &num_white_cell, int &num_black_cell)
{
    return abs(num_white_cell - num_black_cell) <= 1;
}

void find_dinhCat() {}
// ------------------
bool backTrackingDFS(int i, int j, int w, int b, vector<vector<bool>> &board, vector<pair<int, int>> &path)
{
    if (b + w == 0)
    {
        // lưu lại đường đi trong này
        path.emplace_back(i, j);
        return true;
    }

    bool isParity_ok = checkToMau(w, b);
    if (!isParity_ok)
    {
        return false; // quay lui
    }

    bool isBac_ok = check_Bac(i, j, board);
    if (!isBac_ok)
    {
        return false;
    }

    // còn kiểm tra liên thông, nặng hơn kiểm tra 2 cái trên nên đặt dưới cùng

    // đi lên được
    if (i - 1 >= 0 && !board[i - 1][j])
    {
        board[i - 1][j] = true;
        int tmpw = w;
        tmpw -= (i - 1 + j) % 2 == 0 ? 1 : 0;
        int tmpb = b;
        tmpb -= (i - 1 + j) % 2 == 1 ? 1 : 0;
        bool isOk = backTrackingDFS(i - 1, j, tmpw, tmpb, board, path);
        if (isOk)
        {
            path.emplace_back(i, j);
            return true; // tìm được đường đi, kết thúc tìm
        }
        board[i - 1][j] = false;
    }

    // đi xuống được
    if (i + 1 < board.size() && !board[i + 1][j])
    {
        board[i + 1][j] = true;
        int tmpw = w;
        tmpw -= (i + 1 + j) % 2 == 0 ? 1 : 0;
        int tmpb = b;
        tmpb -= (i + 1 + j) % 2 == 1 ? 1 : 0;
        bool isOk = backTrackingDFS(i + 1, j, tmpw, tmpb, board, path);
        if (isOk)
        {
            path.emplace_back(i, j);
            return true; // tìm được đường đi, kết thúc tìm
        }
        board[i + 1][j] = false;
    }

    // đi sang trái được
    if (j - 1 >= 0 && !board[i][j - 1])
    {
        board[i][j - 1] = true;
        int tmpw = w;
        tmpw -= (i + j - 1) % 2 == 0 ? 1 : 0;
        int tmpb = b;
        tmpb -= (i + j - 1) % 2 == 1 ? 1 : 0;
        bool isOk = backTrackingDFS(i, j - 1, tmpw, tmpb, board, path);
        if (isOk)
        {
            path.emplace_back(i, j);
            return true; // tìm được đường đi, kết thúc tìm
        }
        board[i][j - 1] = false;
    }

    // đi sang phải được
    if (j + 1 < board[i].size() && !board[i][j + 1])
    {
        board[i][j + 1] = true;
        int tmpw = w;
        tmpw -= (i + j + 1) % 2 == 0 ? 1 : 0;
        int tmpb = b;
        tmpb -= (i + j + 1) % 2 == 1 ? 1 : 0;
        bool isOk = backTrackingDFS(i, j + 1, tmpw, tmpb, board, path);
        if (isOk)
        {
            path.emplace_back(i, j);
            return true; // tìm được đường đi, kết thúc tìm
        }
        board[i][j + 1] = false;
    }

    return false;
}

// ===== Main Solve Function =====
void solve()
{
    int n, m;
    cin >> n >> m;
    int cur_i, cur_j;
    cin >> cur_i >> cur_j;
    int num_block_cell;
    cin >> num_block_cell;
    // vector<vector<bool>> block_cell_board(n, vector<bool>(m, false));
    // vector<vector<bool>> visited_board(n, vector<bool>(m, false));
    vector<vector<bool>> board(n, vector<bool>(m, false));
    vector<vector<int>> parity_board(n, vector<int>(m));

    int i_block, j_block;

    // (i + j)% 2 nếu 0 là trắng, nếu 1 là đen
    int num_white_cell = 0, num_black_cell = 0;
    num_white_cell += (m / 2 + (m % 2)) * ((n + (n % 2)) / 2); // dòng chẳn
    num_white_cell += (m / 2) * ((n - (n % 2)) / 2);           // dòng lẻ

    num_black_cell += (m / 2 + (m % 2)) * ((n - (n % 2)) / 2); // dòng lẻ
    num_black_cell += (m / 2) * ((n + (n % 2)) / 2);           // dòng chẳn

    pair<int, int> current = {cur_i, cur_j}; // để bên ngoài for tối ưu hiệu năng
    for (int i = 0; i < num_block_cell; i++)
    {
        cin >> i_block >> j_block;
        board[i_block][j_block] = true;
        (i_block + j_block) % 2 == 0 ? num_white_cell -= 1 : num_black_cell -= 1;
    }

    board[cur_i][cur_j] = true;

    // chưa dám gọi hàm
    cout << "ok" << endl;
}

int32_t main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int t = 1;
    // cin >> t; // uncomment nếu là nhiều test
    while (t--)
        solve();
}
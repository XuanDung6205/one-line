#include <cmath>
#include <exception>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include "crow_all.h"

using namespace std;
/* gái gú phù du
waifu bất tử */

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

bool check_LienThong(int start_i, int start_j, const vector<vector<bool>> &board, int w, int b)
{
    // Tạo 1 bản sao để loang không ảnh hưởng bàn cờ chính
    vector<vector<bool>> visited = board;

    int reachable_count = 0;
    queue<pair<int, int>> q;

    q.push({start_i, start_j});
    visited[start_i][start_j] = true;

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!q.empty())
    {
        auto [u, v] = q.front();
        q.pop();

        for (int k = 0; k < 4; k++)
        {
            int nu = u + dx[k];
            int nv = v + dy[k];

            // Nếu hợp lệ và chưa thăm
            if (nu >= 0 && nu < visited.size() && nv >= 0 && nv < visited[0].size() && !visited[nu][nv])
            {
                visited[nu][nv] = true;
                reachable_count++;
                q.push({nu, nv});
            }
        }
    }

    return reachable_count == (w + b);
}

// check xem nếu đi vào ô (i, j) hiện tại có hợp lệ parity hay không
bool checkToMau(int &num_white_cell, int &num_black_cell)
{
    return abs(num_white_cell - num_black_cell) <= 1;
}

void find_dinhCat() {}

// --- HÀM DEBUG TRẠNG THÁI BÀN CỜ TỪNG BƯỚC ---
// void print_Board_Debug(int cur_i, int cur_j, int w, int b, const vector<vector<bool>> &board)
// {
//     cerr << "\n========== DEBUG ĐỆ QUY ==========\n";
//     cerr << "-> Dang xet o: (" << cur_i << ", " << cur_j << ")\n";
//     cerr << "-> Con lai: w=" << w << ", b=" << b << "\n";
//     for (int r = 0; r < board.size(); r++)
//     {
//         for (int c = 0; c < board[r].size(); c++)
//         {
//             if (r == cur_i && c == cur_j)
//                 cerr << "C "; // Current
//             else if (board[r][c])
//                 cerr << "X "; // Visited/Block
//             else
//                 cerr << ". "; // Empty
//         }
//         cerr << "\n";
//     }
//     cerr << "==================================\n";
// }

// ------------------
bool backTrackingDFS(int i, int j, int w, int b, vector<vector<bool>> &board, vector<pair<int, int>> &path)
{
    // GỌI HÀM IN BÀN CỜ Ở MỖI BƯỚC ĐI VÀO
    // print_Board_Debug(i, j, w, b, board);

    if (b + w == 0)
    {
        // lưu lại đường đi trong này
        path.emplace_back(i, j);
        // cerr << "[+] TIM THAY DUONG DI!\n";
        return true;
    }

    bool isParity_ok = checkToMau(w, b);
    if (!isParity_ok)
    {
        // cerr << "[-] Loai: parity\n";
        return false; // quay lui (backtrack)
    }

    bool isBac_ok = check_Bac(i, j, board);
    if (!isBac_ok)
    {
        // cerr << "[-] Loai: deg\n";
        return false;
    }

    // còn kiểm tra liên thông, nặng hơn kiểm tra 2 cái trên nên đặt dưới cùng
    bool isLienThong_ok = check_LienThong(i, j, board, w, b);
    if (!isLienThong_ok)
    {
        // cerr << "[-] Loai: connect\n";
        return false;
    }

    // đi lên được
    if (i - 1 >= 0 && !board[i - 1][j])
    {
        // cout << "1"<< endl;
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

    // đi xuống được (visit)
    if (i + 1 < board.size() && !board[i + 1][j])
    {
        // cout << "2" << endl;
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
        // cout << "3" << endl;
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
        // cout << "4" << endl;
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

    // cerr << "[-] Khong con duong di nao hop le tu (" << i << ", " << j << ") -> BACKTRACK\n";
    return false;
}

// ===== Main Solve Function =====
pair<bool, vector<pair<int, int>>> solve(int n, int m, int cur_i, int cur_j, int num_block_cell, vector<pair<int, int>> blocks)
{
    // int n, m;
    // cin >> n >> m;
    // int cur_i, cur_j;
    // cin >> cur_i >> cur_j;
    // int num_block_cell;
    // cin >> num_block_cell;
    // vector<vector<bool>> block_cell_board(n, vector<bool>(m, false));
    // vector<vector<bool>> visited_board(n, vector<bool>(m, false));
    vector<vector<bool>> board(n, vector<bool>(m, false));
    // vector<vector<int>> parity_board(n, vector<int>(m));

    int i_block, j_block;

    // (i + j)% 2 nếu 0 là trắng, nếu 1 là đen
    int num_white_cell = 0, num_black_cell = 0;
    num_white_cell += (m / 2 + (m % 2)) * ((n + (n % 2)) / 2); // dòng chẳn
    num_white_cell += (m / 2) * ((n - (n % 2)) / 2);           // dòng lẻ

    num_black_cell += (m / 2 + (m % 2)) * ((n - (n % 2)) / 2); // dòng lẻ
    num_black_cell += (m / 2) * ((n + (n % 2)) / 2);           // dòng chẳn

    for (int i = 0; i < num_block_cell; i++)
    {
        i_block = blocks[i].first;
        j_block = blocks[i].second;
        board[i_block][j_block] = true;
        (i_block + j_block) % 2 == 0 ? num_white_cell -= 1 : num_black_cell -= 1;
    }
    // khởi tạo điểm bắt đầu ở đây để khỏi ăn loz từ đầu vào
    board[cur_i][cur_j] = true;
    ((cur_i + cur_j) % 2) == 0 ? num_white_cell-- : num_black_cell--;

    // for(int i = 0; i < board.size(); i++){
    //     for(int j = 0; j < board[i].size(); j++){
    //         cout << board[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    vector<pair<int, int>> path;
    bool canSolve = backTrackingDFS(cur_i, cur_j, num_white_cell, num_black_cell, board, path);
    return {canSolve, path}; // trả kết quả về cho frontend xử lý

    // for (int i = path.size() - 1; i >= 0; i--)
    // {
    //     cout << path[i].first << " " << path[i].second << endl;
    // }

    // for(int i = 0; i < board.size(); i++){
    //     for(int j = 0; j < board[i].size(); j++){
    //         cout << board[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // chưa dám gọi hàm
    // cout << "ok" << endl;
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/solve").methods(crow::HTTPMethod::Post)([](const crow::request &req)
                                                                  {
        auto data = crow::json::load(req.body);
        if (!data) return crow::response(400, "Invalid JSON body");

        try {
            // 1. Tách dữ liệu JSON ra các biến y như cin cũ của bạn
            int n = data["rows"].i();
            int m = data["cols"].i();
            int cur_i = data["start"][0].i();
            int cur_j = data["start"][1].i();
            
            vector<pair<int, int>> blocks;
            if (data.has("blocked")) {
                for (const auto& cell : data["blocked"]) {
                    blocks.push_back({cell[0].i(), cell[1].i()});
                }
            }
            int num_block_cell = blocks.size();

            // 2. GỌI HÀM SOLVE ĐÃ CHỈNH SỬA Ở TRÊN
            auto [canSolve, path] = solve(n, m, cur_i, cur_j, num_block_cell, blocks);

            // 3. Đóng gói kết quả gửi ngược lại Frontend
            crow::json::wvalue out;
            out["success"] = canSolve;
            crow::json::wvalue::list json_path;
            
            // Xử lý ghi ngược path y hệt như vòng lặp in cout của bạn
            for(int i = path.size() - 1; i >= 0 ; i--) {
                crow::json::wvalue point;
                point[0] = path[i].first;
                point[1] = path[i].second;
                json_path.push_back(std::move(point));
            }
            
            out["path"] = std::move(json_path);
            out["message"] = canSolve ? "Path found" : "No path found";
            return crow::response(out);
            
        } catch (const std::exception& e) {
            return crow::response(400, string("Bad request: ") + e.what());
        } });

    app.port(18080).multithreaded().run();
}
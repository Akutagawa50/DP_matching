#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <math.h>

using namespace std;

//読み込むファイル
string temp_file_dir = "city011";
string input_file_dir = "city022";

//斜め遷移の重み
const double dia_weight = 2.0;
//double dia_weight = 1.41421356;
//double dia_weight = 1.0;

vector<vector<double>> cumulative_dist(10, vector<double>(10)); //DPマッチングの累積距離を格納する

double cumulative_min[100] = {0}; //最小の累積距離を格納

int reco_number[100] = {0}; //一致したテンプレートの番号を格納

int reco_count = 0; //正答回数を格納

class data_info
{ //データの情報をまとめるクラス
public:
    data_info(){
        //pass
    };
    //~data_info();
    string filename;             //ファイルの名前
    string word;                 //単語
    int frame;                   //フレーム数
    vector<vector<double>> data; //データ
};

//テンプレートデータと入力データのクラスをそれぞれ100個宣言
vector<data_info> temp(100);
vector<data_info> input(100);

int main()
{
    //テンプレートのデータ入力, city011
    for (int i = 0; i < 100; i++)
    {
        string dir; //番号をcahr型で格納

        dir = char(((i + 1) / 100) + '0');                                          //100の位
        dir += char((((i + 1) % 100) / 10) + '0');                                  //10の位
        dir += char((((i + 1) % 100) % 10) + '0');                                  //1の位
        dir = "./data/" + temp_file_dir + "/" + temp_file_dir + "_" + dir + ".txt"; //ディレクトリを代入

        ifstream temp_file(dir);

        if (temp_file.fail()) //ファイル読み込みに失敗
        {
            cerr << "Failed to open file." << endl;
            return -1;
        }
        temp_file >> temp[i].filename; //ファイル名を入力
        temp_file >> temp[i].word;     //単語を入力
        temp_file >> temp[i].frame;    //フレーム数を入力

        temp[i].data.assign(temp[i].frame, vector<double>(15, 0)); //2次元配列を動的確保

        for (int j = 0; j < temp[i].frame; j++)
        {
            for (int k = 0; k < 15; k++)
                temp_file >> temp[i].data[j][k]; //データを入力
        }
        temp_file.close(); //ファイルを閉じる
    }

    //入力データ入力, city021
    for (int i = 0; i < 100; i++)
    {
        string dir; //番号をcahr型で格納

        dir = char(((i + 1) / 100) + '0');                                                  //100の位
        dir += char((((i + 1) % 100) / 10) + '0');                                          //10の位
        dir += char((((i + 1) % 100) % 10) + '0');                                          //1の位
        dir = dir = "./data/" + input_file_dir + "/" + input_file_dir + "_" + dir + ".txt"; //ディレクトリを代入

        ifstream input_file(dir);

        if (input_file.fail()) //ファイル読み込みに失敗
        {
            cerr << "Failed to open file." << endl;
            return -1;
        }
        input_file >> input[i].filename; //ファイル名を入力
        input_file >> input[i].word;     //単語を入力
        input_file >> input[i].frame;    //フレーム数を入力

        input[i].data.assign(input[i].frame, vector<double>(15, 0)); //2次元配列を動的確保

        for (int j = 0; j < input[i].frame; j++)
        {
            for (int k = 0; k < 15; k++)
            {
                input_file >> input[i].data[j][k];  //データを入力
            }
        }
        input_file.close(); //ファイルを閉じる
    }

    //認識
    for (int x = 0; x < 100; x++)
    { //入力データのfor
        for (int y = 0; y < 100; y++)
        {                                                                                //テンプレートのfor
            cumulative_dist.assign(temp[y].frame, vector<double>(input[x].frame, 10.0)); //2次元配列を動的確保, tempフレーム行 X inputフレーム列
            for (int i = 0; i < temp[y].frame; i++)
            { //累積距離を計算

                for (int j = 0; j < input[x].frame; j++)
                {
                    double sum = 0.0;
                
                    //次元の差の2乗和を合計を格納
                    for (int k = 0; k < 15; k++) //次元の差の2乗和を合計する
                        sum += pow((temp[y].data[i][k] - input[x].data[j][k]), 2.0);

                    double local_dist = sqrt(sum);

                
                    if (i == 0 && j == 0) //最初はそのまま代入
                        cumulative_dist[0][0] = local_dist;

                    else if (i == 0) //端は例外処理
                        cumulative_dist[0][j] = cumulative_dist[0][j - 1] + local_dist;

                    else if (j == 0)
                        cumulative_dist[i][0] = cumulative_dist[i - 1][0] + local_dist;

                    else
                    { //累積距離を上左斜めそれぞれ計算
                        double up, left, dia;
                        up = cumulative_dist[i - 1][j] + local_dist;                   //上方向の累積
                        left = cumulative_dist[i][j - 1] + local_dist;                 //左方向の累積
                        dia = cumulative_dist[i - 1][j - 1] + dia_weight * local_dist; //斜め方向の累積

                        cumulative_dist[i][j] = min({up, left, dia}); //最小値を累積距離に代入
                    }
                }
            }
            //一致率を計算
            double reco;
            reco = cumulative_dist[temp[y].frame - 1][input[x].frame - 1] / double(input[x].frame + temp[y].frame);

            if (reco < cumulative_min[x] || y == 0) //最小の累積距離を更新, 1回目はそのまま代入
            {
                cumulative_min[x] = reco; //最小の累積距離を代入
                reco_number[x] = y;       //最大の認識率の値を更新
            }
        }

        if (x == reco_number[x])
        { //合ってたら正答数を増やす
            reco_count++;
            //cout << "O: "; //〇を出力
        }
        else
        {
            cout << "X: "; //間違っていたら×を出力
            //結果を出力
            cout << "resutl of " + input[x].word + ": " << temp[reco_number[x]].word << ", Cumulative distance: " << cumulative_min[x] << endl;
        }
    }
    cout << "Recognition rate: " << reco_count << "%" << endl;
    return 0;
}

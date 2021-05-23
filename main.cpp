#define debug 1

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

std::string temp_fname[100];  //テンプレートのファイル名[データ番号]
std::string input_fname[100]; //入力データのファイル名語[データ番号]

std::string temp_word[100];  //テンプレートの語[データ番号]
std::string input_word[100]; //入力データの語[データ番号]

int temp_frame[100] = {0};  //テンプレートのフレーム数[データ番号]
int input_frame[100] = {0}; //入力データのフレーム数[データ番号]

const int max_frames = 140;

double temp_data[max_frames][15][100] = {0.0};  //テンプレートを格納する配列[フレーム][次元][データ番号]
double input_data[max_frames][15][100] = {0.0}; //入力データを格納する配列[フレーム][次元][データ番号]

double cumulative_dist[max_frames][max_frames] = {0}; //DPマッチングの累積距離を格納する[縦フレーム数][横フレーム数]

double cumulative_sum[100] = {0}; //認識率[入力データ番号]

int reco_number[100] = {0}; //一致したテンプレートの番号を格納

int reco_count = 0; //正答回数を格納

int main()
{

    //テンプレートのデータ入力, city011
    for (int i = 0; i < 100; i++)
    {
        std::string dir; //番号をcahr型で格納

        dir = char(((i + 1) / 100) + '0');              //100の位
        dir += char((((i + 1) % 100) / 10) + '0');      //10の位
        dir += char((((i + 1) % 100) % 10) + '0');      //1の位
        dir = "./data/city011/city011_" + dir + ".txt"; //ディレクトリを代入

        std::ifstream temp_file(dir);
        if (temp_file.fail()) //ファイル読み込みに失敗
        {
            std::cerr << "Failed to open file." << std::endl;
            return -1;
        }
        temp_file >> temp_fname[i]; //ファイル名を入力
        temp_file >> temp_word[i];  //単語を入力
        temp_file >> temp_frame[i]; //フレーム数を入力

        for (int j = 0; j < temp_frame[i]; j++)
        {
            for (int k = 0; k < 15; k++)
                temp_file >> temp_data[j][k][i]; //データを入力
        }
        temp_file.close(); //ファイルを閉じる
    }

    //入力データ入力, city021
    for (int i = 0; i < 100; i++)
    {
        std::string dir; //番号をcahr型で格納

        dir = char(((i + 1) / 100) + '0');              //100の位
        dir += char((((i + 1) % 100) / 10) + '0');      //10の位
        dir += char((((i + 1) % 100) % 10) + '0');      //1の位
        dir = "./data/city012/city012_" + dir + ".txt"; //ディレクトリを代入

        std::ifstream input_file(dir);
        if (input_file.fail()) //ファイル読み込みに失敗
        {
            std::cerr << "Failed to open file." << std::endl;
            return -1;
        }
        input_file >> input_fname[i]; //ファイル名を入力
        input_file >> input_word[i];  //単語を入力
        input_file >> input_frame[i]; //フレーム数を入力

        for (int j = 0; j < input_frame[i]; j++)
        {
            for (int k = 0; k < 15; k++)
            {
                input_file >> input_data[j][k][i]; //データを入力
            }
        }
        input_file.close(); //ファイルを閉じる
    }

    //認識
    for (int x = 0; x < 56; x++)
    { //入力データのfor
        for (int y = 0; y < 100; y++)
        { //テンプレートのfor

            for (int i = 0; i < temp_frame[y]; i++)
            { //累積距離を計算

                for (int j = 0; j < input_frame[x]; j++)
                {
                    double sum;
                    sum = 0.0;
                    //次元の差の2乗和を合計を格納
                    for (int k = 0; k < 15; k++) //次元の差の2乗和を合計する
                        sum += pow((temp_data[i][k] - input_data[j][k]), 2.0);

                    if (i == 0 && j == 0) //最初はそのまま代入
                        cumulative_dist[i][j] = sqrt(sum);

                    else if (i == 0) //端は例外処理
                        cumulative_dist[0][j] = cumulative_dist[0][j - 1] + sqrt(sum);

                    else if (j == 0)
                        cumulative_dist[i][0] = cumulative_dist[i - 1][0] + sqrt(sum);

                    else
                    { //累積距離を計算
                        double up, left, dia, local_min;
                        up = cumulative_dist[i - 1][j] + sqrt(sum);            //上方向の累積
                        left = cumulative_dist[i][j - 1] + sqrt(sum);          //左方向の累積
                        dia = cumulative_dist[i - 1][j - 1] + 2.0 * sqrt(sum); //斜め方向の累積

                        local_min = up;       //上方向を最小値に設定
                        if (left < local_min) //左方向と大小比較
                            left = local_min;
                        if (dia < local_min) //斜め方向と大小比較
                            dia = local_min;

                        cumulative_dist[i][j] = local_min; //最小値を累積距離に代入
                    }
                }
            }
            //一致率を計算
            double reco;
            reco = cumulative_dist[temp_frame[y] - 1][input_frame[x] - 1] / double(input_frame[x] + temp_frame[y]);

            if (reco < cumulative_sum[x] || y==0) //最小の累積距離を更新, 1回目はそのまま代入
            {
                cumulative_sum[x] = reco; //最小の累積距離を代入
                reco_number[x] = y;       //最大の認識率の値を更新
            }
        }
        //結果を出力
        std::cout << "resutl of " + input_word[x] + ": " << temp_word[reco_number[x]] << ", Cumulative distance: " << cumulative_sum[x] << std::endl;

        if (x == reco_number[x]) //合ってたら正答数を増やす
            reco_count++;
    }
    std::cout << "Recognition rate: " << reco_count << "%" << std::endl;
    return 0;
}